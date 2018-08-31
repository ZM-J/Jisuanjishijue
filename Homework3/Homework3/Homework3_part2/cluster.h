#pragma once
#include "utility.h"
#include "point3d.h"
#include <utility>
#include <vector>

using namespace std;

class cluster {
public:
	cluster(const Mat &);
	vector<point3d> center;
	int n_cluster;
private:
	vector<point3d> p;
	vector<int> group;
	vector<double> weight; // 权重
	int w, h, d;
	void binarize(const Mat &);
	void kruskal();
	int getGroup(int);
};