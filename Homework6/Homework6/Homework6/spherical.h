#pragma once
#include "CImg.h"
using namespace cimg_library;

class spherical {
public:
    spherical(const CImg<unsigned int>& img);
    CImg<unsigned int> res;
private:
    void setParams();
    double focal_length;
    double CCD_width;
    double f, s;
    int w, h;
};