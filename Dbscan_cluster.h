#ifndef DBSCAN_H
#define DBSCAN_H
#include<cmath>
#include <iostream>
#include <vector>
#include<opencv2/opencv.hpp>


using namespace std;
namespace cv
{
namespace DBSCAN
{

	const int NOISE = -2;
	const int NOT_CLASSIFIED = -1;

	class Point {
	public:
		double x, y;
		int ptsCnt, cluster;
		Point(double _x, double _y, int _ptsCnt, int _cluster) : x(_x), y(_y), ptsCnt(_ptsCnt), cluster(_cluster){}
		
		double getDis(const Point & ot) {
			//return sqrt((x - ot.x)*(x - ot.x) + (y - ot.y)*(y - ot.y));
			//使用曼哈顿距离
			//return 1.2*abs((x - ot.x))+0.8*abs((y - ot.y));//生成视频使用的距离
			//return 5*abs((x - ot.x)) + 0.5*abs((y - ot.y));
			//return 3*abs((x - ot.x)) + 0.5*abs((y - ot.y));
			return 3 * std::abs((x - ot.x)) + 0.5*std::abs((y - ot.y));
			//return 5* abs((x - ot.x)) + 0.8*abs((y - ot.y));
			//使用LP-norm距离p=0.5                      # p:  (|x1|^p+|x2|^p+|x3|^p+|x4|^p)^(1/p)
			//return pow(sqrt(abs(x - ot.x)) + sqrt(abs(y - ot.y)),2);
		}

		double getDism(const Point & ot) {
			return 5* std::abs((x - ot.x)) + 0.2*std::abs((y - ot.y));
			//return sqrt((x - ot.x)*(x - ot.x) + (y - ot.y)*(y - ot.y));
			//return pow(sqrt(abs(x - ot.x)) + sqrt(abs(y - ot.y)), 2);

		}
	};
#if 1
	class parallelTestBody : public ParallelLoopBody//参考官方给出的answer，构造一个并行的循环体类  
	{
	public:
		parallelTestBody(int _MODE, vector<Point>&pt, double _eps, vector<vector<int>> &_adjPoints,int _size)//class constructor  
		{
			points = &pt[0];
			eps = _eps;
			adjPoints=_adjPoints.data();
			MODE = _MODE;
			size = _size;
		}
		virtual void operator ()(const Range& range) const //重载操作符（）  
		{

			for (int i = range.start; i < range.end; i++) {
				for (int j = 0; j < size; j++) {

					if (i == j) continue;
					if (MODE == 0)
					{
						if (points[i].getDis(points[j]) <= eps) {
							points[i].ptsCnt++;
							adjPoints[i].push_back(j);
						}
					}
					if (MODE == 1)
					{
						if (points[i].getDism(points[j]) <= eps) {
							points[i].ptsCnt++;
							adjPoints[i].push_back(j);
						}
					}
				}
			}
		}
		parallelTestBody& operator=(const parallelTestBody &) {
			return *this;
		};

	private:
		Point* points;
		double eps;
		vector<int>* adjPoints;
		int MODE;
		int size;
	};
#endif

	class DBCAN {
	public:
		int minPts;
		double eps;
		vector<Point> points;
		int size;
		vector<vector<int> > adjPoints;
		vector<bool> visited;
		vector<vector<int> > cluster;
		int clusterIdx;
		int MODE;
		DBCAN(double eps, int minPts, vector<Point> points,int mode) {
			
			this->eps = eps;
			this->minPts = minPts;
			this->points = points;
			this->size = (int)points.size();
			adjPoints.resize(size);
			this->clusterIdx = -1;
			this->MODE = mode;
		}
		void run() {
			checkNearPoints();

			for (int i = 0; i < size; i++) {
				if (points[i].cluster != NOT_CLASSIFIED) continue;

				if (isCoreObject(i)) {
					dfs(i, ++clusterIdx);
				}
				else {
					points[i].cluster = NOISE;
				}
			}

			cluster.resize(clusterIdx + 1);
			for (int i = 0; i < size; i++) {
				if (points[i].cluster != NOISE) {
					cluster[points[i].cluster].push_back(i);
				}
			}
		}

		void dfs(int now, int c) {
			points[now].cluster = c;
			if (!isCoreObject(now)) return;

			for (auto&next : adjPoints[now]) {
				if (points[next].cluster != NOT_CLASSIFIED) continue;
				dfs(next, c);
			}
		}
#if 1
		void checkNearPoints() {
			parallel_for_(Range(0, size), parallelTestBody(MODE,points,eps,adjPoints,size));
		}
#endif


		// is idx'th point core object?
		bool isCoreObject(int idx) {
			return points[idx].ptsCnt >= minPts;
		}

		vector<vector<int> > getCluster() {
			return cluster;
		}
	};
}
}
#endif