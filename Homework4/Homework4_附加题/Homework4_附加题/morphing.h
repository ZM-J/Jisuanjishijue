#pragma once
#include "utility.h"
#include "triangle.h"

class morphing {
public:
	morphing(const CImg<double> &from, const vector<Point> &from_kps, \
		const CImg<double> &to, const vector<Point> &kps, const vector<triangle> &tris, \
		int n, const string &dir);
private:
	void morphing_point(const CImg<double> &from, const vector<Point> &from_kps, \
		const CImg<double> &to, const vector<Point> &to_kps, const vector<Point> &cur_kps, \
		const vector<triangle> &tris, int x, int y, double scale, CImg<double> &dest);
};