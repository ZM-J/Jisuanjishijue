#pragma once
#include "CImg.h"
#include <vector>
#include <fstream>
using namespace cimg_library;
using namespace std;

extern "C" {
#include <vl/generic.h>
#include <vl/sift.h>
}

class sift {
public:
    sift(const CImg<unsigned int>&);
    vector<vector<double>> d;
    vector<double> ang;
    vector<VlSiftKeypoint> points;
};