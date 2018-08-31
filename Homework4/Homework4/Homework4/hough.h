#pragma once

#include "utility.h"

class hough
{
public:
	hough(const Mat & edge);
	Mat vote;
	double rhostep, thetastep;
private:
	unsigned int w, h;
	Mat hEdge;
	void DeleteSmallComponent();
	void DeleteBorderComponent();
	void DeleteComponent(int x, int y);
	static const int BorderWidth = 3; // 
};