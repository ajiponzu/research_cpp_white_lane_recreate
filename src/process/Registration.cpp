#include "../Process.h"
#include "../Functions.h"
using namespace Func;

//#define OUTPUT_MESH_CENTER

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

		const auto pix = bin_img.at<uint8_t>(static_cast<cv::Point>(next_pos));
		if (pix == 0)
			break;

		scanner = next_pos;
	}
}

static void divide_area_by_white_lane(std::vector<Registration::MeshRect>& area_list,
	const Registration::MeshRect& corners, const Points4& white_lane)
{
	auto area_l = corners;
	area_l.tr = white_lane.mesh_rect.tl;
	area_l.br = white_lane.mesh_rect.bl;

	auto area_r = corners;
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

static Registration::BezierPoint calc_bezier_point(const std::vector<cv::Point2f>& bezier_control_points, const float& bezier_const_t)
{
	static constexpr auto calc_dir_unit_vector
		= [](const cv::Point2f& pt1, const cv::Point2f& pt2)
		{
			const auto dir_vec = pt1 - pt2;
			const auto norm = (float)cv::norm(dir_vec);
			return dir_vec / norm;
		};

	std::vector<cv::Point2f> new_points;
	for (size_t i = 0; i < (bezier_control_points.size() - 1); i++)
		new_points.push_back((1.0f - bezier_const_t) * bezier_control_points[i] + bezier_const_t * bezier_control_points[i + 1]);

	if (bezier_control_points.size() == 2)
		return Registration::BezierPoint(new_points[0], calc_dir_unit_vector(bezier_control_points[1], bezier_control_points[0]));
	else if (bezier_control_points.size() < 2)
		return Registration::BezierPoint();
	else
		return calc_bezier_point(new_points, bezier_const_t);
}

static void view_bezier(const std::vector<std::vector<cv::Point2f>>& bezier_control_points_list, const int& road_id)
{
	auto alt_img = cv::imread("outputs/hiru/background.bmp");

	static constexpr int s_bezier_itr_count = 10000;
	static constexpr float s_bezier_itr_count_rev = 1.0f / s_bezier_itr_count;

	for (int j = 0; j < 3; j++)
	{
		for (int i = 0; i <= s_bezier_itr_count; i++)
		{
			const auto bezier_alt_point = calc_bezier_point(bezier_control_points_list[j], s_bezier_itr_count_rev * i);
			cv::circle(alt_img, bezier_alt_point.point, 1, cv::Scalar(0, 0, 255), 1);
		}
	}

	cv::imwrite(std::format("outputs/alt_bezier_curve{}.bmp", road_id), alt_img);
}

void Registration::Registrator::Run(const std::string& video_code, const std::string& ortho_code)
{
	for (int road_id = 0; road_id < m_roadNum; road_id++)
	{
		DivideRoadByLane(video_code, road_id);
		DivideRoadByLane(ortho_code, road_id);
		CalcSpecificBezierPoint(video_code, road_id);
	}
	DrawRoadsByDividedArea(video_code, ortho_code);
}

void Registration::Registrator::DivideRoadByLane(const std::string& code, const int& road_id)
{
	m_pointsListHashes[road_id][code] = std::vector<MeshRect>();
	auto& pointsList = m_pointsListHashes[road_id][code];

	cv::Mat road_mask;
	cv::Mat lane_img;

	cv::cvtColor(ResourceProvider::GetRoadMask(std::format("{}_road_mask{}", code, road_id)), road_mask, cv::COLOR_BGR2GRAY);
	cv::cvtColor(ResourceProvider::GetProcessOutput(std::format("{}_lane{}", code, road_id)), lane_img, cv::COLOR_BGR2GRAY);

	auto lane_corners_result = lane_img.clone();
	cv::cvtColor(lane_corners_result, lane_corners_result, cv::COLOR_GRAY2BGR);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(lane_img, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

	const size_t bgn_idx = 0;
	const size_t end_idx = contours.size() - 1;
	cv::Scalar colors[2] = { cv::Scalar(0, 0, 255), cv::Scalar(0, 255, 0) };

	auto view_img = ResourceProvider::GetProcessOutput(std::format("{}_non_cars", code)).clone();

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

			MeshRect mesh_rect(points);
			const auto vec = mesh_rect.bl - mesh_rect.tl;
			mesh_rect.line_dir_vec = vec;
			mesh_rect.line_length = cv::norm(vec);
			pointsList.push_back(mesh_rect);
		}

		for (const auto& corners : road_corners_list)
		{
			corners.draw_pts(lane_corners_result, colors[i % 2]);
			pointsList.push_back(corners);
		}
	}
	cv::imwrite(std::format("outputs/{}/road_corners{}.bmp", code, road_id), lane_corners_result);
}

void Registration::Registrator::CalcSpecificBezierPoint(const std::string& video_code, const int& road_id)
{
	const auto& video_points_list = m_pointsListHashes[road_id][video_code];
	auto& bezier_points_list = m_bezierPointsList[road_id];
	bezier_points_list.resize(3);

	const auto points_num = static_cast<int>(video_points_list.size());
	int bezier_start_mesh_idx = 1;
	int bezier_end_mesh_idx = points_num - 2;
	std::vector<std::vector<cv::Point2f>> bezier_control_points_list(3);

	for (int mesh_idx = 0; mesh_idx < points_num; mesh_idx++)
	{
		auto& bezier_control_points = bezier_control_points_list[mesh_idx % 3];
		if (mesh_idx == bezier_start_mesh_idx)
			bezier_control_points.push_back((video_points_list[mesh_idx].bl + video_points_list[mesh_idx].br) / 2);
		else if (mesh_idx == bezier_end_mesh_idx)
			bezier_control_points.push_back((video_points_list[mesh_idx].tl + video_points_list[mesh_idx].tr) / 2);

		if (mesh_idx % 3 == 1)
			bezier_control_points.push_back(Img::calc_rect_center(video_points_list[mesh_idx].get_bounding_rect()));
		else if (mesh_idx % 3 == 0)
		{
			bezier_control_points.push_back(video_points_list[mesh_idx].bl);
			bezier_control_points.push_back(video_points_list[mesh_idx].tl);
		}
		else if (mesh_idx % 3 == 2)
		{
			bezier_control_points.push_back(video_points_list[mesh_idx].br);
			bezier_control_points.push_back(video_points_list[mesh_idx].tr);
		}
		bezier_points_list[mesh_idx % 3].push_back(Registration::BezierPoint());
	}

	static constexpr int s_bezier_itr_count = 1000;
	static constexpr float s_bezier_itr_count_rev = 1.0f / s_bezier_itr_count;
	static constexpr auto get_nearest_point
		= [](const cv::Point2f& base, const Registration::BezierPoint& pt1, const Registration::BezierPoint& pt2)
		{
			if (cv::norm(pt1.point - base) < cv::norm(pt2.point - base))
				return pt1;
			else
				return pt2;
		};

	auto alt_img = ResourceProvider::GetProcessOutput(std::format("{}_non_cars", video_code)).clone();

	for (int bezier_count = 0; bezier_count <= s_bezier_itr_count; bezier_count++)
	{
		const auto bezier_point = calc_bezier_point(bezier_control_points_list[1], s_bezier_itr_count_rev * bezier_count);
		for (int mesh_idx = 0, bezier_point_idx = 0; mesh_idx < points_num; mesh_idx++)
		{
			if (!video_points_list[mesh_idx].is_white_lane_mesh)
				continue;

			const auto bottom = Img::calc_line_center(video_points_list[mesh_idx].bl, video_points_list[mesh_idx].br);
			bezier_points_list[1][bezier_point_idx] = get_nearest_point(bottom, bezier_points_list[1][bezier_point_idx], bezier_point);
			bezier_point_idx++;

			const auto top = Img::calc_line_center(video_points_list[mesh_idx].tl, video_points_list[mesh_idx].tr);
			bezier_points_list[1][bezier_point_idx] = get_nearest_point(top, bezier_points_list[1][bezier_point_idx], bezier_point);
			bezier_point_idx++;
		}
	}

	static constexpr auto get_nearest_tangent_point
		= [](const Registration::BezierPoint& base, const Registration::BezierPoint& pt1, const Registration::BezierPoint& pt2)
		{
			if (std::abs(base.tangent_line_dir.dot(pt1.tangent_line_dir)) >
				std::abs(base.tangent_line_dir.dot(pt2.tangent_line_dir)))
				return pt1;
			else
				return pt2;
		};
	for (int bezier_count = 0; bezier_count <= s_bezier_itr_count; bezier_count++)
	{
		const auto bezier_point_left = calc_bezier_point(bezier_control_points_list[0], s_bezier_itr_count_rev * bezier_count);
		const auto bezier_point_right = calc_bezier_point(bezier_control_points_list[2], s_bezier_itr_count_rev * bezier_count);

		for (int bezier_point_idx = 0; bezier_point_idx < bezier_points_list[1].size(); bezier_point_idx++)
		{
			bezier_points_list[0][bezier_point_idx] = get_nearest_tangent_point(bezier_points_list[1][bezier_point_idx],
				bezier_points_list[0][bezier_point_idx], bezier_point_left);
			bezier_points_list[2][bezier_point_idx] = get_nearest_tangent_point(bezier_points_list[1][bezier_point_idx],
				bezier_points_list[2][bezier_point_idx], bezier_point_right);
		}
	}

	for (int bezier_point_idx = 0; bezier_point_idx < bezier_points_list[1].size(); bezier_point_idx++)
	{
		cv::circle(alt_img, bezier_points_list[0][bezier_point_idx].point, 1, cv::Scalar(0, 0, 255), -1);
		cv::circle(alt_img, bezier_points_list[1][bezier_point_idx].point, 1, cv::Scalar(0, 0, 255), -1);
		cv::circle(alt_img, bezier_points_list[2][bezier_point_idx].point, 1, cv::Scalar(0, 0, 255), -1);
	}

	cv::imwrite(std::format("outputs/alt_bezier_points{}.bmp", road_id), alt_img);
	view_bezier(bezier_control_points_list, road_id);
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

#ifdef OUTPUT_MESH_CENTER
	std::ofstream ofs_select(std::format("resources/eval/{}/select_points_code{}.txt", video_code, road_id));
	std::ofstream ofs_correct(std::format("resources/eval/{}/correct_points_code{}.txt", video_code, road_id));
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