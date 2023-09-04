#include "../Process.h"
#include "../Functions.h"
using namespace Func;

void LaneDetection::Video::ExtractCandidate(const std::string& video_code)
{
	const auto divided_path = std::format("outputs/{}/divided.bmp", video_code);
	const auto divided_shadow_path = std::format("outputs/{}/divided_shadow.bmp", video_code);
	const auto divided_unshadow_path = std::format("outputs/{}/divided_unshadow.bmp", video_code);
	const auto lane_path = std::format("outputs/{}/lane.bmp", video_code);

	cv::Mat road, road_shadow, road_unshadow, lane;
	const auto& road_mask = ResourceProvider::GetRoadMask(std::format("{}_road_mask", video_code));
	const auto& bg = ResourceProvider::GetProcessOutput(std::format("{}_non_cars", video_code));
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

	ResourceProvider::SetProcessOutput(std::format("{}_lane_candidate", video_code), lane);
}

void LaneDetection::Video::ChooseLanes(const std::string& video_code, const int& road_id)
{
	const auto result_path = std::format("outputs/{}/lane{}.bmp", video_code, road_id);

	const auto& bg = ResourceProvider::GetProcessOutput(std::format("{}_non_cars", video_code));
	const auto& road_mask = ResourceProvider::GetRoadMask(std::format("{}_road_mask{}", video_code, road_id));

	auto lane = ResourceProvider::GetProcessOutput(std::format("{}_lane_candidate", video_code)).clone();
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

	ResourceProvider::SetProcessOutput(std::format("{}_lane{}", video_code, road_id), lane_result);
}

void LaneDetection::Video::ChooseAllLanes(const std::string& video_code, const int& roads_num)
{
	for (int road_id = 0; road_id < roads_num; road_id++)
		ChooseLanes(video_code, road_id);
}

void LaneDetection::Ortho::ExtractCandidate(const std::string& code)
{
	const auto non_cars_path = std::format("outputs/{}/background.bmp", code);
	const auto divided_path = std::format("outputs/{}/divided.bmp", code);
	const auto lane_path = std::format("outputs/{}/lane.bmp", code);

	cv::Mat road, road_shadow, road_unshadow, lane;
	const auto& road_mask = ResourceProvider::GetRoadMask(std::format("{}_road_mask", code));
	const auto& ortho = ResourceProvider::GetOrthoTif();
	const auto& cars_mask = ResourceProvider::GetOrthoCarMask();
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

	cv::imwrite(non_cars_path, ortho - cars_mask);
	cv::imwrite(divided_path, road);
	cv::imwrite(lane_path, lane);

	ResourceProvider::SetProcessOutput(std::format("{}_non_cars", code), ortho - cars_mask);
	ResourceProvider::SetProcessOutput(std::format("{}_lane_candidate", code), lane);
}

void LaneDetection::Ortho::ChooseLanes(const std::string& code, const int& road_id)
{
	const auto result_path = std::format("outputs/{}/lane{}.bmp", code, road_id);

	const auto& ortho = ResourceProvider::GetProcessOutput(std::format("{}_non_cars", code));
	const auto& road_mask = ResourceProvider::GetRoadMask(std::format("{}_road_mask{}", code, road_id));

	auto lane = ResourceProvider::GetProcessOutput(std::format("{}_lane_candidate", code)).clone();
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
		cv::resize(lane_i, lane_i, lane_i.size() / 3);
		cv::imshow("gui", lane_i);
		const auto key = cv::waitKey(0);
		if (key == 'y')
			cv::fillConvexPoly(lane_result, contour, cv::Scalar(255, 255, 255));
	}

	cv::imwrite(result_path, lane_result);
	cv::destroyAllWindows();

	ResourceProvider::SetProcessOutput(std::format("{}_lane{}", code, road_id), lane_result);
}

void LaneDetection::Ortho::ChooseAllLanes(const std::string& code, const int& roads_num)
{
	for (int road_id = 0; road_id < roads_num; road_id++)
		ChooseLanes(code, road_id);
}