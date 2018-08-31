#pragma once
#include "CImg.h"

using namespace cimg_library;

class otsu {
public:
    otsu(const CImg<double> &);
    CImg<double> res;
private:
    double findThresh(const CImg<double> &);
    void getRes(const CImg<double> &, double);
    const double L = 255.0;
    const double step = 1.0;
};