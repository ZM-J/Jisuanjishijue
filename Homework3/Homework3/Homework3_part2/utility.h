#pragma once
#include "CImg.h"
#include <complex>
using namespace cimg_library;
using namespace std;

typedef double PixelType;
typedef CImg<PixelType> Mat;
typedef complex<double> Point;

const int MAX_WH = 256;