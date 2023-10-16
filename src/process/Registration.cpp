#include "../Process.h"
using namespace Func;

struct Points4
{
	Registration::MeshRect mesh_rect;

	cv::Point2f top_v;
	cv::Point2f rev_top_v;
	cv::Point2f btm_v;
	cv::Point2f rev_btm_v;

	cv::Point2f left_v;
	cv::Point2f rev_left_v;
	cv::Point2f right_v;
	cv::Point2f rev_right_v;

	void output_pts() const
	{
		mesh_rect.output_pts();
	}

	void output_tb_vecs() const
	{
		std::cout << "top: " << top_v << rev_top_v << std::endl;
		std::cout << "bottom: " << btm_v << rev_btm_v << std::endl;
	}

	void output_lr_vecs() const
	{
		std::cout << "left: " << left_v << rev_left_v << std::endl;
		std::cout << "right: " << right_v << rev_right_v << std::endl;
	}

	void output_inf() const
	{
		output_pts();
		output_tb_vecs();
		output_lr_vecs();
	}

	// is_rotated: true=> rotated_rect points, false=> rect points
	void set_points(const cv::RotatedRect& rotated_rect, const bool& is_rotated)
	{
		if (is_rotated)
			rotated_rect.points(&(mesh_rect.bl));
		else
			mesh_rect = Registration::MeshRect(rotated_rect.boundingRect2f());

		create_tb_vectors();
		create_lr_vectors();
	}

	void set_points(const std::array<cv::Point2f, 4>& points)
	{
		std::memcpy(&(mesh_rect.bl), points.data(), sizeof(cv::Point2f) * points.size());

		create_tb_vectors();
		create_lr_vectors();
	}

	Points4 create_white_lane_rect() const
	{
		Points4 white_lane = *this;

		while (white_lane.which_use_vectors())
		{
			white_lane.mesh_rect.arrange_pos();
			white_lane.create_tb_vectors();
			white_lane.create_lr_vectors();
		}

		const auto vec = white_lane.mesh_rect.bl - white_lane.mesh_rect.tl;
		white_lane.mesh_rect.line_dir_vec = vec;
		white_lane.mesh_rect.line_length = cv::norm(vec);
		white_lane.mesh_rect.is_white_lane_mesh = true;

		return white_lane;
	}

private:
	void create_top_vector()
	{
		top_v = mesh_rect.tr - mesh_rect.tl;
		top_v = top_v / cv::norm(top_v);
		rev_top_v = cv::Point2f(0, 0) - top_v;
	}

	void create_btm_vector()
	{
		btm_v = mesh_rect.br - mesh_rect.bl;
		btm_v = btm_v / cv::norm(btm_v);
		rev_btm_v = cv::Point2f(0, 0) - btm_v;
	}

	void create_tb_vectors()
	{
		create_top_vector();
		create_btm_vector();
	}

	void create_left_vector()
	{
		left_v = mesh_rect.bl - mesh_rect.tl;
		left_v = left_v / cv::norm(left_v);
		rev_left_v = cv::Point2f(0, 0) - left_v;
	}

	void create_right_vector()
	{
		right_v = mesh_rect.br - mesh_rect.tr;
		right_v = right_v / cv::norm(right_v);
		rev_right_v = cv::Point2f(0, 0) - right_v;
	}

	void create_lr_vectors()
	{
		create_left_vector();
		create_right_vector();
	}

	// true: tb_vectors; norm(tb) < norm(lr), false: lr_vectors; not
	bool which_use_vectors() const
	{
		return mesh_rect.which_use_vectors();
	}
};

static void search_road_corner(cv::Point2f& scanner, const cv::Point2f& vec, const cv::Mat& bin_img)
{
	while (true)
	{
		const auto next_pos = scanner + vec;

		if (!Img::is_on_mask(bin_img, static_cast<cv::Point>(next_pos)))
			break;

		scanner = next_pos;
	}
}

static void divide_area_by_white_lane(std::vector<Registration::MeshRect>& area_list,
	const Registration::MeshRect& corners, const Points4& white_lane)
{
	auto area_l = Registration::MeshRect(corners);
	area_l.tr = white_lane.mesh_rect.tl;
	area_l.br = white_lane.mesh_rect.bl;

	auto area_r = Registration::MeshRect(corners);
	area_r.tl = white_lane.mesh_rect.tr;
	area_r.bl = white_lane.mesh_rect.br;

	area_l.line_dir_vec = area_r.line_dir_vec = white_lane.mesh_rect.line_dir_vec;
	area_l.line_length = area_r.line_length = white_lane.mesh_rect.line_length;

	area_list.push_back(area_l);
	area_list.push_back(white_lane.mesh_rect);
	area_list.push_back(area_r);
}

static std::vector<Registration::MeshRect> search_road_corners(const Points4& points, const cv::Mat& bin_img)
{
	Registration::MeshRect ans;

	cv::Point2f scanner = points.mesh_rect.tl;
	search_road_corner(scanner, points.rev_top_v, bin_img);
	ans.tl = scanner;

	scanner = points.mesh_rect.tr;
	search_road_corner(scanner, points.top_v, bin_img);
	ans.tr = scanner;

	scanner = points.mesh_rect.bl;
	search_road_corner(scanner, points.rev_btm_v, bin_img);
	ans.bl = scanner;

	scanner = points.mesh_rect.br;
	search_road_corner(scanner, points.btm_v, bin_img);
	ans.br = scanner;

	std::vector<Registration::MeshRect> answers;
	divide_area_by_white_lane(answers, ans, points);

	return answers;
}

void Registration::Registrator::Run(const std::string& video_code, const std::string& ortho_code)
{
	for (int road_id = 0; road_id < m_roadNum; road_id++)
	{
		DivideRoadByLane(video_code, road_id);
		DivideRoadByLane(ortho_code, road_id);
	}
	DrawRoadsByDividedArea(video_code, ortho_code);
}

void Registration::Registrator::DivideRoadByLane(const std::string& code, const int& road_id)
{
	m_meshListHashes[road_id][code] = std::vector<MeshRect>();
	auto& mesh_list = m_meshListHashes[road_id][code];

	cv::Mat road_mask;
	cv::Mat lane_img;

	cv::cvtColor(ResourceProvider::GetRoadMask(std::format("{}_road_mask{}", code, road_id)), road_mask, cv::COLOR_BGR2GRAY);
	cv::cvtColor(ResourceProvider::GetProcessOutput(std::format("{}_lane{}", code, road_id)), lane_img, cv::COLOR_BGR2GRAY);

	auto lane_corners_result = lane_img.clone();
	cv::cvtColor(lane_corners_result, lane_corners_result, cv::COLOR_GRAY2BGR);

	cv::Scalar colors[2] = { cv::Scalar(0, 0, 255), cv::Scalar(0, 255, 0) };

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(lane_img, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	std::vector<Points4> white_lanes;
	for (size_t contour_idx = 0; contour_idx < contours.size(); contour_idx++)
	{
		const auto& contour = contours[contour_idx];
		const auto rotated_rect = cv::minAreaRect(contour);

		Points4 points;
		points.set_points(rotated_rect, true);
		white_lanes.push_back(points.create_white_lane_rect());
	}

	for (size_t white_lane_idx = 0; white_lane_idx < white_lanes.size(); white_lane_idx++)
	{
		const auto white_lane = white_lanes[white_lane_idx];
		const auto road_corners_list = search_road_corners(white_lane, road_mask);

		auto last_idx = mesh_list.size() - 3;
		for (size_t road_corners_idx = 0; white_lane_idx > 0 && road_corners_idx < 3; last_idx++, road_corners_idx++)
		{
			const auto& corners = road_corners_list[road_corners_idx];
			const auto& prev_corners = mesh_list[last_idx];
			const std::vector points{ prev_corners.tl, corners.bl, corners.br, prev_corners.tr };

			MeshRect mesh_rect(points);
			const auto vec = mesh_rect.bl - mesh_rect.tl;
			mesh_rect.line_dir_vec = vec;
			mesh_rect.line_length = cv::norm(vec);
			mesh_list.push_back(mesh_rect);
		}

		for (const auto& corners : road_corners_list)
		{
			corners.draw_pts(lane_corners_result, colors[white_lane_idx % 2]);
			mesh_list.push_back(corners);
		}
	}
	cv::imwrite(std::format("outputs/{}/road_corners{}.bmp", code, road_id), lane_corners_result);
}

std::pair<cv::Mat, cv::Mat> Registration::Registrator::DrawRoadByDividedArea(const std::string& video_code, const std::string& ortho_code, const int& road_id)
{
	const auto& video_mesh_list = m_meshListHashes[road_id][video_code];
	const auto& ortho_mesh_list = m_meshListHashes[road_id][ortho_code];

	if (video_mesh_list.size() != ortho_mesh_list.size())
	{
		std::cout << "Maybe failed to divide road, because the number of found points is different..." << std::endl;
		std::cout << std::format("size: {}-{}\n", video_mesh_list.size(), ortho_mesh_list.size());
		return { cv::Mat(), cv::Mat() };
	}
	const auto mesh_num = static_cast<int>(video_mesh_list.size());

	const auto ortho_warp_path = std::format("outputs/{}/{}/hmg_ortho_warp{}.bmp", ortho_code, video_code, road_id);
	const auto warp_layer_path = std::format("outputs/{}/{}/hmg_warp_layer{}.bmp", ortho_code, video_code, road_id);

	const auto& road_mask = ResourceProvider::GetRoadMask(std::format("{}_road_mask{}", video_code, road_id));
	const auto& ortho = ResourceProvider::GetOrthoTif();
	const auto& dsm = ResourceProvider::GetOrthoDsm();
	const auto& bg = ResourceProvider::GetProcessOutput(std::format("{}_non_cars", video_code));
	cv::Mat transed_points_map = cv::Mat::zeros(road_mask.size(), CV_32FC4);
	cv::Mat lanes_inf_map = cv::Mat::zeros(road_mask.size(), CV_32FC4);
	cv::Mat hmg_layer = cv::Mat::zeros(ortho.size(), ortho.type());
	cv::Mat hmg_warp_result = ortho.clone();

	for (int mesh_idx = 0; mesh_idx < mesh_num; mesh_idx++)
	{
		const auto src_pts = video_mesh_list[mesh_idx].get_pt_list();
		const auto dst_pts = ortho_mesh_list[mesh_idx].get_pt_list();
		Func::Img::warp_img_by_hmg(bg, hmg_warp_result, hmg_layer, src_pts, dst_pts);

		const auto hmg_mat = cv::getPerspectiveTransform(src_pts, dst_pts);
		const auto ortho_dir = ortho_mesh_list[mesh_idx].line_dir_vec / ortho_mesh_list[mesh_idx].line_length;
		const auto length_ratio = ortho_mesh_list[mesh_idx].line_length / video_mesh_list[mesh_idx].line_length;

		const auto rect = video_mesh_list[mesh_idx].get_bounding_rect();
		cv::Mat contour_mask = cv::Mat::zeros(road_mask.size(), CV_8UC1);
		video_mesh_list[mesh_idx].fill_convex(contour_mask, cv::Scalar(255));

		for (int y = rect.y; y <= rect.br().y; y++)
		{
			for (int x = rect.x; x <= rect.br().x; x++)
			{
				const auto cur_point = cv::Point(x, y);
				if (!Img::is_on_mask(contour_mask, cur_point))
					continue;

				std::vector<cv::Point2f> dst_points, src_points{ static_cast<cv::Point2f>(cur_point) };
				cv::perspectiveTransform(src_points, dst_points, hmg_mat);
				const auto& dst_point = dst_points[0];
				if (!dst_point.inside(cv::Rect(0, 0, dsm.size().width, dsm.size().height)))
				{
					std::cout << "********" << std::endl;
					std::cout << std::format("road_id_{}, error_{}, cur_point: ({}, {}), dst_point: ({}, {})",
						road_id, mesh_idx, cur_point.x, cur_point.y, dst_point.x, dst_point.y);
					std::cout << hmg_mat << std::endl;
					continue;
				}
				const auto& dsm_z = dsm.at<float>(static_cast<cv::Point>(dst_point));
				// opencvでBGRAの順でTif保存 ⇒ GDALだとRGBAの順
				transed_points_map.at<cv::Vec4f>(cur_point) = cv::Vec4f(dsm_z, dst_point.y, dst_point.x, 255.0f); // 読み取りの際にx, y, z, マスクの順になるよう保存
				lanes_inf_map.at<cv::Vec4f>(cur_point) = cv::Vec4f((float)length_ratio, ortho_dir.y, ortho_dir.x, 255.0f); // 読み取りの際にdir.x, dir.y, ratio, マスクの順になるよう保存
			}
		}
	}

	cv::imwrite(ortho_warp_path, hmg_warp_result);
	cv::imwrite(warp_layer_path, hmg_layer);

	return { transed_points_map, lanes_inf_map };
}

void Registration::Registrator::DrawRoadsByDividedArea(const std::string& video_code, const std::string& ortho_code)
{
	int road_id = 0;
	auto [transed_points_map_img, lanes_inf_map_img] = DrawRoadByDividedArea(video_code, ortho_code, road_id);
	for (road_id = 1; road_id < m_roadNum; road_id++)
	{
		const auto& [transed_points_map, lanes_inf_map] = DrawRoadByDividedArea(video_code, ortho_code, road_id);
		transed_points_map_img += transed_points_map;
		lanes_inf_map_img += lanes_inf_map;
	}
	cv::imwrite(std::format("outputs/{}/transed_points_map.tif", video_code), transed_points_map_img);
	cv::imwrite(std::format("outputs/{}/lanes_inf_map.tif", video_code), lanes_inf_map_img);
}