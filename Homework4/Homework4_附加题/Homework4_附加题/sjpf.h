#pragma once
#include "utility.h"
#include "triangle.h"

class sjpf {
public:
	sjpf(const vector<Point> &pts);
	vector<triangle> tris;
private:
	void BowyerWatson(const vector<Point> &pts);
};