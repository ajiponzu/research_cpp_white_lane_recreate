#pragma once
#include "ResourceProvider.h"
#include "Functions.h"

namespace Background
{
	void Create(const std::string& video_code,
		const size_t& spend_time = 500,
		const int& history = 500, const double& thr = 16.0);
};

namespace LaneDetection
{
	namespace Video
	{
		void ExtractCandidate(const std::string& video_code);
		void ChooseLanes(const std::string& video_code, const int& road_id);
		void ChooseAllLanes(const std::string& video_code, const int& roads_num);
	};

	namespace Ortho
	{
		void ExtractCandidate(const std::string& code);
		void ChooseLanes(const std::string& code, const int& road_id);
		void ChooseAllLanes(const std::string& code, const int& roads_num);
	};
};

namespace Registration
{
	struct MeshRect
	{
		// RotatedRect.pointsで要求されるメモリレイアウトに準拠した順番
		// 型が同じなので，順番を入れ替えても動作するが，確実にバグるので注意
		cv::Point2f bl;
		cv::Point2f tl;
		cv::Point2f tr;
		cv::Point2f br;

		cv::Point2f line_dir_vec = cv::Point2f(0.0f, 0.0f);
		double line_length = 0.0;
		bool is_white_lane_mesh = false;

		MeshRect() = default;
		MeshRect(const cv::Rect2f& rect)
		{
			tl = rect.tl();
			br = rect.br();
			tr = tl + cv::Point2f(rect.width, 0.0f);
			bl = tl + cv::Point2f(0.0f, rect.height);
		}
		// points{a, b, c, d} -> bl = a, tl = b, tr = c, br = d;
		MeshRect(const std::vector<cv::Point2f>& points)
		{
			if (points.size() != 4)
			{
				std::cout << "points size must be four." << std::endl;
				std::exit(-1);
			}
			bl = points[0];
			tl = points[1];
			tr = points[2];
			br = points[3];
		}

		void output_pts() const
		{
			std::cout << "bl, tl, tr, br: " << bl << tl << tr << br << std::endl;
		}

		void draw_pts(cv::Mat& img, const cv::Scalar& color = cv::Scalar(255, 255, 255)) const
		{
			const auto rev_color = cv::Scalar(255, 255, 255) - color;
			cv::circle(img, bl, 1, color, -1);
			cv::circle(img, tl, 1, rev_color, -1);
			cv::circle(img, tr, 1, rev_color, -1);
			cv::circle(img, br, 1, color, -1);
		}

		cv::Rect get_bounding_rect() const
		{
			const std::vector<cv::Point> points{ bl, tl, tr, br };
			return cv::boundingRect(points);
		}

		cv::Mat crop_img_ref(const cv::Mat& img) const
		{
			const auto rect = get_bounding_rect();

			return img(rect);
		}

		void fill_convex(cv::Mat& img, const cv::Scalar& color) const
		{
			const std::vector<cv::Point> points{ bl, tl, tr, br };
			cv::fillConvexPoly(img, points, color);
		}

		// true: norm(tb) > norm(lr), false: not
		bool which_use_vectors() const
		{
			return cv::norm(tr - tl) > cv::norm(bl - tl);
		}

		// 人が見た時の直感的なtl, bl,...とopencvが算出するtl, bl, ...は違うので,
		// 人の直感ベースで座標を割り当てなおす
		void arrange_pos()
		{
			MeshRect mesh_rect = *this;
			mesh_rect.bl = br;
			mesh_rect.br = tr;
			mesh_rect.tr = tl;
			mesh_rect.tl = bl;

			*this = mesh_rect;
		}

		std::vector<cv::Point2f> get_pt_list() const
		{
			return std::vector{ bl, tl, tr, br };
		}
	};

	class Registrator
	{
	public:
		Registrator(const int& road_num)
			: m_roadNum(road_num)
		{
			m_meshListHashes.resize(road_num);
		}

		void Run(const std::string& video_code, const std::string& ortho_code);

	private:
		std::vector<std::unordered_map<std::string, std::vector<MeshRect>>> m_meshListHashes;
		int m_roadNum;

		void DivideRoadByLane(const std::string& code, const int& road_id);
		void DrawRoadsByDividedArea(const std::string& video_code, const std::string& ortho_code);
		std::pair<cv::Mat, cv::Mat> DrawRoadByDividedArea(const std::string& video_code,
			const std::string& ortho_code, const int& road_id);
	};
};