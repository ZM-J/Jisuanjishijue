#pragma once
#include "utility.h"
#include "point3d.h"
#include <vector>
using namespace std;

class result
{
public:
	result(const Mat &, const vector<point3d> &, double, double, double);
	Mat res;
private:
	int w, h;
};