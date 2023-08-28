#include "Eval.h"
#include "../Functions.h"

#define AUTO_EVAL

static cv::Point g_clicked_video_point;
static cv::Point g_clicked_ortho_point;

static cv::Point2f g_clicked_video_point_org;
static cv::Point2f g_clicked_ortho_point_org;

static std::vector<cv::Point> g_selected_video_points;
static std::vector<cv::Point> g_correct_ortho_points;

static std::vector<cv::Point2f> g_selected_video_org_points;
static std::vector<cv::Point2f> g_correct_ortho_org_points;

//static const double g_RATIO = 2.5;
//static const double g_RATIO_REV = 0.4;
//static constexpr auto g_UI_CIRCLE_RADIUS = 2;
//static  constexpr auto g_VIDEO_CIRCLE_BLACK_RADIUS = 9;
//static  constexpr auto g_VIDEO_CIRCLE_RADIUS = 7;
//static  constexpr auto g_ORTHO_CIRCLE_BLACK_RADIUS = 12;
//static  constexpr auto g_ORTHO_CIRCLE_RADIUS = 10;

static const double g_RATIO = 0.5;
static const double g_RATIO_REV = 2.0;
static  constexpr auto g_VIDEO_CIRCLE_BLACK_RADIUS = 3;
static  constexpr auto g_VIDEO_CIRCLE_RADIUS = 1;
static  constexpr auto g_ORTHO_CIRCLE_BLACK_RADIUS = 5;
static  constexpr auto g_ORTHO_CIRCLE_RADIUS = 3;

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
	if (wnd_name == "ortho")
		cv::namedWindow(wnd_name, cv::WindowFlags::WINDOW_KEEPRATIO);
	else
		cv::namedWindow(wnd_name, cv::WindowFlags::WINDOW_GUI_EXPANDED);
	cv::setMouseCallback(wnd_name, mouse_callback);
}

static void input_points_interface(const std::string& video_code, const std::string& ortho_code, const cv::Mat& video_img, const cv::Mat& ortho_img)
{
	std::system("cls");
	set_window(video_code, recv_mouse_video_msg);
	set_window(ortho_code, recv_mouse_ortho_msg);

	while (true)
	{
		std::string input;
		std::cout << "input: continue? 'c'(click) : 'i'(key input) : others" << std::endl;
		std::cin >> input;

		if (input != "c" && input != "i")
			break;

		while (input == "c")
		{
			auto video_view = video_img.clone();
			for (const auto& point : g_selected_video_points)
			{
				cv::circle(video_view, point, g_VIDEO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
				cv::circle(video_view, point, g_VIDEO_CIRCLE_RADIUS, cv::Scalar(0, 255, 0), -1);
			}
			cv::imshow(video_code, video_view);

			auto ortho_view = ortho_img.clone();
			for (const auto& point : g_correct_ortho_points)
			{
				cv::circle(ortho_view, point, g_ORTHO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
				cv::circle(ortho_view, point, g_ORTHO_CIRCLE_RADIUS, cv::Scalar(0, 0, 255), -1);
			}
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
			g_selected_video_points.push_back(static_cast<cv::Point>(g_clicked_video_point_org * g_RATIO_REV));

			std::cout << "ortho -> input_x: " << std::endl;
			std::cin >> g_clicked_ortho_point_org.x;
			std::cout << "ortho -> input_y: " << std::endl;
			std::cin >> g_clicked_ortho_point_org.y;
			g_correct_ortho_org_points.push_back(g_clicked_ortho_point_org);
			g_correct_ortho_points.push_back(static_cast<cv::Point>(g_clicked_ortho_point_org * g_RATIO_REV));
		}
	}
}

static void input_select_points(const std::string& video_code, const std::string& ortho_code, const uint64_t& experiment_id)
{
	std::ifstream ifs(std::format("io_images/{}/{}_warp/select_points_code{}.txt", ortho_code, video_code, experiment_id));
	while (ifs.eof() == false)
	{
		std::string line;
		std::getline(ifs, line);

		std::vector<float> numbers;
		std::stringstream line_stream{ line };
		std::string number_txt;
		while (std::getline(line_stream, number_txt, ','))
			numbers.push_back(std::stof(number_txt));
		if (numbers.empty())
			continue;

		const cv::Point2f point(numbers[0], numbers[1]);
		g_selected_video_org_points.push_back(point);
		g_selected_video_points.push_back(static_cast<cv::Point>(point * g_RATIO_REV));
	}
}

static void input_correct_points(const std::string& video_code, const std::string& ortho_code, const uint64_t& experiment_id)
{
	std::ifstream ifs(std::format("io_images/{}/{}_warp/correct_points_code{}.txt", ortho_code, video_code, experiment_id));
	while (ifs.eof() == false)
	{
		std::string line;
		std::getline(ifs, line);

		std::vector<float> numbers;
		std::stringstream line_stream{ line };
		std::string number_txt;
		while (std::getline(line_stream, number_txt, ','))
			numbers.push_back(std::stof(number_txt));
		if (numbers.empty())
			continue;

		const cv::Point2f point(numbers[0], numbers[1]);
		g_correct_ortho_org_points.push_back(point);
		g_correct_ortho_points.push_back(static_cast<cv::Point>(point * g_RATIO_REV));
	}
}

static void output_select_points_code(const std::string& video_code, const std::string& ortho_code, const uint64_t& experiment_id)
{
	std::ofstream ofs(std::format("io_images/{}/{}_warp/final_select_points_code{}.txt", ortho_code, video_code, experiment_id));

	for (const auto& point : g_selected_video_org_points)
		ofs << std::format("g_selected_video_org_points.push_back(cv::Point2f({}, {}));", point.x, point.y) << std::endl;
}

static void output_correct_points_code(const std::string& video_code, const std::string& ortho_code, const uint64_t& experiment_id)
{
	std::ofstream ofs(std::format("io_images/{}/{}_warp/final_correct_points_code{}.txt", ortho_code, video_code, experiment_id));

	for (const auto& point : g_correct_ortho_org_points)
		ofs << std::format("g_correct_ortho_org_points.push_back(cv::Point2f({}, {}));", point.x, point.y) << std::endl;
}

static void draw_result_circle(cv::Mat& video_result_img, cv::Mat& ortho_result_img,
	const cv::Point& video_point, const cv::Point& correct_ortho_point, const cv::Point& transformed_pt)
{
	cv::circle(video_result_img, video_point, g_VIDEO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
	cv::circle(video_result_img, video_point, g_VIDEO_CIRCLE_RADIUS, cv::Scalar(0, 255, 0), -1);

	cv::circle(ortho_result_img, transformed_pt, g_ORTHO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
	cv::circle(ortho_result_img, transformed_pt, g_ORTHO_CIRCLE_RADIUS, cv::Scalar(0, 255, 0), -1);

	cv::circle(ortho_result_img, static_cast<cv::Point>(correct_ortho_point), g_ORTHO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
	cv::circle(ortho_result_img, static_cast<cv::Point>(correct_ortho_point), g_ORTHO_CIRCLE_RADIUS, cv::Scalar(0, 0, 255), -1);
}

static void reset_ui()
{
	g_selected_video_points.clear();
	g_selected_video_org_points.clear();
	g_correct_ortho_points.clear();
	g_correct_ortho_org_points.clear();
	cv::destroyAllWindows();
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
#ifdef AUTO_EVAL
	while (experiment_id < road_num)
#elif
	while (true)
#endif
	{
#ifndef AUTO_EVAL
		std::string input;
		std::cout << "Eval system: continue? 'y' : others" << std::endl;
		std::cin >> input;
		if (input != "y")
			break;
#endif

#ifdef AUTO_EVAL
		input_select_points(video_code, ortho_code, experiment_id);
		input_correct_points(video_code, ortho_code, experiment_id);
#elif
		input_points_interface(video_code, ortho_code, video_img, ortho_img);
#endif

		if (g_selected_video_org_points.size() != g_correct_ortho_org_points.size())
		{
			std::cout << "not executed: number of video_points must be number of ortho_points" << std::endl;
			experiment_id++;
			reset_ui();
			continue;
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

			draw_result_circle(video_result_img, ortho_result_img, video_point, correct_ortho_point, transformed_pt);
		}

		const auto eval_video_path = std::format("io_images/ortho/{}_eval/result{}_video.png", video_code, experiment_id);
		cv::imwrite(eval_video_path, video_result_img);
		const auto eval_ortho_path = std::format("io_images/ortho/{}_eval/result{}_ortho.png", video_code, experiment_id);
		cv::imwrite(eval_ortho_path, ortho_result_img);
		experiment_id++;
		reset_ui();
	}
}

void PreMethodEval::Run(const std::string& video_code, const std::string& ortho_code, const int& road_num, const float& meter_per_pix)
{
	std::vector hmg_altitude_points = {
		cv::Point2f(500.f, 790.f),
		cv::Point2f(550.f, 305.f),
		cv::Point2f(1190.f, 303.f),
		cv::Point2f(1450.f, 770.f)
	};
	std::vector hmg_ortho_points = {
		cv::Point2f(763.f, 1593.f),
		cv::Point2f(1595.f, 45.f),
		cv::Point2f(1770.f, 620.f),
		cv::Point2f(863.f, 2085.f)
	};

	const auto hmg_mat = cv::getPerspectiveTransform(hmg_altitude_points, hmg_ortho_points);

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
#ifdef AUTO_EVAL
	while (experiment_id < road_num)
#elif
	while (true)
#endif
	{
#ifndef AUTO_EVAL
		std::string input;
		std::cout << "Eval system: continue? 'y' : others" << std::endl;
		std::cin >> input;
		if (input != "y")
			break;
#endif

#ifdef AUTO_EVAL
		input_select_points(video_code, ortho_code, experiment_id);
		input_correct_points(video_code, ortho_code, experiment_id);
#elif
		input_points_interface(video_code, ortho_code, video_img, ortho_img);
#endif

		if (g_selected_video_org_points.size() != g_correct_ortho_org_points.size())
		{
			std::cout << "not executed: number of video_points must be number of ortho_points" << std::endl;
			experiment_id++;
			reset_ui();
			continue;
		}

		std::cout << "calculating....." << std::endl;
		std::cout << "************\n\n" << std::endl;

		auto video_result_img = video_org_img.clone();
		auto ortho_result_img = ortho_org_img.clone();

		const auto result_tsv_path = std::format("io_images/ortho/{}_eval/result_premethod{}.tsv", video_code, experiment_id);
		std::ofstream result_tsv_ofs(result_tsv_path);
		result_tsv_ofs << std::string(" altitude \t transed \tcorrect \t error_distance [m] ") << std::endl;

		std::vector<cv::Point2f> transformed_points;
		if (!g_selected_video_org_points.empty())
			cv::perspectiveTransform(g_selected_video_org_points, transformed_points, hmg_mat);

		const auto& points_num = g_selected_video_org_points.size();
		for (size_t point_id = 0; point_id < points_num; point_id++)
		{
			const auto video_point = static_cast<cv::Point>(g_selected_video_org_points[point_id]);
			const auto& correct_ortho_point = g_correct_ortho_org_points[point_id];
			const auto& transformed_pt = transformed_points[point_id];

			const auto misalignment_vec = (transformed_pt - correct_ortho_point) * meter_per_pix;
			const auto misalignment_norm = cv::norm(misalignment_vec);

			std::cout << "transformed_pt: " << transformed_pt << std::endl;
			std::cout << "misalignment: " << misalignment_norm << " [m]" << std::endl;
#ifdef _DEBUG
			std::cout << "misalignment: " << misalignment_vec << " (x_dir [m], y_dir [m])" << std::endl;
#endif

			std::cout << "\n\n************\n\n" << std::endl;

			result_tsv_ofs
				<< std::format(" {} \t {} \t {} \t {} ",
					std::format("({}, {})", video_point.x, video_point.y),
					std::format("({}, {})", transformed_pt.x, transformed_pt.y),
					std::format("({}, {})", correct_ortho_point.x, correct_ortho_point.y),
					misalignment_norm)
				<< std::endl;

			draw_result_circle(video_result_img, ortho_result_img, video_point, correct_ortho_point, transformed_pt);
			for (const auto& point : hmg_altitude_points)
			{
				cv::circle(video_img, point, g_VIDEO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
				cv::circle(video_img, point, g_VIDEO_CIRCLE_RADIUS, cv::Scalar(255, 0, 0), -1);
			}
			for (const auto& point : hmg_ortho_points)
			{
				cv::circle(ortho_img, point, g_ORTHO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
				cv::circle(ortho_img, point, g_ORTHO_CIRCLE_RADIUS, cv::Scalar(255, 0, 0), -1);
			}
		}

		const auto eval_video_path = std::format("io_images/ortho/{}_eval/result_premethod{}_video.png", video_code, experiment_id);
		cv::imwrite(eval_video_path, video_result_img);
		const auto eval_ortho_path = std::format("io_images/ortho/{}_eval/result_premethod{}_ortho.png", video_code, experiment_id);
		cv::imwrite(eval_ortho_path, ortho_result_img);
		experiment_id++;
		reset_ui();
	}
}