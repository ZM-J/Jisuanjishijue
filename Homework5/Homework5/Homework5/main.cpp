#define XMD_H
#define cimg_use_jpeg
#include "CImg.h"
#include "iteration.h"
#include "otsu.h"
#include <sstream>
#include <iostream>

using namespace cimg_library;
using namespace std;

int main() {
    for (int i = 1; i <= 100; i++) {
        stringstream ss, itss, otss;
        ss << "Dataset/" << i << ".jpg";
        itss << "Iteration/" << i << ".jpg";
        otss << "Otsu/" << i << ".jpg";
        CImg<double> img(ss.str().c_str());
        img.RGBtoYCbCr().channel(0);
        iteration it(img);
        it.res.save(itss.str().c_str());
        otsu ot(img);
        ot.res.save(otss.str().c_str());
    }
    return 0;
}