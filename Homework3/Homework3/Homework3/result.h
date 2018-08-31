#pragma once
#include "utility.h"
#include <vector>
using namespace std;

class result
{
public:
	result(const Mat &, const vector<Point> &, double, double);
	Mat res;
	// ½»µã
	vector<Point> jd;
private:
	int w, h;
	void findJD(const vector<double> &, const vector<double> &);
	void selectJD(const vector<double> &, const vector<double> &);
};