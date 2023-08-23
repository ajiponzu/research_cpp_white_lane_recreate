#pragma once

namespace Eval
{
	void Run(const std::string& video_code, const std::string& ortho_code, const int& road_num, const float& meter_per_pix);
};

namespace PreMethodEval
{
	void Run(const std::string& video_code, const std::string& ortho_code, const int& road_num, const float& meter_per_pix);
};