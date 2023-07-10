#pragma once

namespace Func
{
	namespace Img
	{
		// get white-lane areas
		// color-image-segmentation by k-means
		cv::Mat get_white_lane(const cv::Mat& mat, const int& n_k = 3);

		// get shadow-area by lab statistic
		cv::Mat extract_shadow(const cv::Mat& mat);

		// apply with morphological method with shadow and get unshadow-area by bit_not
		cv::Mat get_unshadow(cv::Mat& shadow);

		// hmg_warp
		void warp_img_by_hmg(const cv::Mat& src, cv::Mat& dst, cv::Mat& hmg_layer,
			const std::vector<cv::Point2f>& src_pts, const std::vector<cv::Point2f>& dst_pts);
	};

	namespace GeoCvt
	{
		cv::Mat get_multicolor_mat(const std::string& path);

		cv::Mat get_float_tif(const std::string& path);
	};
};
