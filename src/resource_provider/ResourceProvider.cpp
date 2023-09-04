#include "../ResourceProvider.h"
#include "../Functions.h"
using namespace Func;

cv::Mat ResourceProvider::s_orthoDsm;
cv::Mat ResourceProvider::s_orthoTif;
cv::Mat ResourceProvider::s_orthoCarMask;
std::unordered_map<std::string, cv::Mat> ResourceProvider::s_roadMaskHash;
std::unordered_map<std::string, cv::Mat> ResourceProvider::s_processOutputHash;

static cv::Mat RegistExistedProcessOutputsResource(const std::string& path)
{
	cv::Mat resource;

	if (std::filesystem::exists(path))
		resource = cv::imread(path);
	else
		std::cout << std::format("'{}' is not existed", path) << std::endl;

	return resource;
}

void ResourceProvider::Init(const int& road_num, const std::string& video_code, const std::string& ortho_code)
{
	const auto ortho_dem_path = std::format("resources/{}/dsm.tif", ortho_code);
	const auto ortho_tif_path = std::format("resources/{}/ortho.tif", ortho_code);
	const auto ortho_car_mask_path = std::format("resources/{}/cars_mask.png", ortho_code);

	s_orthoDsm = GeoCvt::get_float_tif(ortho_dem_path);
	s_orthoTif = GeoCvt::get_multicolor_mat(ortho_tif_path);
	s_orthoCarMask = cv::imread(ortho_car_mask_path);

	s_roadMaskHash[std::format("{}_road_mask", video_code)] = cv::imread(std::format("resources/{}/road_mask.png", video_code));
	s_roadMaskHash[std::format("{}_road_mask", ortho_code)] = cv::imread(std::format("resources/{}/road_mask.png", ortho_code));

	for (int road_id = 0; road_id < road_num; road_id++)
	{
		s_roadMaskHash[std::format("{}_road_mask{}", video_code, road_id)]
			= cv::imread(std::format("resources/{}/road_mask{}.png", video_code, road_id));
		s_roadMaskHash[std::format("{}_road_mask{}", ortho_code, road_id)]
			= cv::imread(std::format("resources/{}/road_mask{}.png", ortho_code, road_id));
	}

	s_processOutputHash[std::format("{}_non_cars", video_code)]
		= RegistExistedProcessOutputsResource(std::format("outputs/{}/background.bmp", video_code));
	s_processOutputHash[std::format("{}_non_cars", ortho_code)]
		= RegistExistedProcessOutputsResource(std::format("outputs/{}/background.bmp", ortho_code));
	s_processOutputHash[std::format("{}_lane_candidate", video_code)]
		= RegistExistedProcessOutputsResource(std::format("outputs/{}/lane.bmp", video_code));
	s_processOutputHash[std::format("{}_lane_candidate", ortho_code)]
		= RegistExistedProcessOutputsResource(std::format("outputs/{}/lane.bmp", ortho_code));

	for (int road_id = 0; road_id < road_num; road_id++)
	{
		s_processOutputHash[std::format("{}_lane{}", video_code, road_id)]
			= RegistExistedProcessOutputsResource(std::format("outputs/{}/lane{}.bmp", video_code, road_id));
		s_processOutputHash[std::format("{}_lane{}", ortho_code, road_id)]
			= RegistExistedProcessOutputsResource(std::format("outputs/{}/lane{}.bmp", ortho_code, road_id));
	}

#ifdef _DEBUG
	for (const auto& [key, value] : s_processOutputHash)
		std::cout << std::format("[p_o] key: {}, value_ch: {}", key, value.channels()) << std::endl;
	for (const auto& [key, value] : s_roadMaskHash)
		std::cout << std::format("[r_m] key: {}, value_ch: {}", key, value.channels()) << std::endl;
#endif
}