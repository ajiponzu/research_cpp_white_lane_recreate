#include "Process.h"
#include "Functions.h"
#include "ResourceProvider.h"

//#define TEST

#ifdef TEST

#include "eval/Eval.h"
void test()
{
	const std::string video_code = "hiru";
	//const std::string video_code = "yugata";

	const std::string ortho_code = "ortho";
	const float meter_per_pix = 0.2f;
	const int road_num = 4;

	ResourceProvider::Init(road_num, video_code, ortho_code);

	Eval::Run(video_code, ortho_code, road_num, meter_per_pix);
	PreMethodEval::Run(video_code, ortho_code, road_num, meter_per_pix);
}

#else

void app()
{
	const std::string video_code = "hiru";
	//const std::string video_code = "yugata";

	const std::string ortho_code = "ortho";
	const int road_num = 4;

	ResourceProvider::Init(road_num, video_code, ortho_code);

	//Background::Create(video_code, 2000, 500, 16.0);

	//LaneDetection::Video::ExtractCandidate(video_code);
	//LaneDetection::Video::ChooseAllLanes(video_code, road_num);

	//LaneDetection::Ortho::ExtractCandidate(ortho_code);
	//LaneDetection::Ortho::ChooseAllLanes(ortho_code, road_num);

	Registration::Registrator registrator(road_num);
	registrator.Run(video_code, ortho_code);
}

#endif

int main()
{
#ifdef TEST

	test();

#else

	app();

#endif

	return 0;
}