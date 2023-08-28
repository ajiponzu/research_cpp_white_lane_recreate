#include "../Process.h"
#include "../Functions.h"
using namespace Func;

//#define OUTPUT_MESH_CENTER

struct Points4
{
	Registration::PosVec pos_vec;

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
		pos_vec.output_pts();
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
			rotated_rect.points(&(pos_vec.bl));
		else
			pos_vec = Registration::PosVec(rotated_rect.boundingRect2f());

		create_tb_vectors();
		create_lr_vectors();
	}

	Points4 create_white_lane_rect() const
	{
		Points4 white_lane = *this;

		while (white_lane.which_use_vectors())
		{
			white_lane.pos_vec.arrange_pos();
			white_lane.create_tb_vectors();
			white_lane.create_lr_vectors();
		}

		const auto vec = white_lane.pos_vec.bl - white_lane.pos_vec.tl;
		white_lane.pos_vec.line_dir_vec = vec;
		white_lane.pos_vec.line_length = cv::norm(vec);

		return white_lane;
	}

private:
	void create_top_vector()
	{
		top_v = pos_vec.tr - pos_vec.tl;
		top_v = top_v / cv::norm(top_v);
		rev_top_v = cv::Point2f(0, 0) - top_v;
	}

	void create_btm_vector()
	{
		btm_v = pos_vec.br - pos_vec.bl;
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
		left_v = pos_vec.bl - pos_vec.tl;
		left_v = left_v / cv::norm(left_v);
		rev_left_v = cv::Point2f(0, 0) - left_v;
	}

	void create_right_vector()
	{
		right_v = pos_vec.br - pos_vec.tr;
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
		return pos_vec.which_use_vectors();
	}
};

static void search_road_corner(cv::Point2f& scanner, const cv::Point2f& vec, const cv::Mat& bin_img)
{
	while (true)
	{
		const auto next_pos = scanner + vec;

		const auto pix = bin_img.at<uint8_t>(static_cast<cv::Point>(next_pos));
		if (pix == 0)
			break;

		scanner = next_pos;
	}
}

static void divide_area_by_white_lane(std::vector<Registration::PosVec>& area_list,
	const Registration::PosVec& corners, const Points4& white_lane)
{
	auto area_l = corners;
	area_l.tr = white_lane.pos_vec.tl;
	area_l.br = white_lane.pos_vec.bl;

	auto area_r = corners;
	area_r.tl = white_lane.pos_vec.tr;
	area_r.bl = white_lane.pos_vec.br;

	area_l.line_dir_vec = area_r.line_dir_vec = white_lane.pos_vec.line_dir_vec;
	area_l.line_length = area_r.line_length = white_lane.pos_vec.line_length;

	area_list.push_back(area_l);
	area_list.push_back(white_lane.pos_vec);
	area_list.push_back(area_r);
}

static std::vector<Registration::PosVec> search_road_corners(const Points4& points, const cv::Mat& bin_img)
{
	Registration::PosVec ans;

	cv::Point2f scanner = points.pos_vec.tl;
	search_road_corner(scanner, points.rev_top_v, bin_img);
	ans.tl = scanner;

	scanner = points.pos_vec.tr;
	search_road_corner(scanner, points.top_v, bin_img);
	ans.tr = scanner;

	scanner = points.pos_vec.bl;
	search_road_corner(scanner, points.rev_btm_v, bin_img);
	ans.bl = scanner;

	scanner = points.pos_vec.br;
	search_road_corner(scanner, points.btm_v, bin_img);
	ans.br = scanner;

	std::vector<Registration::PosVec> answers;
	divide_area_by_white_lane(answers, ans, points);

	return answers;
}

void Registration::Registrator::DivideRoadByLane(const std::string& code, const int& road_id)
{
	m_pointsListHashes[road_id][code] = std::vector<PosVec>();
	auto& pointsList = m_pointsListHashes[road_id][code];

	const auto road_mask_path = std::format("resources/{}/road_mask{}.png", code, road_id);
	const auto lane_img_path = std::format("io_images/{}/lane{}.png", code, road_id);
	const auto road_mask = cv::imread(road_mask_path, cv::IMREAD_GRAYSCALE);
	const auto lane_img = cv::imread(lane_img_path, cv::IMREAD_GRAYSCALE);
	auto lane_result = lane_img.clone();
	cv::cvtColor(lane_result, lane_result, cv::COLOR_GRAY2BGR);
	auto lane_corners_result = lane_result.clone();

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(lane_img, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	const size_t bgn_idx = 0;
	const size_t end_idx = contours.size() - 1;
	cv::Scalar colors[2] = { cv::Scalar(0, 0, 255), cv::Scalar(0, 255, 0) };

	std::vector<Points4> white_lanes;
	for (size_t i = 0; i < contours.size(); i++)
	{
		const auto& contour = contours[i];
		const auto rotated_rect = cv::minAreaRect(contour);

		Points4 points;
		points.set_points(rotated_rect, true);
		white_lanes.push_back(points.create_white_lane_rect());
	}

	for (size_t i = 0; i < white_lanes.size(); i++)
	{
		const auto& white_lane = white_lanes[i];
		const auto road_corners_list = search_road_corners(white_lane, road_mask);

		auto last_idx = pointsList.size() - 3;
		for (size_t j = 0; i > 0 && j < 3; last_idx++, j++)
		{
			const auto& corners = road_corners_list[j];
			const auto& prev_corners = pointsList[last_idx];
			const std::vector points{ prev_corners.tl, corners.bl, corners.br, prev_corners.tr };

			PosVec pos_vec(points);
			const auto vec = pos_vec.bl - pos_vec.tl;
			pos_vec.line_dir_vec = vec;
			pos_vec.line_length = cv::norm(vec);
			pointsList.push_back(pos_vec);
		}

		for (const auto& corners : road_corners_list)
		{
			corners.draw_pts(lane_corners_result, colors[i % 2]);
			pointsList.push_back(corners);
		}
	}
	cv::imwrite(std::format("io_images/{}/road_corners{}.png", code, road_id), lane_corners_result);
}

void Registration::Registrator::DivideRoadsByLane(const std::string& code)
{
	for (int i = 0; i < m_roadNum; i++)
		DivideRoadByLane(code, i);
}

std::pair<cv::Mat, cv::Mat> Registration::Registrator::DrawRoadByDividedArea(const std::string& video_code, const std::string& ortho_code, const int& road_id)
{
	const auto& video_points_list = m_pointsListHashes[road_id][video_code];
	const auto& ortho_points_list = m_pointsListHashes[road_id][ortho_code];

	if (video_points_list.size() != ortho_points_list.size())
	{
		std::cout << "Maybe failed to divide road, because the number of found points is different..." << std::endl;
		std::cout << std::format("size: {}-{}\n", video_points_list.size(), ortho_points_list.size());
		return { cv::Mat(), cv::Mat() };
	}
	const auto points_num = static_cast<int>(video_points_list.size());

	const auto road_mask_path = std::format("resources/{}/road_mask{}.png", video_code, road_id);
	const auto ortho_img_path = std::format("resources/{}/ortho.tif", ortho_code);
	const auto dsm_img_path = std::format("resources/{}/dsm.tif", ortho_code);
	const auto bg_img_path = std::format("io_images/{}/background.png", video_code);

	const auto ortho_warp_path = std::format("io_images/{}/{}_warp/hmg_ortho_warp{}.png", ortho_code, video_code, road_id);
	const auto warp_layer_path = std::format("io_images/{}/{}_warp/hmg_warp_layer{}.png", ortho_code, video_code, road_id);

	const auto road_mask = cv::imread(road_mask_path);
	const auto ortho = GeoCvt::get_multicolor_mat(ortho_img_path);
	const auto dsm = GeoCvt::get_float_tif(dsm_img_path);
	const auto bg = cv::imread(bg_img_path);
	cv::Mat transed_points_map = cv::Mat::zeros(road_mask.size(), CV_32FC4);
	cv::Mat lanes_inf_map = cv::Mat::zeros(road_mask.size(), CV_32FC4);
	cv::Mat hmg_layer = cv::Mat::zeros(ortho.size(), ortho.type());
	cv::Mat hmg_warp_result = ortho.clone();

#ifdef OUTPUT_MESH_CENTER
	std::ofstream ofs_select(std::format("io_images/{}/{}_warp/select_points_code{}.txt", ortho_code, video_code, road_id));
	std::ofstream ofs_correct(std::format("io_images/{}/{}_warp/correct_points_code{}.txt", ortho_code, video_code, road_id));
#endif

	for (int i = 0; i < points_num; i++)
	{
		const auto src_pts = video_points_list[i].get_pt_list();
		const auto dst_pts = ortho_points_list[i].get_pt_list();
		Func::Img::warp_img_by_hmg(bg, hmg_warp_result, hmg_layer, src_pts, dst_pts);

		const auto hmg_mat = cv::getPerspectiveTransform(src_pts, dst_pts);
		const auto ortho_dir = ortho_points_list[i].line_dir_vec / ortho_points_list[i].line_length;
		const auto length_ratio = ortho_points_list[i].line_length / video_points_list[i].line_length;

		const auto rect = video_points_list[i].get_bounding_rect();
		cv::Mat contour_mask = cv::Mat::zeros(road_mask.size(), CV_8UC1);
		video_points_list[i].fill_convex(contour_mask, cv::Scalar(255));

#ifdef OUTPUT_MESH_CENTER
		const auto video_mesh_center_pos = Img::calc_rect_center(video_points_list[i].get_bounding_rect());
		const auto ortho_mesh_center_pos = Img::calc_rect_center(ortho_points_list[i].get_bounding_rect());
		ofs_select
			<< std::format("{},{}"
				, video_mesh_center_pos.x, video_mesh_center_pos.y)
			<< std::endl;
		ofs_correct
			<< std::format("{},{}"
				, ortho_mesh_center_pos.x, ortho_mesh_center_pos.y)
			<< std::endl;
#endif

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
					std::cout << std::format("road_id_{}, error_{}, cur_point: ({}, {}), dst_point: ({}, {})", road_id, i, cur_point.x, cur_point.y, dst_point.x, dst_point.y);
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
	int i = 0;
	auto [transed_points_map_img, lanes_inf_map_img] = DrawRoadByDividedArea(video_code, ortho_code, i);
	for (i = 1; i < m_roadNum; i++)
	{
		const auto& [transed_points_map, lanes_inf_map] = DrawRoadByDividedArea(video_code, ortho_code, i);
		transed_points_map_img += transed_points_map;
		lanes_inf_map_img += lanes_inf_map;
	}
	cv::imwrite(std::format("io_images/{}/transed_points_map.tif", video_code), transed_points_map_img);
	cv::imwrite(std::format("io_images/{}/lanes_inf_map.tif", video_code), lanes_inf_map_img);
}