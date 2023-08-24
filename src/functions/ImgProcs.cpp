#include "../Functions.h"
using namespace Func;

// get white-lane areas
// color-image-segmentation by k-means
cv::Mat Img::get_white_lane(const cv::Mat& mat, const int& n_k)
{
	cv::Mat data, result, km_labels, km_centers;

	mat.convertTo(data, CV_32F);
	data = data.reshape(3, (int)(data.total()));

	cv::kmeans(data, n_k, km_labels,
		cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 10, 1.0),
		1, cv::KMEANS_PP_CENTERS, km_centers);

	result = mat.clone();
	auto p_result = result.ptr<cv::Vec3b>(0);
	auto p_km_labels = km_labels.ptr<int>(0);
	auto p_km_centers = km_centers.ptr<cv::Vec3f>(0);

	for (size_t i = 0; i < result.total(); i++)
	{
		cv::Vec3b color = p_km_centers[p_km_labels[i]]; // 各クラスタにおける色空間上の中心色を利用
		p_result[i][0] = color[0];
		p_result[i][1] = color[1];
		p_result[i][2] = color[2];
	}

	int max_mean = 0, max_mean_idx = 0, min_mean = 256, min_mean_idx = 0;
	std::vector<int> center_lum_list;
	for (int i = 0; i < n_k; i++)
	{
		cv::Vec3b color = p_km_centers[i];
		const auto& mean = (int)(cv::mean(color)[0]);
		center_lum_list.push_back(mean);

		if (max_mean < mean)
		{
			max_mean = mean;
			max_mean_idx = i;
		}
		else if (min_mean > mean)
		{
			min_mean = mean;
			min_mean_idx = i;
		}
	}
	std::sort(center_lum_list.begin(), center_lum_list.end());
	const auto white_metrics = max_mean - center_lum_list[(size_t)n_k - 2];
	const auto is_white = white_metrics > 40 && max_mean > 130;

	const cv::Vec3b lane_color = is_white ? p_km_centers[max_mean_idx] : cv::Vec3f(255.0f, 255.0f, 255.0f);
	const cv::Vec3b bg_color = p_km_centers[min_mean_idx];
	auto pix_lamda = [&](cv::Vec3b& pix, const int* pos)
		{
			auto new_color = cv::Vec3b(128, 128, 128);
			if (pix == lane_color)
				new_color = cv::Vec3b(255, 255, 255);
			else if (pix == bg_color)
				new_color = cv::Vec3b(0, 0, 0);

			pix = new_color;
		};
	result.forEach<cv::Vec3b>(pix_lamda);

	return result;
}

// get shadow-area by lab statistic
cv::Mat Img::extract_shadow(const cv::Mat& mat)
{
	// [0], [1], [2]にl, a, bが分割して代入される動的配列
	std::vector<cv::Mat> lab_vec;
	cv::Mat lab, gray_color = cv::Mat::ones(mat.size(), CV_8UC1) * 128;

	cv::cvtColor(mat, lab, cv::COLOR_BGR2Lab); //l*a*b*に変換
	cv::split(lab, lab_vec); //split: チャンネルごとに分割する関数

	/* 参照型でリソース削減しつつ, わかりやすいエイリアスを定義 */
	auto& l = lab_vec[0];
	auto& a = lab_vec[1];
	auto& b = lab_vec[2];
	/* end */

	/* 統計量導出 */
	cv::Scalar mean_l_scalar, std_l_scalar;
	cv::meanStdDev(l, mean_l_scalar, std_l_scalar);
	const auto& mean_a = cv::mean(a)[0];
	const auto& mean_b = cv::mean(b)[0];
	const auto& mean_l = mean_l_scalar[0];
	const auto& std_l = std_l_scalar[0];
	/* end */

	/* L値を決定する処理 */
	if ((mean_a + mean_b) <= 256)
	{
		auto thr = mean_l - std_l / 3;
		l = (l <= thr);
	}
	else
		l = (l <= 10).mul(b <= 10);
	/* end */

	/* a, b値を128で埋めてグレースケール化 */
	a = gray_color;
	b = gray_color;
	/* end */

	/* 統合処理 */
	cv::Mat shadow;
	cv::merge(lab_vec, lab);
	cv::cvtColor(lab, shadow, cv::COLOR_Lab2BGR);
	/* end */

	return shadow;
}

// apply with morphological method with shadow and get unshadow-area by bit_not
cv::Mat Img::get_unshadow(cv::Mat& shadow)
{
	cv::cvtColor(shadow, shadow, cv::COLOR_BGR2GRAY);

	std::vector<std::vector<cv::Point>> contours;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(shadow, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);
	for (const auto& contour : contours)
		cv::fillConvexPoly(shadow, contour, cv::Scalar(255, 255, 255));

	const auto kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));
	cv::morphologyEx(shadow, shadow, cv::MORPH_CLOSE, kernel, cv::Point(-1, -1), 1);

	cv::Mat unshadow;
	cv::bitwise_not(shadow, unshadow);

	cv::cvtColor(shadow, shadow, cv::COLOR_GRAY2BGR);
	cv::cvtColor(unshadow, unshadow, cv::COLOR_GRAY2BGR);

	return unshadow;
}

void Func::Img::warp_img_by_hmg(const cv::Mat& src, cv::Mat& dst, cv::Mat& hmg_layer,
	const std::vector<cv::Point2f>& src_pts, const std::vector<cv::Point2f>& dst_pts)
{
	const auto src_rect = cv::boundingRect(src_pts);
	const auto dst_rect = cv::boundingRect(dst_pts);

	std::vector<cv::Point2f> local_src_pts;
	std::vector<cv::Point2f> local_dst_pts;
	std::vector<cv::Point> local_dst_pts_i;

	for (const auto& pts : src_pts)
		local_src_pts.push_back(pts - static_cast<cv::Point2f>(src_rect.tl()));
	for (const auto& pts : dst_pts)
	{
		local_dst_pts.push_back(pts - static_cast<cv::Point2f>(dst_rect.tl()));
		local_dst_pts_i.push_back(static_cast<cv::Point>(pts) - dst_rect.tl());
	}

	auto src_crop = src(src_rect);
	auto dst_crop = dst(dst_rect);
	auto hmg_layer_crop = hmg_layer(dst_rect);

	cv::Mat hmg_crop_result;
	const auto hmg_mat = cv::getPerspectiveTransform(local_src_pts, local_dst_pts);
	cv::warpPerspective(src_crop, hmg_crop_result, hmg_mat, dst_rect.size());

	cv::Mat mask = cv::Mat::zeros(dst_crop.size(), dst_crop.type());
	cv::fillConvexPoly(mask, local_dst_pts_i, cv::Scalar(255, 255, 255), cv::LineTypes::LINE_AA);

	cv::Mat rev_mask, org_layer;
	cv::bitwise_not(mask, rev_mask);
	cv::bitwise_and(hmg_crop_result, mask, hmg_crop_result);
	cv::bitwise_and(dst_crop, rev_mask, org_layer);
	hmg_crop_result += org_layer;

	hmg_crop_result.copyTo(dst_crop);
	hmg_crop_result.copyTo(hmg_layer_crop);
}