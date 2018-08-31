//
//  canny.h
//  Canny Edge Detector
//
//  Created by Hasan Akgün on 21/03/14.
//  Copyright (c) 2014 Hasan Akgün. All rights reserved.
//  Hasaki is SB
//

#ifndef _CANNY_
#define _CANNY_
#include "CImg.h"
#include <vector>

using namespace std;
using namespace cimg_library;

typedef double PixelType;
typedef CImg<PixelType> Mat;

const double PI = acos(-1.0);

class canny {
private:
	Mat img; //Original Image
	Mat grayscaled; // Grayscale
	Mat gFiltered; // Gradient
    Mat sFiltered; // Sobel Filtered
    Mat angles; // Angle Map
    Mat nonMaxSupped; // Non-maxima supp.
    Mat thres; // Double threshold
	Mat thin;// thinning final
	unsigned int w, h;
	double lowThresh, highThresh;
public:
    canny(string); //Constructor
private:
	void toGrayScale();
	Mat createGaussianFilter(int, int, double); //Creates a gaussian filter
    void sobel(); //Sobel filtering
    void nonMaxSupp(); //Non-maxima supp.
    void threshold(double, double); //Double threshold and finalize picture
	void selectThresholds();
	void thinning();
};

#endif
