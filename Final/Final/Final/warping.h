#pragma once
#include "utility.h"
#include <vector>

using namespace std;

class warping
{
public:
	warping(const Mat &, const vector<Point> &);
	Mat paper;
    Point zuoshang, zuoxia, youshang, youxia;
private:
	int w, h;
	int paper_w, paper_h;
	Point getPoint(const vector<Point> &, const Point &);
	void getPaperSize();
	void getPaper(const Mat &);
	Mat warp_matrix;
	Mat solve_matrix, solve_vector;
	void buildSolve(int, const Point &, const Point &);
	void AssessAverage();
	double RelErr(double x, double y);
};