#pragma once
#include "utility.h"
#include <utility>
#include <vector>

using namespace std;

class cluster {
public:
	cluster(const Mat &, int);
	vector<Point> center;
private:
	vector<Point> p;
	vector<int> group;
	vector<double> weight; // ШЈжи
	int w, h;
	void binarize(const Mat &);
	void kruskal(int);
	int getGroup(int);
	double b;
	void shift();
	void shiftback();
};