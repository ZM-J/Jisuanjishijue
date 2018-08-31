#pragma once
#include "utility.h"

class selroi {
public:
	selroi(const CImg<double> &img, const vector<Point> &ps, int w, int h);
	CImg<double> roi;
	vector<Point> keypoint;
private:
	Point p1, p2;
	int w, h;
	void getROI(const CImg<double>& img);
	void getKeypoint(const vector<Point>& ps);
	Point getRawPoint(const Point &p);
	Point getPoint(const Point &p);
};