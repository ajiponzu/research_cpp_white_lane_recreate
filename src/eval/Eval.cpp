#include "Eval.h"
#include "../Functions.h"

static cv::Point g_clicked_video_point;
static cv::Point g_clicked_ortho_point;

static cv::Point2f g_clicked_video_point_org;
static cv::Point2f g_clicked_ortho_point_org;

static std::vector<cv::Point> g_selected_video_points;
static std::vector<cv::Point> g_correct_ortho_points;

static std::vector<cv::Point2f> g_selected_video_org_points;
static std::vector<cv::Point2f> g_correct_ortho_org_points;

//static const double g_RATIO = 2.5;
//static const double g_RATIO_REV = 0.4;
//static constexpr auto g_UI_CIRCLE_RADIUS = 2;
//static  constexpr auto g_VIDEO_CIRCLE_BLACK_RADIUS = 9;
//static  constexpr auto g_VIDEO_CIRCLE_RADIUS = 7;
//static  constexpr auto g_ORTHO_CIRCLE_BLACK_RADIUS = 12;
//static  constexpr auto g_ORTHO_CIRCLE_RADIUS = 10;

static const double g_RATIO = 0.5;
static const double g_RATIO_REV = 2.0;
static  constexpr auto g_VIDEO_CIRCLE_BLACK_RADIUS = 3;
static  constexpr auto g_VIDEO_CIRCLE_RADIUS = 1;
static  constexpr auto g_ORTHO_CIRCLE_BLACK_RADIUS = 5;
static  constexpr auto g_ORTHO_CIRCLE_RADIUS = 3;

static void recv_mouse_msg(int event, int x, int y, int flag, void* callBack) {}

static void recv_mouse_video_msg(int event, int x, int y, int flag, void* callBack)
{
	switch (event)
	{
	case cv::EVENT_LBUTTONDOWN: // マウス左クリック
		g_clicked_video_point = cv::Point(x, y);
		g_clicked_video_point_org = static_cast<cv::Point2f>(g_clicked_video_point) * (float)g_RATIO;
		std::cout << "clicked_point on satta: " << g_clicked_video_point_org << std::endl;
		g_selected_video_points.push_back(g_clicked_video_point);
		g_selected_video_org_points.push_back(g_clicked_video_point_org);
		break;
	case cv::EVENT_RBUTTONDOWN: // マウス右クリック
		if (g_selected_video_points.empty())
			break;
		g_selected_video_points.pop_back();
		g_selected_video_org_points.pop_back();
		break;
	default:
		break;
	}
}

static void recv_mouse_ortho_msg(int event, int x, int y, int flag, void* callBack)
{
	switch (event)
	{
	case cv::EVENT_LBUTTONDOWN: // マウス左クリック
		g_clicked_ortho_point = cv::Point(x, y);
		g_clicked_ortho_point_org = static_cast<cv::Point2f>(g_clicked_ortho_point) * (float)g_RATIO;
		std::cout << "clicked_point on ortho: " << g_clicked_ortho_point_org << std::endl;
		g_correct_ortho_points.push_back(g_clicked_ortho_point);
		g_correct_ortho_org_points.push_back(g_clicked_ortho_point_org);
		break;
	case cv::EVENT_RBUTTONDOWN: // マウス右クリック
		if (g_correct_ortho_points.empty())
			break;
		g_correct_ortho_points.pop_back();
		g_correct_ortho_org_points.pop_back();
		break;
	default:
		break;
	}
}

static void set_window(const std::string& wnd_name, decltype(recv_mouse_msg) mouse_callback)
{
	if (wnd_name == "ortho")
		cv::namedWindow(wnd_name, cv::WindowFlags::WINDOW_KEEPRATIO);
	else
		cv::namedWindow(wnd_name, cv::WindowFlags::WINDOW_GUI_EXPANDED);
	cv::setMouseCallback(wnd_name, mouse_callback);
}

static void push_correct_points_data0()
{
	g_correct_ortho_org_points.push_back(cv::Point2f(784, 1582));
	g_correct_ortho_org_points.push_back(cv::Point2f(792, 1585));
	g_correct_ortho_org_points.push_back(cv::Point2f(799, 1588));
	g_correct_ortho_org_points.push_back(cv::Point2f(799, 1547));
	g_correct_ortho_org_points.push_back(cv::Point2f(807, 1551));
	g_correct_ortho_org_points.push_back(cv::Point2f(815, 1554));
	g_correct_ortho_org_points.push_back(cv::Point2f(815, 1513));
	g_correct_ortho_org_points.push_back(cv::Point2f(823, 1517));
	g_correct_ortho_org_points.push_back(cv::Point2f(830, 1520));
	g_correct_ortho_org_points.push_back(cv::Point2f(831, 1479));
	g_correct_ortho_org_points.push_back(cv::Point2f(838, 1483));
	g_correct_ortho_org_points.push_back(cv::Point2f(846, 1486));
	g_correct_ortho_org_points.push_back(cv::Point2f(847, 1445));
	g_correct_ortho_org_points.push_back(cv::Point2f(855, 1449));
	g_correct_ortho_org_points.push_back(cv::Point2f(862, 1452));
	g_correct_ortho_org_points.push_back(cv::Point2f(864, 1411));
	g_correct_ortho_org_points.push_back(cv::Point2f(872, 1415));
	g_correct_ortho_org_points.push_back(cv::Point2f(878, 1419));
	g_correct_ortho_org_points.push_back(cv::Point2f(881, 1378));
	g_correct_ortho_org_points.push_back(cv::Point2f(888, 1382));
	g_correct_ortho_org_points.push_back(cv::Point2f(895, 1385));
	g_correct_ortho_org_points.push_back(cv::Point2f(897, 1345));
	g_correct_ortho_org_points.push_back(cv::Point2f(905, 1349));
	g_correct_ortho_org_points.push_back(cv::Point2f(912, 1353));
	g_correct_ortho_org_points.push_back(cv::Point2f(915, 1312));
	g_correct_ortho_org_points.push_back(cv::Point2f(922, 1316));
	g_correct_ortho_org_points.push_back(cv::Point2f(929, 1320));
	g_correct_ortho_org_points.push_back(cv::Point2f(932, 1279));
	g_correct_ortho_org_points.push_back(cv::Point2f(940, 1283));
	g_correct_ortho_org_points.push_back(cv::Point2f(947, 1287));
	g_correct_ortho_org_points.push_back(cv::Point2f(950, 1246));
	g_correct_ortho_org_points.push_back(cv::Point2f(958, 1250));
	g_correct_ortho_org_points.push_back(cv::Point2f(964, 1254));
	g_correct_ortho_org_points.push_back(cv::Point2f(968, 1213));
	g_correct_ortho_org_points.push_back(cv::Point2f(976, 1217));
	g_correct_ortho_org_points.push_back(cv::Point2f(983, 1221));
	g_correct_ortho_org_points.push_back(cv::Point2f(986, 1180));
	g_correct_ortho_org_points.push_back(cv::Point2f(994, 1184));
	g_correct_ortho_org_points.push_back(cv::Point2f(1000, 1188));
	g_correct_ortho_org_points.push_back(cv::Point2f(1004, 1147));
	g_correct_ortho_org_points.push_back(cv::Point2f(1012, 1151));
	g_correct_ortho_org_points.push_back(cv::Point2f(1019, 1155));
	g_correct_ortho_org_points.push_back(cv::Point2f(1022, 1115));
	g_correct_ortho_org_points.push_back(cv::Point2f(1029, 1119));
	g_correct_ortho_org_points.push_back(cv::Point2f(1036, 1123));
	g_correct_ortho_org_points.push_back(cv::Point2f(1040, 1082));
	g_correct_ortho_org_points.push_back(cv::Point2f(1048, 1086));
	g_correct_ortho_org_points.push_back(cv::Point2f(1054, 1090));
	g_correct_ortho_org_points.push_back(cv::Point2f(1058, 1049));
	g_correct_ortho_org_points.push_back(cv::Point2f(1066, 1053));
	g_correct_ortho_org_points.push_back(cv::Point2f(1073, 1057));
	g_correct_ortho_org_points.push_back(cv::Point2f(1076, 1016));
	g_correct_ortho_org_points.push_back(cv::Point2f(1084, 1021));
	g_correct_ortho_org_points.push_back(cv::Point2f(1091, 1025));
	g_correct_ortho_org_points.push_back(cv::Point2f(1094, 984));
	g_correct_ortho_org_points.push_back(cv::Point2f(1102, 988));
	g_correct_ortho_org_points.push_back(cv::Point2f(1109, 992));
	g_correct_ortho_org_points.push_back(cv::Point2f(1112, 951));
	g_correct_ortho_org_points.push_back(cv::Point2f(1120, 955));
	g_correct_ortho_org_points.push_back(cv::Point2f(1127, 959));
	g_correct_ortho_org_points.push_back(cv::Point2f(1130, 918));
	g_correct_ortho_org_points.push_back(cv::Point2f(1138, 923));
	g_correct_ortho_org_points.push_back(cv::Point2f(1145, 927));
	g_correct_ortho_org_points.push_back(cv::Point2f(1149, 886));
	g_correct_ortho_org_points.push_back(cv::Point2f(1156, 890));
	g_correct_ortho_org_points.push_back(cv::Point2f(1163, 894));
	g_correct_ortho_org_points.push_back(cv::Point2f(1167, 853));
	g_correct_ortho_org_points.push_back(cv::Point2f(1175, 857));
	g_correct_ortho_org_points.push_back(cv::Point2f(1182, 861));
	g_correct_ortho_org_points.push_back(cv::Point2f(1185, 821));
	g_correct_ortho_org_points.push_back(cv::Point2f(1193, 825));
	g_correct_ortho_org_points.push_back(cv::Point2f(1200, 829));
	g_correct_ortho_org_points.push_back(cv::Point2f(1204, 788));
	g_correct_ortho_org_points.push_back(cv::Point2f(1211, 792));
	g_correct_ortho_org_points.push_back(cv::Point2f(1218, 795));
	g_correct_ortho_org_points.push_back(cv::Point2f(1221, 755));
	g_correct_ortho_org_points.push_back(cv::Point2f(1229, 759));
	g_correct_ortho_org_points.push_back(cv::Point2f(1236, 763));
	g_correct_ortho_org_points.push_back(cv::Point2f(1240, 722));
	g_correct_ortho_org_points.push_back(cv::Point2f(1247, 727));
	g_correct_ortho_org_points.push_back(cv::Point2f(1254, 731));
	g_correct_ortho_org_points.push_back(cv::Point2f(1258, 690));
	g_correct_ortho_org_points.push_back(cv::Point2f(1265, 694));
	g_correct_ortho_org_points.push_back(cv::Point2f(1272, 698));
	g_correct_ortho_org_points.push_back(cv::Point2f(1276, 657));
	g_correct_ortho_org_points.push_back(cv::Point2f(1283, 661));
	g_correct_ortho_org_points.push_back(cv::Point2f(1291, 665));
	g_correct_ortho_org_points.push_back(cv::Point2f(1294, 624));
	g_correct_ortho_org_points.push_back(cv::Point2f(1302, 628));
	g_correct_ortho_org_points.push_back(cv::Point2f(1308, 632));
	g_correct_ortho_org_points.push_back(cv::Point2f(1312, 591));
	g_correct_ortho_org_points.push_back(cv::Point2f(1320, 595));
	g_correct_ortho_org_points.push_back(cv::Point2f(1326, 599));
	g_correct_ortho_org_points.push_back(cv::Point2f(1330, 558));
	g_correct_ortho_org_points.push_back(cv::Point2f(1338, 562));
	g_correct_ortho_org_points.push_back(cv::Point2f(1345, 566));
	g_correct_ortho_org_points.push_back(cv::Point2f(1348, 525));
	g_correct_ortho_org_points.push_back(cv::Point2f(1356, 529));
	g_correct_ortho_org_points.push_back(cv::Point2f(1363, 533));
	g_correct_ortho_org_points.push_back(cv::Point2f(1366, 492));
	g_correct_ortho_org_points.push_back(cv::Point2f(1374, 497));
	g_correct_ortho_org_points.push_back(cv::Point2f(1381, 501));
	g_correct_ortho_org_points.push_back(cv::Point2f(1384, 460));
	g_correct_ortho_org_points.push_back(cv::Point2f(1392, 464));
	g_correct_ortho_org_points.push_back(cv::Point2f(1399, 468));
	g_correct_ortho_org_points.push_back(cv::Point2f(1403, 427));
	g_correct_ortho_org_points.push_back(cv::Point2f(1410, 432));
	g_correct_ortho_org_points.push_back(cv::Point2f(1417, 436));
	g_correct_ortho_org_points.push_back(cv::Point2f(1420, 395));
	g_correct_ortho_org_points.push_back(cv::Point2f(1428, 399));
	g_correct_ortho_org_points.push_back(cv::Point2f(1435, 403));
	g_correct_ortho_org_points.push_back(cv::Point2f(1439, 362));
	g_correct_ortho_org_points.push_back(cv::Point2f(1446, 366));
	g_correct_ortho_org_points.push_back(cv::Point2f(1453, 370));
	g_correct_ortho_org_points.push_back(cv::Point2f(1457, 329));
	g_correct_ortho_org_points.push_back(cv::Point2f(1465, 333));
	g_correct_ortho_org_points.push_back(cv::Point2f(1471, 337));
	g_correct_ortho_org_points.push_back(cv::Point2f(1475, 297));
	g_correct_ortho_org_points.push_back(cv::Point2f(1483, 301));
	g_correct_ortho_org_points.push_back(cv::Point2f(1490, 305));
	g_correct_ortho_org_points.push_back(cv::Point2f(1493, 264));
	g_correct_ortho_org_points.push_back(cv::Point2f(1500, 268));
	g_correct_ortho_org_points.push_back(cv::Point2f(1507, 272));
	g_correct_ortho_org_points.push_back(cv::Point2f(1510, 231));
	g_correct_ortho_org_points.push_back(cv::Point2f(1518, 235));
	g_correct_ortho_org_points.push_back(cv::Point2f(1525, 239));
	g_correct_ortho_org_points.push_back(cv::Point2f(1529, 198));
	g_correct_ortho_org_points.push_back(cv::Point2f(1536, 203));
	g_correct_ortho_org_points.push_back(cv::Point2f(1544, 207));
	g_correct_ortho_org_points.push_back(cv::Point2f(1548, 165));
	g_correct_ortho_org_points.push_back(cv::Point2f(1554, 170));
	g_correct_ortho_org_points.push_back(cv::Point2f(1562, 174));
	g_correct_ortho_org_points.push_back(cv::Point2f(1565, 133));
	g_correct_ortho_org_points.push_back(cv::Point2f(1573, 138));
	g_correct_ortho_org_points.push_back(cv::Point2f(1580, 142));
	g_correct_ortho_org_points.push_back(cv::Point2f(1584, 101));
	g_correct_ortho_org_points.push_back(cv::Point2f(1591, 105));
	g_correct_ortho_org_points.push_back(cv::Point2f(1598, 109));
	g_correct_ortho_org_points.push_back(cv::Point2f(1602, 68));
	g_correct_ortho_org_points.push_back(cv::Point2f(1610, 72));
	g_correct_ortho_org_points.push_back(cv::Point2f(1616, 76));

	for (const auto& point : g_correct_ortho_org_points)
		g_correct_ortho_points.push_back(point * g_RATIO_REV);
}

static void push_correct_points_data1()
{
	g_correct_ortho_org_points.push_back(cv::Point2f(821, 1600));
	g_correct_ortho_org_points.push_back(cv::Point2f(829, 1603));
	g_correct_ortho_org_points.push_back(cv::Point2f(838, 1606));
	g_correct_ortho_org_points.push_back(cv::Point2f(837, 1565));
	g_correct_ortho_org_points.push_back(cv::Point2f(845, 1568));
	g_correct_ortho_org_points.push_back(cv::Point2f(852, 1572));
	g_correct_ortho_org_points.push_back(cv::Point2f(853, 1531));
	g_correct_ortho_org_points.push_back(cv::Point2f(860, 1534));
	g_correct_ortho_org_points.push_back(cv::Point2f(868, 1538));
	g_correct_ortho_org_points.push_back(cv::Point2f(869, 1497));
	g_correct_ortho_org_points.push_back(cv::Point2f(876, 1501));
	g_correct_ortho_org_points.push_back(cv::Point2f(883, 1504));
	g_correct_ortho_org_points.push_back(cv::Point2f(884, 1464));
	g_correct_ortho_org_points.push_back(cv::Point2f(892, 1467));
	g_correct_ortho_org_points.push_back(cv::Point2f(900, 1471));
	g_correct_ortho_org_points.push_back(cv::Point2f(902, 1430));
	g_correct_ortho_org_points.push_back(cv::Point2f(909, 1433));
	g_correct_ortho_org_points.push_back(cv::Point2f(917, 1437));
	g_correct_ortho_org_points.push_back(cv::Point2f(918, 1396));
	g_correct_ortho_org_points.push_back(cv::Point2f(926, 1399));
	g_correct_ortho_org_points.push_back(cv::Point2f(933, 1403));
	g_correct_ortho_org_points.push_back(cv::Point2f(936, 1362));
	g_correct_ortho_org_points.push_back(cv::Point2f(943, 1366));
	g_correct_ortho_org_points.push_back(cv::Point2f(950, 1370));
	g_correct_ortho_org_points.push_back(cv::Point2f(952, 1329));
	g_correct_ortho_org_points.push_back(cv::Point2f(960, 1333));
	g_correct_ortho_org_points.push_back(cv::Point2f(968, 1337));
	g_correct_ortho_org_points.push_back(cv::Point2f(970, 1296));
	g_correct_ortho_org_points.push_back(cv::Point2f(978, 1300));
	g_correct_ortho_org_points.push_back(cv::Point2f(985, 1305));
	g_correct_ortho_org_points.push_back(cv::Point2f(988, 1263));
	g_correct_ortho_org_points.push_back(cv::Point2f(996, 1267));
	g_correct_ortho_org_points.push_back(cv::Point2f(1003, 1271));
	g_correct_ortho_org_points.push_back(cv::Point2f(1006, 1230));
	g_correct_ortho_org_points.push_back(cv::Point2f(1014, 1234));
	g_correct_ortho_org_points.push_back(cv::Point2f(1021, 1238));
	g_correct_ortho_org_points.push_back(cv::Point2f(1023, 1198));
	g_correct_ortho_org_points.push_back(cv::Point2f(1031, 1202));
	g_correct_ortho_org_points.push_back(cv::Point2f(1039, 1206));
	g_correct_ortho_org_points.push_back(cv::Point2f(1042, 1165));
	g_correct_ortho_org_points.push_back(cv::Point2f(1049, 1169));
	g_correct_ortho_org_points.push_back(cv::Point2f(1056, 1173));
	g_correct_ortho_org_points.push_back(cv::Point2f(1060, 1132));
	g_correct_ortho_org_points.push_back(cv::Point2f(1067, 1136));
	g_correct_ortho_org_points.push_back(cv::Point2f(1075, 1140));
	g_correct_ortho_org_points.push_back(cv::Point2f(1078, 1100));
	g_correct_ortho_org_points.push_back(cv::Point2f(1086, 1103));
	g_correct_ortho_org_points.push_back(cv::Point2f(1093, 1107));
	g_correct_ortho_org_points.push_back(cv::Point2f(1097, 1066));
	g_correct_ortho_org_points.push_back(cv::Point2f(1104, 1071));
	g_correct_ortho_org_points.push_back(cv::Point2f(1111, 1075));
	g_correct_ortho_org_points.push_back(cv::Point2f(1115, 1034));
	g_correct_ortho_org_points.push_back(cv::Point2f(1122, 1038));
	g_correct_ortho_org_points.push_back(cv::Point2f(1130, 1042));
	g_correct_ortho_org_points.push_back(cv::Point2f(1133, 1001));
	g_correct_ortho_org_points.push_back(cv::Point2f(1140, 1005));
	g_correct_ortho_org_points.push_back(cv::Point2f(1148, 1009));
	g_correct_ortho_org_points.push_back(cv::Point2f(1151, 969));
	g_correct_ortho_org_points.push_back(cv::Point2f(1159, 972));
	g_correct_ortho_org_points.push_back(cv::Point2f(1165, 977));
	g_correct_ortho_org_points.push_back(cv::Point2f(1168, 936));
	g_correct_ortho_org_points.push_back(cv::Point2f(1176, 940));
	g_correct_ortho_org_points.push_back(cv::Point2f(1184, 943));
	g_correct_ortho_org_points.push_back(cv::Point2f(1186, 903));
	g_correct_ortho_org_points.push_back(cv::Point2f(1194, 907));
	g_correct_ortho_org_points.push_back(cv::Point2f(1201, 911));
	g_correct_ortho_org_points.push_back(cv::Point2f(1205, 870));
	g_correct_ortho_org_points.push_back(cv::Point2f(1213, 874));
	g_correct_ortho_org_points.push_back(cv::Point2f(1220, 878));
	g_correct_ortho_org_points.push_back(cv::Point2f(1224, 837));
	g_correct_ortho_org_points.push_back(cv::Point2f(1231, 841));
	g_correct_ortho_org_points.push_back(cv::Point2f(1238, 845));
	g_correct_ortho_org_points.push_back(cv::Point2f(1241, 804));
	g_correct_ortho_org_points.push_back(cv::Point2f(1249, 809));
	g_correct_ortho_org_points.push_back(cv::Point2f(1256, 812));
	g_correct_ortho_org_points.push_back(cv::Point2f(1260, 772));
	g_correct_ortho_org_points.push_back(cv::Point2f(1267, 775));
	g_correct_ortho_org_points.push_back(cv::Point2f(1275, 780));
	g_correct_ortho_org_points.push_back(cv::Point2f(1277, 739));
	g_correct_ortho_org_points.push_back(cv::Point2f(1285, 743));
	g_correct_ortho_org_points.push_back(cv::Point2f(1293, 748));
	g_correct_ortho_org_points.push_back(cv::Point2f(1296, 707));
	g_correct_ortho_org_points.push_back(cv::Point2f(1303, 711));
	g_correct_ortho_org_points.push_back(cv::Point2f(1311, 715));
	g_correct_ortho_org_points.push_back(cv::Point2f(1315, 673));
	g_correct_ortho_org_points.push_back(cv::Point2f(1322, 677));
	g_correct_ortho_org_points.push_back(cv::Point2f(1329, 681));
	g_correct_ortho_org_points.push_back(cv::Point2f(1332, 640));
	g_correct_ortho_org_points.push_back(cv::Point2f(1340, 644));
	g_correct_ortho_org_points.push_back(cv::Point2f(1347, 648));
	g_correct_ortho_org_points.push_back(cv::Point2f(1350, 608));
	g_correct_ortho_org_points.push_back(cv::Point2f(1358, 611));
	g_correct_ortho_org_points.push_back(cv::Point2f(1365, 615));
	g_correct_ortho_org_points.push_back(cv::Point2f(1369, 575));
	g_correct_ortho_org_points.push_back(cv::Point2f(1376, 579));
	g_correct_ortho_org_points.push_back(cv::Point2f(1384, 583));
	g_correct_ortho_org_points.push_back(cv::Point2f(1387, 542));
	g_correct_ortho_org_points.push_back(cv::Point2f(1394, 547));
	g_correct_ortho_org_points.push_back(cv::Point2f(1402, 551));

	for (const auto& point : g_correct_ortho_org_points)
		g_correct_ortho_points.push_back(point * g_RATIO_REV);
}

static void push_correct_points_data2()
{
	g_correct_ortho_org_points.push_back(cv::Point2f(791, 1939));
	g_correct_ortho_org_points.push_back(cv::Point2f(798, 1944));
	g_correct_ortho_org_points.push_back(cv::Point2f(806, 1949));
	g_correct_ortho_org_points.push_back(cv::Point2f(822, 1898));
	g_correct_ortho_org_points.push_back(cv::Point2f(829, 1904));
	g_correct_ortho_org_points.push_back(cv::Point2f(836, 1910));
	g_correct_ortho_org_points.push_back(cv::Point2f(852, 1859));
	g_correct_ortho_org_points.push_back(cv::Point2f(859, 1865));
	g_correct_ortho_org_points.push_back(cv::Point2f(866, 1870));
	g_correct_ortho_org_points.push_back(cv::Point2f(875, 1829));
	g_correct_ortho_org_points.push_back(cv::Point2f(882, 1834));
	g_correct_ortho_org_points.push_back(cv::Point2f(889, 1839));
	g_correct_ortho_org_points.push_back(cv::Point2f(899, 1798));
	g_correct_ortho_org_points.push_back(cv::Point2f(906, 1803));
	g_correct_ortho_org_points.push_back(cv::Point2f(913, 1807));
	g_correct_ortho_org_points.push_back(cv::Point2f(929, 1757));
	g_correct_ortho_org_points.push_back(cv::Point2f(936, 1762));
	g_correct_ortho_org_points.push_back(cv::Point2f(943, 1767));
	g_correct_ortho_org_points.push_back(cv::Point2f(958, 1717));
	g_correct_ortho_org_points.push_back(cv::Point2f(965, 1721));
	g_correct_ortho_org_points.push_back(cv::Point2f(973, 1726));
	g_correct_ortho_org_points.push_back(cv::Point2f(989, 1675));
	g_correct_ortho_org_points.push_back(cv::Point2f(995, 1681));
	g_correct_ortho_org_points.push_back(cv::Point2f(1002, 1686));
	g_correct_ortho_org_points.push_back(cv::Point2f(1016, 1637));
	g_correct_ortho_org_points.push_back(cv::Point2f(1024, 1643));
	g_correct_ortho_org_points.push_back(cv::Point2f(1031, 1648));
	g_correct_ortho_org_points.push_back(cv::Point2f(1039, 1607));
	g_correct_ortho_org_points.push_back(cv::Point2f(1046, 1612));
	g_correct_ortho_org_points.push_back(cv::Point2f(1053, 1617));
	g_correct_ortho_org_points.push_back(cv::Point2f(1060, 1577));
	g_correct_ortho_org_points.push_back(cv::Point2f(1068, 1582));
	g_correct_ortho_org_points.push_back(cv::Point2f(1075, 1587));
	g_correct_ortho_org_points.push_back(cv::Point2f(1085, 1542));
	g_correct_ortho_org_points.push_back(cv::Point2f(1092, 1547));
	g_correct_ortho_org_points.push_back(cv::Point2f(1100, 1552));
	g_correct_ortho_org_points.push_back(cv::Point2f(1112, 1505));
	g_correct_ortho_org_points.push_back(cv::Point2f(1118, 1510));
	g_correct_ortho_org_points.push_back(cv::Point2f(1126, 1515));
	g_correct_ortho_org_points.push_back(cv::Point2f(1141, 1464));
	g_correct_ortho_org_points.push_back(cv::Point2f(1148, 1469));
	g_correct_ortho_org_points.push_back(cv::Point2f(1155, 1474));
	g_correct_ortho_org_points.push_back(cv::Point2f(1170, 1421));
	g_correct_ortho_org_points.push_back(cv::Point2f(1177, 1426));
	g_correct_ortho_org_points.push_back(cv::Point2f(1185, 1431));
	g_correct_ortho_org_points.push_back(cv::Point2f(1198, 1380));
	g_correct_ortho_org_points.push_back(cv::Point2f(1206, 1386));
	g_correct_ortho_org_points.push_back(cv::Point2f(1213, 1391));
	g_correct_ortho_org_points.push_back(cv::Point2f(1226, 1341));
	g_correct_ortho_org_points.push_back(cv::Point2f(1233, 1345));
	g_correct_ortho_org_points.push_back(cv::Point2f(1241, 1351));
	g_correct_ortho_org_points.push_back(cv::Point2f(1255, 1299));
	g_correct_ortho_org_points.push_back(cv::Point2f(1262, 1304));
	g_correct_ortho_org_points.push_back(cv::Point2f(1270, 1309));
	g_correct_ortho_org_points.push_back(cv::Point2f(1283, 1257));
	g_correct_ortho_org_points.push_back(cv::Point2f(1290, 1262));
	g_correct_ortho_org_points.push_back(cv::Point2f(1298, 1268));
	g_correct_ortho_org_points.push_back(cv::Point2f(1310, 1215));
	g_correct_ortho_org_points.push_back(cv::Point2f(1318, 1220));
	g_correct_ortho_org_points.push_back(cv::Point2f(1326, 1226));
	g_correct_ortho_org_points.push_back(cv::Point2f(1338, 1174));
	g_correct_ortho_org_points.push_back(cv::Point2f(1345, 1178));
	g_correct_ortho_org_points.push_back(cv::Point2f(1353, 1184));
	g_correct_ortho_org_points.push_back(cv::Point2f(1366, 1131));
	g_correct_ortho_org_points.push_back(cv::Point2f(1373, 1136));
	g_correct_ortho_org_points.push_back(cv::Point2f(1381, 1141));
	g_correct_ortho_org_points.push_back(cv::Point2f(1393, 1090));
	g_correct_ortho_org_points.push_back(cv::Point2f(1401, 1095));
	g_correct_ortho_org_points.push_back(cv::Point2f(1408, 1100));
	g_correct_ortho_org_points.push_back(cv::Point2f(1421, 1048));
	g_correct_ortho_org_points.push_back(cv::Point2f(1428, 1053));
	g_correct_ortho_org_points.push_back(cv::Point2f(1435, 1058));
	g_correct_ortho_org_points.push_back(cv::Point2f(1447, 1007));
	g_correct_ortho_org_points.push_back(cv::Point2f(1455, 1011));
	g_correct_ortho_org_points.push_back(cv::Point2f(1463, 1016));
	g_correct_ortho_org_points.push_back(cv::Point2f(1473, 964));
	g_correct_ortho_org_points.push_back(cv::Point2f(1481, 969));
	g_correct_ortho_org_points.push_back(cv::Point2f(1489, 974));
	g_correct_ortho_org_points.push_back(cv::Point2f(1500, 922));
	g_correct_ortho_org_points.push_back(cv::Point2f(1507, 926));
	g_correct_ortho_org_points.push_back(cv::Point2f(1516, 931));
	g_correct_ortho_org_points.push_back(cv::Point2f(1526, 879));
	g_correct_ortho_org_points.push_back(cv::Point2f(1534, 883));
	g_correct_ortho_org_points.push_back(cv::Point2f(1542, 888));
	g_correct_ortho_org_points.push_back(cv::Point2f(1552, 836));
	g_correct_ortho_org_points.push_back(cv::Point2f(1560, 840));
	g_correct_ortho_org_points.push_back(cv::Point2f(1567, 845));
	g_correct_ortho_org_points.push_back(cv::Point2f(1577, 794));
	g_correct_ortho_org_points.push_back(cv::Point2f(1585, 798));
	g_correct_ortho_org_points.push_back(cv::Point2f(1593, 802));
	g_correct_ortho_org_points.push_back(cv::Point2f(1603, 751));
	g_correct_ortho_org_points.push_back(cv::Point2f(1610, 755));
	g_correct_ortho_org_points.push_back(cv::Point2f(1618, 760));
	g_correct_ortho_org_points.push_back(cv::Point2f(1626, 709));
	g_correct_ortho_org_points.push_back(cv::Point2f(1634, 713));
	g_correct_ortho_org_points.push_back(cv::Point2f(1641, 718));
	g_correct_ortho_org_points.push_back(cv::Point2f(1650, 666));
	g_correct_ortho_org_points.push_back(cv::Point2f(1657, 672));
	g_correct_ortho_org_points.push_back(cv::Point2f(1665, 676));
	g_correct_ortho_org_points.push_back(cv::Point2f(1674, 623));
	g_correct_ortho_org_points.push_back(cv::Point2f(1682, 628));
	g_correct_ortho_org_points.push_back(cv::Point2f(1690, 632));
	g_correct_ortho_org_points.push_back(cv::Point2f(1697, 581));
	g_correct_ortho_org_points.push_back(cv::Point2f(1705, 584));
	g_correct_ortho_org_points.push_back(cv::Point2f(1713, 589));

	for (const auto& point : g_correct_ortho_org_points)
		g_correct_ortho_points.push_back(point * g_RATIO_REV);
}

static void push_correct_points_data3()
{
	g_correct_ortho_org_points.push_back(cv::Point2f(857, 2052));
	g_correct_ortho_org_points.push_back(cv::Point2f(864, 2057));
	g_correct_ortho_org_points.push_back(cv::Point2f(872, 2062));
	g_correct_ortho_org_points.push_back(cv::Point2f(886, 2011));
	g_correct_ortho_org_points.push_back(cv::Point2f(893, 2016));
	g_correct_ortho_org_points.push_back(cv::Point2f(901, 2022));
	g_correct_ortho_org_points.push_back(cv::Point2f(914, 1971));
	g_correct_ortho_org_points.push_back(cv::Point2f(922, 1976));
	g_correct_ortho_org_points.push_back(cv::Point2f(929, 1980));
	g_correct_ortho_org_points.push_back(cv::Point2f(943, 1930));
	g_correct_ortho_org_points.push_back(cv::Point2f(950, 1934));
	g_correct_ortho_org_points.push_back(cv::Point2f(957, 1940));
	g_correct_ortho_org_points.push_back(cv::Point2f(971, 1889));
	g_correct_ortho_org_points.push_back(cv::Point2f(978, 1894));
	g_correct_ortho_org_points.push_back(cv::Point2f(986, 1899));
	g_correct_ortho_org_points.push_back(cv::Point2f(993, 1858));
	g_correct_ortho_org_points.push_back(cv::Point2f(999, 1863));
	g_correct_ortho_org_points.push_back(cv::Point2f(1007, 1868));
	g_correct_ortho_org_points.push_back(cv::Point2f(1013, 1827));
	g_correct_ortho_org_points.push_back(cv::Point2f(1021, 1832));
	g_correct_ortho_org_points.push_back(cv::Point2f(1028, 1836));
	g_correct_ortho_org_points.push_back(cv::Point2f(1042, 1785));
	g_correct_ortho_org_points.push_back(cv::Point2f(1049, 1790));
	g_correct_ortho_org_points.push_back(cv::Point2f(1056, 1794));
	g_correct_ortho_org_points.push_back(cv::Point2f(1070, 1743));
	g_correct_ortho_org_points.push_back(cv::Point2f(1078, 1748));
	g_correct_ortho_org_points.push_back(cv::Point2f(1085, 1753));
	g_correct_ortho_org_points.push_back(cv::Point2f(1098, 1702));
	g_correct_ortho_org_points.push_back(cv::Point2f(1106, 1706));
	g_correct_ortho_org_points.push_back(cv::Point2f(1113, 1711));
	g_correct_ortho_org_points.push_back(cv::Point2f(1126, 1659));
	g_correct_ortho_org_points.push_back(cv::Point2f(1133, 1664));
	g_correct_ortho_org_points.push_back(cv::Point2f(1141, 1669));
	g_correct_ortho_org_points.push_back(cv::Point2f(1153, 1617));
	g_correct_ortho_org_points.push_back(cv::Point2f(1160, 1622));
	g_correct_ortho_org_points.push_back(cv::Point2f(1168, 1627));
	g_correct_ortho_org_points.push_back(cv::Point2f(1180, 1575));
	g_correct_ortho_org_points.push_back(cv::Point2f(1187, 1581));
	g_correct_ortho_org_points.push_back(cv::Point2f(1195, 1585));
	g_correct_ortho_org_points.push_back(cv::Point2f(1207, 1534));
	g_correct_ortho_org_points.push_back(cv::Point2f(1215, 1538));
	g_correct_ortho_org_points.push_back(cv::Point2f(1222, 1543));
	g_correct_ortho_org_points.push_back(cv::Point2f(1235, 1491));
	g_correct_ortho_org_points.push_back(cv::Point2f(1242, 1496));
	g_correct_ortho_org_points.push_back(cv::Point2f(1250, 1501));
	g_correct_ortho_org_points.push_back(cv::Point2f(1261, 1449));
	g_correct_ortho_org_points.push_back(cv::Point2f(1269, 1454));
	g_correct_ortho_org_points.push_back(cv::Point2f(1276, 1459));
	g_correct_ortho_org_points.push_back(cv::Point2f(1288, 1407));
	g_correct_ortho_org_points.push_back(cv::Point2f(1295, 1411));
	g_correct_ortho_org_points.push_back(cv::Point2f(1303, 1416));
	g_correct_ortho_org_points.push_back(cv::Point2f(1315, 1364));
	g_correct_ortho_org_points.push_back(cv::Point2f(1322, 1368));
	g_correct_ortho_org_points.push_back(cv::Point2f(1329, 1373));
	g_correct_ortho_org_points.push_back(cv::Point2f(1341, 1321));
	g_correct_ortho_org_points.push_back(cv::Point2f(1349, 1326));
	g_correct_ortho_org_points.push_back(cv::Point2f(1356, 1331));
	g_correct_ortho_org_points.push_back(cv::Point2f(1366, 1279));
	g_correct_ortho_org_points.push_back(cv::Point2f(1374, 1284));
	g_correct_ortho_org_points.push_back(cv::Point2f(1382, 1288));
	g_correct_ortho_org_points.push_back(cv::Point2f(1393, 1236));
	g_correct_ortho_org_points.push_back(cv::Point2f(1401, 1241));
	g_correct_ortho_org_points.push_back(cv::Point2f(1408, 1246));
	g_correct_ortho_org_points.push_back(cv::Point2f(1419, 1193));
	g_correct_ortho_org_points.push_back(cv::Point2f(1427, 1198));
	g_correct_ortho_org_points.push_back(cv::Point2f(1434, 1202));
	g_correct_ortho_org_points.push_back(cv::Point2f(1445, 1150));
	g_correct_ortho_org_points.push_back(cv::Point2f(1453, 1154));
	g_correct_ortho_org_points.push_back(cv::Point2f(1460, 1158));
	g_correct_ortho_org_points.push_back(cv::Point2f(1471, 1106));
	g_correct_ortho_org_points.push_back(cv::Point2f(1478, 1111));
	g_correct_ortho_org_points.push_back(cv::Point2f(1486, 1115));
	g_correct_ortho_org_points.push_back(cv::Point2f(1495, 1064));
	g_correct_ortho_org_points.push_back(cv::Point2f(1504, 1068));
	g_correct_ortho_org_points.push_back(cv::Point2f(1511, 1072));
	g_correct_ortho_org_points.push_back(cv::Point2f(1521, 1021));
	g_correct_ortho_org_points.push_back(cv::Point2f(1528, 1025));
	g_correct_ortho_org_points.push_back(cv::Point2f(1536, 1029));
	g_correct_ortho_org_points.push_back(cv::Point2f(1546, 977));
	g_correct_ortho_org_points.push_back(cv::Point2f(1554, 981));
	g_correct_ortho_org_points.push_back(cv::Point2f(1561, 985));
	g_correct_ortho_org_points.push_back(cv::Point2f(1570, 934));
	g_correct_ortho_org_points.push_back(cv::Point2f(1579, 938));
	g_correct_ortho_org_points.push_back(cv::Point2f(1586, 942));
	g_correct_ortho_org_points.push_back(cv::Point2f(1595, 890));
	g_correct_ortho_org_points.push_back(cv::Point2f(1603, 895));
	g_correct_ortho_org_points.push_back(cv::Point2f(1611, 899));
	g_correct_ortho_org_points.push_back(cv::Point2f(1620, 847));
	g_correct_ortho_org_points.push_back(cv::Point2f(1628, 851));
	g_correct_ortho_org_points.push_back(cv::Point2f(1636, 855));
	g_correct_ortho_org_points.push_back(cv::Point2f(1645, 803));
	g_correct_ortho_org_points.push_back(cv::Point2f(1653, 807));
	g_correct_ortho_org_points.push_back(cv::Point2f(1660, 812));
	g_correct_ortho_org_points.push_back(cv::Point2f(1669, 760));
	g_correct_ortho_org_points.push_back(cv::Point2f(1676, 764));
	g_correct_ortho_org_points.push_back(cv::Point2f(1685, 768));
	g_correct_ortho_org_points.push_back(cv::Point2f(1693, 716));
	g_correct_ortho_org_points.push_back(cv::Point2f(1701, 720));
	g_correct_ortho_org_points.push_back(cv::Point2f(1708, 724));
	g_correct_ortho_org_points.push_back(cv::Point2f(1717, 672));
	g_correct_ortho_org_points.push_back(cv::Point2f(1725, 676));
	g_correct_ortho_org_points.push_back(cv::Point2f(1732, 680));
	g_correct_ortho_org_points.push_back(cv::Point2f(1740, 628));
	g_correct_ortho_org_points.push_back(cv::Point2f(1748, 632));
	g_correct_ortho_org_points.push_back(cv::Point2f(1756, 637));

	for (const auto& point : g_correct_ortho_org_points)
		g_correct_ortho_points.push_back(point * g_RATIO_REV);
}

static const std::vector<std::function<void()>> g_push_correct_points_data_func_list = {
	push_correct_points_data0,
	push_correct_points_data1,
	push_correct_points_data2,
	push_correct_points_data3,
};

static void input_points_interface(const std::string& video_code, const std::string& ortho_code, const cv::Mat& video_img, const cv::Mat& ortho_img)
{
	std::system("cls");
	set_window(video_code, recv_mouse_video_msg);
	set_window(ortho_code, recv_mouse_ortho_msg);

	while (true)
	{
		std::string input;
		std::cout << "input: continue? 'c'(click) : 'i'(key input) : others" << std::endl;
		std::cin >> input;

		if (input != "c" && input != "i")
			break;

		while (input == "c")
		{
			auto video_view = video_img.clone();
			for (const auto& point : g_selected_video_points)
			{
				cv::circle(video_view, point, g_VIDEO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
				cv::circle(video_view, point, g_VIDEO_CIRCLE_RADIUS, cv::Scalar(0, 255, 0), -1);
			}
			cv::imshow(video_code, video_view);

			auto ortho_view = ortho_img.clone();
			for (const auto& point : g_correct_ortho_points)
			{
				cv::circle(ortho_view, point, g_ORTHO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
				cv::circle(ortho_view, point, g_ORTHO_CIRCLE_RADIUS, cv::Scalar(0, 0, 255), -1);
			}
			cv::imshow(ortho_code, ortho_view);

			if (cv::waitKey(1) == 'q')
				break;
		}

		if (input == "i")
		{
			std::cout << "video -> input_x: " << std::endl;
			std::cin >> g_clicked_video_point_org.x;
			std::cout << "video -> input_y: " << std::endl;
			std::cin >> g_clicked_video_point_org.y;
			g_selected_video_org_points.push_back(g_clicked_video_point_org);
			g_selected_video_points.push_back(static_cast<cv::Point>(g_clicked_video_point_org * g_RATIO_REV));

			std::cout << "ortho -> input_x: " << std::endl;
			std::cin >> g_clicked_ortho_point_org.x;
			std::cout << "ortho -> input_y: " << std::endl;
			std::cin >> g_clicked_ortho_point_org.y;
			g_correct_ortho_org_points.push_back(g_clicked_ortho_point_org);
			g_correct_ortho_points.push_back(static_cast<cv::Point>(g_clicked_ortho_point_org * g_RATIO_REV));
		}
	}
}

static void output_select_points_code(const std::string& video_code, const std::string& ortho_code, const uint64_t& experiment_id)
{
	std::ofstream ofs(std::format("io_images/{}/{}_warp/select_points_code{}.txt", ortho_code, video_code, experiment_id));

	for (const auto& point : g_selected_video_org_points)
		ofs << std::format("g_selected_video_org_points.push_back(cv::Point2f({}, {}));", point.x, point.y) << std::endl;
}

void Eval::Run(const std::string& video_code, const std::string& ortho_code, const float& meter_per_pix)
{
	cv::Mat transed_points_map = Func::GeoCvt::get_float_tif(std::format("io_images/{}/transed_points_map.tif", video_code));
	cv::Mat lanes_inf_map = Func::GeoCvt::get_float_tif(std::format("io_images/{}/lanes_inf_map.tif", video_code));

	auto video_img = cv::imread(std::format("io_images/{}/background.png", video_code));
	auto ortho_img = Func::GeoCvt::get_multicolor_mat(std::format("resources/{}/ortho.tif", ortho_code));

	auto video_org_img = video_img.clone();
	auto ortho_org_img = ortho_img.clone();

	const auto video_mask = cv::imread(std::format("resources/{}/road_mask.png", video_code));
	const auto ortho_mask = cv::imread(std::format("resources/{}/road_mask.png", ortho_code));

	cv::addWeighted(video_org_img, 0.85, video_mask, 0.15, 1.0, video_org_img);
	cv::addWeighted(ortho_org_img, 0.85, ortho_mask, 0.15, 1.0, ortho_org_img);

	cv::resize(video_org_img, video_img, cv::Size(), g_RATIO_REV, g_RATIO_REV);
	cv::resize(ortho_org_img, ortho_img, cv::Size(), g_RATIO_REV, g_RATIO_REV);

	uint64_t experiment_id = 0;
	//while (true)
	while (experiment_id < 4)
	{
		cv::destroyAllWindows();
		g_selected_video_points.clear();
		g_selected_video_org_points.clear();
		g_correct_ortho_points.clear();
		g_correct_ortho_org_points.clear();

		std::string input;
		std::cout << "Eval system: continue? 'y' : others" << std::endl;
		std::cin >> input;
		if (input != "y")
			break;

		g_push_correct_points_data_func_list[experiment_id]();
		input_points_interface(video_code, ortho_code, video_img, ortho_img);
		output_select_points_code(video_code, ortho_code, experiment_id);

		if (g_selected_video_org_points.size() != g_correct_ortho_org_points.size())
		{
			std::cout << "not executed: number of video_points must be number of ortho_points" << std::endl;
			experiment_id++;
			continue;
		}

		std::cout << "calculating....." << std::endl;
		std::cout << "************\n\n" << std::endl;

		auto video_result_img = video_org_img.clone();
		auto ortho_result_img = ortho_org_img.clone();

		const auto result_tsv_path = std::format("io_images/ortho/{}_eval/result{}.tsv", video_code, experiment_id);
		std::ofstream result_tsv_ofs(result_tsv_path);
		result_tsv_ofs << std::string(" altitude \t transed \tcorrect \t error_distance [m] ") << std::endl;

		const auto& points_num = g_selected_video_org_points.size();
		for (size_t point_id = 0; point_id < points_num; point_id++)
		{
			const auto video_point = static_cast<cv::Point>(g_selected_video_org_points[point_id]);
			const auto& correct_ortho_point = g_correct_ortho_org_points[point_id];

			const auto& pix = transed_points_map.at<cv::Vec4f>(video_point);
			if ((int)pix[3] < 128)
				continue;

			const auto transformed_pt = cv::Point2f(pix[0], pix[1]);
			const auto misalignment_vec = (transformed_pt - correct_ortho_point) * meter_per_pix;
			const auto misalignment_norm = cv::norm(misalignment_vec);

			std::cout << "trans_pix: " << pix << std::endl;
			std::cout << "transformed_pt: " << transformed_pt << std::endl;
			std::cout << "misalignment: " << misalignment_norm << " [m]" << std::endl;
#ifdef _DEBUG
			std::cout << "misalignment: " << misalignment_vec << " (x_dir [m], y_dir [m])" << std::endl;
			std::cout << "ortho_lanes_inf(lane_dir.x, lane_dir.y, lane_latio[ortho / video]): "
				<< lanes_inf_map.at<cv::Vec4f>(video_point) << std::endl;
#endif

			std::cout << "\n\n************\n\n" << std::endl;

			result_tsv_ofs
				<< std::format(" {} \t {} \t {} \t {} ",
					std::format("({}, {})", video_point.x, video_point.y),
					std::format("({}, {})", transformed_pt.x, transformed_pt.y),
					std::format("({}, {})", correct_ortho_point.x, correct_ortho_point.y),
					misalignment_norm)
				<< std::endl;

			cv::circle(video_result_img, video_point, g_VIDEO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
			cv::circle(video_result_img, video_point, g_VIDEO_CIRCLE_RADIUS, cv::Scalar(0, 255, 0), -1);

			cv::circle(ortho_result_img, transformed_pt, g_ORTHO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
			cv::circle(ortho_result_img, transformed_pt, g_ORTHO_CIRCLE_RADIUS, cv::Scalar(0, 255, 0), -1);

			cv::circle(ortho_result_img, static_cast<cv::Point>(correct_ortho_point), g_ORTHO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
			cv::circle(ortho_result_img, static_cast<cv::Point>(correct_ortho_point), g_ORTHO_CIRCLE_RADIUS, cv::Scalar(0, 0, 255), -1);
		}

		const auto eval_video_path = std::format("io_images/ortho/{}_eval/result{}_video.png", video_code, experiment_id);
		cv::imwrite(eval_video_path, video_result_img);
		const auto eval_ortho_path = std::format("io_images/ortho/{}_eval/result{}_ortho.png", video_code, experiment_id);
		cv::imwrite(eval_ortho_path, ortho_result_img);
		experiment_id++;
	}
	cv::destroyAllWindows();
}

void PreMethodEval::Run(const std::string& video_code, const std::string& ortho_code, const float& meter_per_pix)
{
	std::vector hmg_altitude_points = {
		cv::Point2f(500.f, 790.f),
		cv::Point2f(550.f, 305.f),
		cv::Point2f(1190.f, 303.f),
		cv::Point2f(1450.f, 770.f)
	};
	std::vector hmg_ortho_points = {
		cv::Point2f(763.f, 1593.f),
		cv::Point2f(1595.f, 45.f),
		cv::Point2f(1770.f, 620.f),
		cv::Point2f(863.f, 2085.f)
	};

	const auto hmg_mat = cv::getPerspectiveTransform(hmg_altitude_points, hmg_ortho_points);

	auto video_img = cv::imread(std::format("io_images/{}/background.png", video_code));
	auto ortho_img = Func::GeoCvt::get_multicolor_mat(std::format("resources/{}/ortho.tif", ortho_code));

	auto video_org_img = video_img.clone();
	auto ortho_org_img = ortho_img.clone();

	const auto video_mask = cv::imread(std::format("resources/{}/road_mask.png", video_code));
	const auto ortho_mask = cv::imread(std::format("resources/{}/road_mask.png", ortho_code));

	cv::addWeighted(video_org_img, 0.85, video_mask, 0.15, 1.0, video_org_img);
	cv::addWeighted(ortho_org_img, 0.85, ortho_mask, 0.15, 1.0, ortho_org_img);

	cv::resize(video_org_img, video_img, cv::Size(), g_RATIO_REV, g_RATIO_REV);
	cv::resize(ortho_org_img, ortho_img, cv::Size(), g_RATIO_REV, g_RATIO_REV);

	uint64_t experiment_id = 0;
	while (true)
	{
		std::string input;
		std::cout << "Eval system: continue? 'y' : others" << std::endl;
		std::cin >> input;
		if (input != "y")
			break;

		input_points_interface(video_code, ortho_code, video_img, ortho_img);

		std::cout << "calculating....." << std::endl;
		std::cout << "************\n\n" << std::endl;

		auto video_result_img = video_org_img.clone();
		auto ortho_result_img = ortho_org_img.clone();

		const auto result_tsv_path = std::format("io_images/ortho/{}_eval/result_premethod{}.tsv", video_code, experiment_id);
		std::ofstream result_tsv_ofs(result_tsv_path);
		result_tsv_ofs << std::string(" altitude \t transed \tcorrect \t error_distance [m] ") << std::endl;

		std::vector<cv::Point2f> transformed_points;
		if (!g_selected_video_org_points.empty())
			cv::perspectiveTransform(g_selected_video_org_points, transformed_points, hmg_mat);

		const auto& points_num = g_selected_video_org_points.size();
		for (size_t point_id = 0; point_id < points_num; point_id++)
		{
			const auto video_point = static_cast<cv::Point>(g_selected_video_org_points[point_id]);
			const auto& correct_ortho_point = g_correct_ortho_org_points[point_id];
			const auto& transformed_pt = transformed_points[point_id];

			const auto misalignment_vec = (transformed_pt - correct_ortho_point) * meter_per_pix;
			const auto misalignment_norm = cv::norm(misalignment_vec);

			std::cout << "transformed_pt: " << transformed_pt << std::endl;
			std::cout << "misalignment: " << misalignment_norm << " [m]" << std::endl;
#ifdef _DEBUG
			std::cout << "misalignment: " << misalignment_vec << " (x_dir [m], y_dir [m])" << std::endl;
#endif

			std::cout << "\n\n************\n\n" << std::endl;

			result_tsv_ofs
				<< std::format(" {} \t {} \t {} \t {} ",
					std::format("({}, {})", video_point.x, video_point.y),
					std::format("({}, {})", transformed_pt.x, transformed_pt.y),
					std::format("({}, {})", correct_ortho_point.x, correct_ortho_point.y),
					misalignment_norm)
				<< std::endl;

			cv::circle(video_result_img, video_point, g_VIDEO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
			cv::circle(video_result_img, video_point, g_VIDEO_CIRCLE_RADIUS, cv::Scalar(0, 255, 0), -1);

			cv::circle(ortho_result_img, transformed_pt, g_ORTHO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
			cv::circle(ortho_result_img, transformed_pt, g_ORTHO_CIRCLE_RADIUS, cv::Scalar(0, 255, 0), -1);

			cv::circle(ortho_result_img, static_cast<cv::Point>(correct_ortho_point), g_ORTHO_CIRCLE_BLACK_RADIUS, cv::Scalar(0, 0, 0), -1);
			cv::circle(ortho_result_img, static_cast<cv::Point>(correct_ortho_point), g_ORTHO_CIRCLE_RADIUS, cv::Scalar(0, 0, 255), -1);
		}

		const auto eval_video_path = std::format("io_images/ortho/{}_eval/result_premethod{}_video.png", video_code, experiment_id);
		cv::imwrite(eval_video_path, video_result_img);
		const auto eval_ortho_path = std::format("io_images/ortho/{}_eval/result_premethod{}_ortho.png", video_code, experiment_id);
		cv::imwrite(eval_ortho_path, ortho_result_img);
		experiment_id++;

		g_selected_video_points.clear();
		g_selected_video_org_points.clear();
		g_correct_ortho_points.clear();
		g_correct_ortho_org_points.clear();
		cv::destroyAllWindows();
	}
}