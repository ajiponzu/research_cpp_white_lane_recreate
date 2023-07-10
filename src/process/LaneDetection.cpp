#include "../Process.h"
#include "../Functions.h"
using namespace Func;

void LaneDetection::Video::ExtractCandidate(const std::string& video_code)
{
	const auto road_mask_path = std::format("resources/{}/road_mask.png", video_code);
	const auto bg_img_path = std::format("io_images/{}/background.png", video_code);
	const auto divided_path = std::format("io_images/{}/lane/divided.png", video_code);
	const auto divided_shadow_path = std::format("io_images/{}/lane/divided_shadow.png", video_code);
	const auto divided_unshadow_path = std::format("io_images/{}/lane/divided_unshadow.png", video_code);
	const auto lane_path = std::format("io_images/{}/lane.png", video_code);

	cv::Mat road, road_shadow, road_unshadow, lane;
	auto road_mask = cv::imread(road_mask_path);
	auto bg = cv::imread(bg_img_path);
	auto shadow = Func::Img::extract_shadow(bg);
	auto unshadow = Func::Img::get_unshadow(shadow);
	cv::bitwise_and(shadow, road_mask, shadow);
	cv::bitwise_and(unshadow, road_mask, unshadow);
	cv::bitwise_and(bg, shadow, road_shadow);
	cv::bitwise_and(bg, unshadow, road_unshadow);

	road_shadow = Img::get_white_lane(road_shadow, 4);
	road_unshadow = Img::get_white_lane(road_unshadow, 4);
	road = road_shadow + road_unshadow;
	cv::cvtColor(road, road, cv::COLOR_BGR2GRAY);
	lane = (road == cv::Scalar(255, 255, 255));
	cv::cvtColor(lane, lane, cv::COLOR_GRAY2BGR);

	cv::imwrite(divided_path, road);
	cv::imwrite(divided_shadow_path, road_shadow);
	cv::imwrite(divided_unshadow_path, road_unshadow);
	cv::imwrite(lane_path, lane);
}

void LaneDetection::Video::ChooseLanes(const std::string& video_code, const int& road_id)
{
	const auto bg_img_path = std::format("io_images/{}/background.png", video_code);
	const auto lane_img_path = std::format("io_images/{}/lane.png", video_code);
	const auto road_mask_path = std::format("resources/{}/road_mask{}.png", video_code, road_id);
	const auto result_path = std::format("io_images/{}/lane{}.png", video_code, road_id);

	auto bg = cv::imread(bg_img_path);
	auto lane = cv::imread(lane_img_path);
	auto road_mask = cv::imread(road_mask_path);

	cv::bitwise_and(lane, road_mask, lane);
	cv::cvtColor(lane, lane, cv::COLOR_BGR2GRAY);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat lane_result = cv::Mat::zeros(bg.size(), bg.type());
	cv::Mat lane_result_mini = lane_result.clone();

	cv::findContours(lane, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	std::cout << "please press 'y' key when you find correct lane." << std::endl;
	for (const auto& contour : contours)
	{
		cv::resize(lane_result, lane_result_mini, lane_result.size() / 4);
		cv::imshow("lane_result", lane_result_mini);

		const auto area = (int)cv::contourArea(contour);
		if (area < 2)
			continue;

		cv::Mat lane_i = cv::Mat::zeros(bg.size(), bg.type());
		cv::addWeighted(lane_i, 0.75, bg, 0.25, 1.0, lane_i);
		cv::fillConvexPoly(lane_i, contour, cv::Scalar(255, 255, 255));
		cv::resize(lane_i, lane_i, lane_i.size() / 2);
		cv::imshow("gui", lane_i);
		const auto key = cv::waitKey(0);
		if (key == 'y')
			cv::fillConvexPoly(lane_result, contour, cv::Scalar(255, 255, 255));
	}

	cv::imwrite(result_path, lane_result);
	cv::destroyAllWindows();
}

void LaneDetection::Video::ChooseAllLanes(const std::string& video_code, const int& roads_num)
{
	for (int road_id = 0; road_id < roads_num; road_id++)
		ChooseLanes(video_code, road_id);
}

void LaneDetection::Ortho::ExtractCandidate(const std::string& code)
{
	const auto road_mask_path = std::format("resources/{}/road_mask.png", code);
	const auto ortho_img_path = std::format("resources/{}/ortho.tif", code);
	const auto cars_mask_path = std::format("resources/{}/cars_mask.png", code);
	const auto divided_path = std::format("io_images/{}/lane/divided.png", code);
	const auto lane_path = std::format("io_images/{}/lane.png", code);

	cv::Mat road, road_shadow, road_unshadow, lane;
	auto road_mask = cv::imread(road_mask_path);
	auto ortho = GeoCvt::get_multicolor_mat(ortho_img_path);
	auto cars_mask = cv::imread(cars_mask_path);
	auto shadow = Func::Img::extract_shadow(ortho);
	auto unshadow = Func::Img::get_unshadow(shadow);
	cv::bitwise_and(shadow, road_mask, shadow);
	cv::bitwise_and(unshadow, road_mask, unshadow);
	cv::bitwise_and(ortho, shadow, road_shadow);
	cv::bitwise_and(ortho, unshadow, road_unshadow);
	road_shadow -= cars_mask;
	road_unshadow -= cars_mask;

	road_shadow = Img::get_white_lane(road_shadow, 4);
	road_unshadow = Img::get_white_lane(road_unshadow, 4);
	road = road_shadow + road_unshadow;
	cv::cvtColor(road, road, cv::COLOR_BGR2GRAY);
	lane = (road == cv::Scalar(255, 255, 255));
	cv::cvtColor(lane, lane, cv::COLOR_GRAY2BGR);

	cv::imwrite(divided_path, road);
	cv::imwrite(lane_path, lane);
}

void LaneDetection::Ortho::ChooseLanes(const std::string& code, const int& road_id)
{
	const auto ortho_img_path = std::format("resources/{}/ortho.tif", code);
	const auto road_mask_path = std::format("resources/{}/road_mask{}.png", code, road_id);
	const auto lane_img_path = std::format("io_images/{}/lane.png", code);
	const auto result_path = std::format("io_images/{}/lane{}.png", code, road_id);

	auto ortho = GeoCvt::get_multicolor_mat(ortho_img_path);
	auto lane = cv::imread(lane_img_path);
	auto road_mask = cv::imread(road_mask_path);

	cv::bitwise_and(lane, road_mask, lane);
	cv::cvtColor(lane, lane, cv::COLOR_BGR2GRAY);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::Mat lane_result = cv::Mat::zeros(ortho.size(), ortho.type());
	cv::Mat lane_result_mini = lane_result.clone();

	cv::findContours(lane, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	std::cout << "please press 'y' key when you find correct lane." << std::endl;
	for (const auto& contour : contours)
	{
		cv::resize(lane_result, lane_result_mini, lane_result.size() / 4);
		cv::imshow("lane_result", lane_result_mini);

		const auto area = (int)cv::contourArea(contour);
		if (area < 2)
			continue;

		cv::Mat lane_i = cv::Mat::zeros(ortho.size(), ortho.type());
		cv::addWeighted(lane_i, 0.75, ortho, 0.25, 1.0, lane_i);
		cv::fillConvexPoly(lane_i, contour, cv::Scalar(255, 255, 255));
		cv::resize(lane_i, lane_i, lane_i.size() / 2);
		cv::imshow("gui", lane_i);
		const auto key = cv::waitKey(0);
		if (key == 'y')
			cv::fillConvexPoly(lane_result, contour, cv::Scalar(255, 255, 255));
	}

	cv::imwrite(result_path, lane_result);
	cv::destroyAllWindows();
}

void LaneDetection::Ortho::ChooseAllLanes(const std::string& code, const int& roads_num)
{
	for (int road_id = 0; road_id < roads_num; road_id++)
		ChooseLanes(code, road_id);
}