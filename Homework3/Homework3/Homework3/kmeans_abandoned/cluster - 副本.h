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
	vector<double> d;
	int w, h;
	void binarize(const Mat &);
	void kmeans(int);
	void kmeans_pick(int);
	int nearest(int, int);
	double b;
	void shift();
	void shiftback();
};