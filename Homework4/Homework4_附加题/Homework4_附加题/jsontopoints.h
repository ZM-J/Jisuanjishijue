#pragma once
#include "utility.h"

class jsontopoints {
public:
	jsontopoints(const string &);
	vector<Point> keypoint;
	int w, h;
private:
	void convert(const string &);
};