#pragma once
#include "CImg.h"
#include <complex>
#include <vector>
#include <string>
#include <fstream>
#include <set>
#include <iostream>

using namespace cimg_library;
using namespace std;

typedef complex<double> Point;

const double eps = 1e-9;
int sgn(double x);