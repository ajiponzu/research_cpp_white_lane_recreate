#pragma once

class ResourceProvider
{
private:
	static cv::Mat s_orthoDsm;
	static cv::Mat s_orthoTif;
	static cv::Mat s_orthoCarMask;
	static std::unordered_map<std::string, cv::Mat> s_roadMaskHash;
	static std::unordered_map<std::string, cv::Mat> s_processOutputHash;

public:
	static void Init(const int& road_num, const std::string& video_code, const std::string& ortho_code);

	static const cv::Mat& GetOrthoDsm() { return s_orthoDsm; }
	static const cv::Mat& GetOrthoTif() { return s_orthoTif; }
	static const cv::Mat& GetOrthoCarMask() { return s_orthoCarMask; }
	static const cv::Mat& GetRoadMask(const std::string& road_mask_key) { return s_roadMaskHash[road_mask_key]; }
	static const cv::Mat& GetProcessOutput(const std::string& output_img_key) { return s_processOutputHash.at(output_img_key); }

	static void SetProcessOutput(const std::string& output_img_key, const cv::Mat& output_img)
	{
		s_processOutputHash[output_img_key] = output_img.clone();
	}

	ResourceProvider() = delete;
	bool operator==(const ResourceProvider& other) const = delete;
};