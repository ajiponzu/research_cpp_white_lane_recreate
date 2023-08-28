#include "Process.h"
#include "Functions.h"
#define TEST

#ifdef TEST

#include "eval/Eval.h"
void test()
{
	//const std::string video_code = "hiru";
	const std::string video_code = "yugata";

	const std::string ortho_code = "ortho";
	const float meter_per_pix = 0.2f;

	Eval::Run(video_code, ortho_code, 4, meter_per_pix);
	PreMethodEval::Run(video_code, ortho_code, 4, meter_per_pix);
}

#else

void app()
{
	const std::string video_code = "hiru";
	//const std::string video_code = "yugata";

	const std::string ortho_code = "ortho";
	const int road_num = 4;

	//Background::Create(video_code, 2000, 500, 16.0);

	//LaneDetection::Video::ExtractCandidate(video_code);
	//LaneDetection::Video::ChooseAllLanes(video_code, road_num);

	//LaneDetection::Ortho::ExtractCandidate(ortho_code);
	//LaneDetection::Ortho::ChooseAllLanes(ortho_code, road_num);

	Registration::Registrator registrator(road_num);
	registrator.DivideRoadsByLane(video_code);
	registrator.DivideRoadsByLane(ortho_code);
	registrator.DrawRoadsByDividedArea(video_code, ortho_code);
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