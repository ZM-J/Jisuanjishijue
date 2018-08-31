#pragma once
#include "CImg.h"
#include "ransac.h"
#include <vector>
#include <algorithm>
using namespace cimg_library;
using namespace std;

class mosaic {
public:
    mosaic(const CImg<double> &img1, const CImg<double> &img2, const ransac &rs);
    CImg<unsigned int> res;
private:
    const double invalid_pix = -1.0; // -1.0
    void mulres(const CImg<double> &img1, const CImg<double> &img2);
    vector<CImg<double>> get_gaussian_pyramid(const CImg<double> &img, int numLayer);
    void get_laplacian_pyramid(vector<CImg<double>> &pyr);
};