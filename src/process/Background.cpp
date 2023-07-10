#include "../Process.h"

void Background::Create(const std::string& video_code,
	const size_t& spend_time, const int& history, const double& thr)
{
	const auto i_v_path = std::format("resources/{}/input.mp4", video_code);
	const auto o_img_path = std::format("io_images/{}/background.png", video_code);

	auto video_cap = cv::VideoCapture(i_v_path);
	if (!video_cap.isOpened())
	{
		std::cerr << std::format("{} can't be opened.", i_v_path);
		std::abort();
	}

	auto mat_size = cv::Size(
		(int)(video_cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH)),
		(int)(video_cap.get(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT))
	);

	int frame_count = 0;
	cv::Mat cur_frame, frame_float;
	auto p_bg_subtr
		= cv::createBackgroundSubtractorMOG2(history, thr);

	while (frame_count < spend_time)
	{
		video_cap.read(cur_frame);
		if (cur_frame.empty())
			break;

		cv::Mat sub;
		cur_frame.convertTo(frame_float, CV_32F);
		p_bg_subtr->apply(frame_float, sub);

		frame_count++;
		std::cout << frame_count << std::endl;
	}

	cv::Mat bg;
	p_bg_subtr->getBackgroundImage(bg);
	cv::imwrite(o_img_path, bg);

	std::cout << "background has been created..." << std::endl;
	video_cap.release();
}