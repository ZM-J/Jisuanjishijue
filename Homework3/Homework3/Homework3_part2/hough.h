#pragma once

#include "utility.h"

class hough
{
public:
	hough(const Mat & edge);
	Mat vote;
	double xstep, ystep, rstep;
private:
	unsigned int w, h;
	Mat hEdge;
	void DeleteSmallComponent();
	void Thinning();
	static const int BorderWidth = 3; // 
};