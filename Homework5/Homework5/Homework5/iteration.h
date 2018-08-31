#pragma once
#include "CImg.h"

using namespace cimg_library;

class iteration {
public:
    iteration(const CImg<double> &);
    CImg<double> res;
private:
    double findThresh(const CImg<double> &);
    void getRes(const CImg<double> &, double);
    const int MAX_ITER = 500;
    const int L = 255;
};