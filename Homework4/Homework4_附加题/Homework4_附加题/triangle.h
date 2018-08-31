#pragma once
#include "utility.h"

class triangle {
public:
	triangle();
	triangle(int a, int b, int c);
	CImg<double> getGravCoord(const vector<Point> &pts, const Point &p);
	Point getGravPoint(const vector<Point> &pts, const CImg<double> &lambda);
	bool operator ==(const triangle &b) const;
	bool circleContain(const vector<Point> &pts, int did) const;
	bool hasEdge(const pair<int, int> &e) const;
	pair<int, int> getEdge(int i);
	int id[3];
};