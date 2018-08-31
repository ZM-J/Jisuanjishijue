#pragma once
#include "utility.h"
#include <string>
using namespace std;

class canny {
public:
	Mat img; //Original Image
	Mat thres; // Double threshold
private:
	unsigned int w, h;
	double lowThresh, highThresh;
public:
    canny(const string &); //Constructor
private:
	void toGrayScale(const Mat &, Mat &);
	Mat createGaussianFilter(int, int, double); //Creates a gaussian filter
	void sobel(const Mat &, Mat &, Mat &); //Sobel filtering
	void nonMaxSupp(const Mat &, const Mat &, Mat &); //Non-maxima supp.
	void threshold(const Mat &, double, double, Mat &); //Double threshold and finalize picture
	void selectThresholds(const Mat &);
};
