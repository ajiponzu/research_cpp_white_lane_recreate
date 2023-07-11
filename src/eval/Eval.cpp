#include "Eval.h"
#include "../Functions.h"

static cv::Point g_clicked_video_point;
static cv::Point g_clicked_ortho_point;

static cv::Point2f g_clicked_video_point_org;
static cv::Point2f g_clicked_ortho_point_org;

static double g_ratio = 2.0;
static double g_ratio_rev = 0.5;

static void recv_mouse_msg(int event, int x, int y, int flag, void* callBack) {}

static void recv_mouse_video_msg(int event, int x, int y, int flag, void* callBack)
{
	switch (event)
	{
	case cv::EVENT_LBUTTONDOWN: // マウス左クリック
		g_clicked_video_point = cv::Point(x, y);
		g_clicked_video_point_org = static_cast<cv::Point2f>(g_clicked_video_point) * (float)g_ratio;
		std::cout << "clicked_point on satta: " << g_clicked_video_point_org << std::endl;
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
		g_clicked_ortho_point_org = static_cast<cv::Point2f>(g_clicked_ortho_point) * (float)g_ratio;
		std::cout << "clicked_point on ortho: " << g_clicked_ortho_point_org << std::endl;
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

	auto ortho_org_img = ortho_img.clone();

	const auto video_mask = cv::imread(std::format("resources/{}/road_mask.png", video_code));
	const auto ortho_mask = cv::imread(std::format("resources/{}/road_mask.png", ortho_code));

	cv::addWeighted(video_img, 0.85, video_mask, 0.15, 1.0, video_img);
	cv::addWeighted(ortho_org_img, 0.85, ortho_mask, 0.15, 1.0, ortho_org_img);

	cv::resize(video_img, video_img, cv::Size(), g_ratio_rev, g_ratio_rev);
	cv::resize(ortho_org_img, ortho_img, cv::Size(), g_ratio_rev, g_ratio_rev);

	uint64_t experiment_id = 0;
	while (true)
	{
		std::string input;
		std::cout << "Eval system: continue? 'y' : 'i' : others" << std::endl;
		std::cin >> input;
		if (input != "y" && input != "i")
			break;

		set_window(video_code, recv_mouse_video_msg);
		set_window(ortho_code, recv_mouse_ortho_msg);

		while (input != "i")
		{
			auto video_view = video_img.clone();
			cv::circle(video_view, g_clicked_video_point, 2, cv::Scalar(0, 0, 255), -1);
			cv::imshow(video_code, video_view);

			auto ortho_view = ortho_img.clone();
			cv::circle(ortho_view, g_clicked_ortho_point, 2, cv::Scalar(0, 255, 0), -1);
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
			std::cout << "ortho -> input_x: " << std::endl;
			std::cin >> g_clicked_ortho_point_org.x;
			std::cout << "ortho -> input_y: " << std::endl;
			std::cin >> g_clicked_ortho_point_org.y;
		}

		std::cout << "calculating....." << std::endl;
		auto ortho_result_img = ortho_org_img.clone();

		const auto& pix = transed_points_map.at<cv::Vec4f>(static_cast<cv::Point>(g_clicked_video_point_org));
		if (pix[3] != 255.0f)
			continue;

		const auto transformed_pt = cv::Point2f(pix[0], pix[1]);
		const auto misalignment_vec = (transformed_pt - g_clicked_ortho_point_org) * meter_per_pix;
		const auto misalignment_norm = cv::norm(misalignment_vec);

		std::cout << "trans_pix: " << pix << std::endl;
		std::cout << "transformed_pt: " << transformed_pt << std::endl;
		std::cout << "misalignment: " << misalignment_vec << " (x_dir [m], y_dir [m])" << std::endl;
		std::cout << "misalignment: " << misalignment_norm << " [m]" << std::endl;
		std::cout << "ortho_lanes_inf(lane_dir.x, lane_dir.y, lane_latio[ortho / video]): "
			<< lanes_inf_map.at<cv::Vec4f>(static_cast<cv::Point>(g_clicked_video_point_org)) << std::endl;

		cv::circle(ortho_result_img, transformed_pt, 3, cv::Scalar(0, 0, 255), -1);
		cv::resize(ortho_result_img, ortho_result_img, cv::Size(), g_ratio_rev, g_ratio_rev);
		cv::circle(ortho_result_img, g_clicked_ortho_point, 2, cv::Scalar(0, 255, 0), -1);
		cv::imshow("ortho_ortho", ortho_result_img);
		cv::waitKey(0);

		cv::destroyAllWindows();
		const auto eval_path = std::format("io_images/ortho/{}_eval/result{}.png", video_code, experiment_id);
		cv::imwrite(eval_path, ortho_result_img);
		experiment_id++;
	}
}