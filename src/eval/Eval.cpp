#include "Eval.h"
#include "../Functions.h"

static cv::Point g_clicked_video_point;
static cv::Point g_clicked_ortho_point;

static cv::Point2f g_clicked_video_point_org;
static cv::Point2f g_clicked_ortho_point_org;

static std::vector<cv::Point> g_selected_video_points;
static std::vector<cv::Point> g_correct_ortho_points;

static std::vector<cv::Point2f> g_selected_video_org_points;
static std::vector<cv::Point2f> g_correct_ortho_org_points;

static const double g_RATIO = 2.5;
static const double g_RATIO_REV = 0.4;

static void recv_mouse_msg(int event, int x, int y, int flag, void* callBack) {}

static void recv_mouse_video_msg(int event, int x, int y, int flag, void* callBack)
{
	switch (event)
	{
	case cv::EVENT_LBUTTONDOWN: // マウス左クリック
		g_clicked_video_point = cv::Point(x, y);
		g_clicked_video_point_org = static_cast<cv::Point2f>(g_clicked_video_point) * (float)g_RATIO;
		std::cout << "clicked_point on satta: " << g_clicked_video_point_org << std::endl;
		g_selected_video_points.push_back(g_clicked_video_point);
		g_selected_video_org_points.push_back(g_clicked_video_point_org);
		break;
	case cv::EVENT_RBUTTONDOWN: // マウス右クリック
		if (g_selected_video_points.empty())
			break;
		g_selected_video_points.pop_back();
		g_selected_video_org_points.pop_back();
		break;
	default:
		break;
	}
}

static void recv_mouse_ortho_msg(int event, int x, int y, int flag, void* callBack)
{
	switch (event)
	{
	case cv::EVENT_LBUTTONDOWN: // マウス左クリック
		g_clicked_ortho_point = cv::Point(x, y);
		g_clicked_ortho_point_org = static_cast<cv::Point2f>(g_clicked_ortho_point) * (float)g_RATIO;
		std::cout << "clicked_point on ortho: " << g_clicked_ortho_point_org << std::endl;
		g_correct_ortho_points.push_back(g_clicked_ortho_point);
		g_correct_ortho_org_points.push_back(g_clicked_ortho_point_org);
		break;
	case cv::EVENT_RBUTTONDOWN: // マウス右クリック
		if (g_correct_ortho_points.empty())
			break;
		g_correct_ortho_points.pop_back();
		g_correct_ortho_org_points.pop_back();
		break;
	default:
		break;
	}
}

static void set_window(const std::string& wnd_name, decltype(recv_mouse_msg) mouse_callback)
{
	cv::namedWindow(wnd_name);
	cv::setMouseCallback(wnd_name, mouse_callback);
}

void Eval::Run(const std::string& video_code, const std::string& ortho_code, const int& road_num, const float& meter_per_pix)
{
	cv::Mat transed_points_map = Func::GeoCvt::get_float_tif(std::format("io_images/{}/transed_points_map.tif", video_code));
	cv::Mat lanes_inf_map = Func::GeoCvt::get_float_tif(std::format("io_images/{}/lanes_inf_map.tif", video_code));

	auto video_img = cv::imread(std::format("io_images/{}/background.png", video_code));
	auto ortho_img = Func::GeoCvt::get_multicolor_mat(std::format("resources/{}/ortho.tif", ortho_code));

	auto video_org_img = video_img.clone();
	auto ortho_org_img = ortho_img.clone();

	const auto video_mask = cv::imread(std::format("resources/{}/road_mask.png", video_code));
	const auto ortho_mask = cv::imread(std::format("resources/{}/road_mask.png", ortho_code));

	cv::addWeighted(video_org_img, 0.85, video_mask, 0.15, 1.0, video_org_img);
	cv::addWeighted(ortho_org_img, 0.85, ortho_mask, 0.15, 1.0, ortho_org_img);

	cv::resize(video_org_img, video_img, cv::Size(), g_RATIO_REV, g_RATIO_REV);
	cv::resize(ortho_org_img, ortho_img, cv::Size(), g_RATIO_REV, g_RATIO_REV);

	uint64_t experiment_id = 0;
	while (true)
	{
		std::system("cls");
		std::string input;
		std::cout << "Eval system: continue? 'y' : others" << std::endl;
		std::cin >> input;
		if (input != "y")
			break;

		set_window(video_code, recv_mouse_video_msg);
		set_window(ortho_code, recv_mouse_ortho_msg);

		while (true)
		{
			std::cout << "input: continue? 'c'(click) : 'i'(key input) : others" << std::endl;
			std::cin >> input;

			if (input != "c" && input != "i")
				break;

			while (input == "c")
			{
				auto video_view = video_img.clone();
				for (const auto& point : g_selected_video_points)
					cv::circle(video_view, point, 2, cv::Scalar(0, 0, 255), -1);
				cv::imshow(video_code, video_view);

				auto ortho_view = ortho_img.clone();
				for (const auto& point : g_correct_ortho_points)
					cv::circle(ortho_view, point, 2, cv::Scalar(0, 255, 0), -1);
				cv::imshow(ortho_code, ortho_view);

				if (cv::waitKey(1) == 'q')
					break;
			}

			if (input == "i")
			{
				std::cout << "video -> input_x: " << std::endl;
				std::cin >> g_clicked_video_point_org.x;
				std::cout << "video -> input_y: " << std::endl;
				std::cin >> g_clicked_video_point_org.y;
				g_selected_video_org_points.push_back(g_clicked_video_point_org);
				g_selected_video_points.push_back(static_cast<cv::Point>(g_clicked_video_point_org / 2));

				std::cout << "ortho -> input_x: " << std::endl;
				std::cin >> g_clicked_ortho_point_org.x;
				std::cout << "ortho -> input_y: " << std::endl;
				std::cin >> g_clicked_ortho_point_org.y;
				g_correct_ortho_org_points.push_back(g_clicked_ortho_point_org);
				g_correct_ortho_points.push_back(static_cast<cv::Point>(g_clicked_ortho_point_org / 2));
			}
		}

		std::cout << "calculating....." << std::endl;
		std::cout << "************\n\n" << std::endl;
		auto video_result_img = video_org_img.clone();
		auto ortho_result_img = ortho_org_img.clone();

		const auto result_tsv_path = std::format("io_images/ortho/{}_eval/result{}.tsv", video_code, experiment_id);
		std::ofstream result_tsv_ofs(result_tsv_path);
		result_tsv_ofs << std::string(" altitude \t transed \tcorrect \t error_distance [m] ") << std::endl;

		const auto& points_num = g_selected_video_org_points.size();
		for (size_t point_id = 0; point_id < points_num; point_id++)
		{
			const auto video_point = static_cast<cv::Point>(g_selected_video_org_points[point_id]);
			const auto& correct_ortho_point = g_correct_ortho_org_points[point_id];

			const auto& pix = transed_points_map.at<cv::Vec4f>(video_point);
			if ((int)pix[3] < 128)
				continue;

			const auto transformed_pt = cv::Point2f(pix[0], pix[1]);
			const auto misalignment_vec = (transformed_pt - correct_ortho_point) * meter_per_pix;
			const auto misalignment_norm = cv::norm(misalignment_vec);

			std::cout << "trans_pix: " << pix << std::endl;
			std::cout << "transformed_pt: " << transformed_pt << std::endl;
			std::cout << "misalignment: " << misalignment_norm << " [m]" << std::endl;
#ifdef _DEBUG
			std::cout << "misalignment: " << misalignment_vec << " (x_dir [m], y_dir [m])" << std::endl;
			std::cout << "ortho_lanes_inf(lane_dir.x, lane_dir.y, lane_latio[ortho / video]): "
				<< lanes_inf_map.at<cv::Vec4f>(video_point) << std::endl;
#endif
			std::cout << "\n\n************\n\n" << std::endl;

			result_tsv_ofs
				<< std::format(" {} \t {} \t {} \t {} ",
					std::format("({}, {})", video_point.x, video_point.y),
					std::format("({}, {})", transformed_pt.x, transformed_pt.y),
					std::format("({}, {})", correct_ortho_point.x, correct_ortho_point.y),
					misalignment_norm)
				<< std::endl;

			cv::circle(video_result_img, video_point, 9, cv::Scalar(0, 0, 0), -1);
			cv::circle(video_result_img, video_point, 7, cv::Scalar(0, 255, 0), -1);

			cv::circle(ortho_result_img, transformed_pt, 12, cv::Scalar(0, 0, 0), -1);
			cv::circle(ortho_result_img, transformed_pt, 10, cv::Scalar(0, 255, 0), -1);

			cv::circle(ortho_result_img, static_cast<cv::Point>(correct_ortho_point), 12, cv::Scalar(0, 0, 0), -1);
			cv::circle(ortho_result_img, static_cast<cv::Point>(correct_ortho_point), 10, cv::Scalar(0, 0, 255), -1);
		}

		const auto eval_video_path = std::format("io_images/ortho/{}_eval/result{}_video.png", video_code, experiment_id);
		cv::imwrite(eval_video_path, video_result_img);
		const auto eval_ortho_path = std::format("io_images/ortho/{}_eval/result{}_ortho.png", video_code, experiment_id);
		cv::imwrite(eval_ortho_path, ortho_result_img);
		experiment_id++;

		g_selected_video_points.clear();
		g_selected_video_org_points.clear();
		g_correct_ortho_points.clear();
		g_correct_ortho_org_points.clear();
		cv::destroyAllWindows();
	}
}

static cv::Mat g_pre_method_transform_mat = cv::Mat();
static std::vector<cv::Point2f> g_transed_points;

void PreMethodEval::Run(const std::string& video_code, const std::string& ortho_code, const int& road_num, const float& meter_per_pix)
{
}