#include "cylindral.h"
#include <algorithm>
using namespace std;

cylindral::cylindral(const CImg<unsigned int>& img)
{
    w = img.width(), h = img.height();
    setParams();
    double midw = (w - 1.0) / 2.0, midh = (h - 1.0) / 2.0;
    double minx = s * atan(-midw / f);
    double miny = s * -midh / f;
    double maxx = s * atan(midw / f);
    double maxy = s * midh / f;
    res = CImg<unsigned int>((int)(maxx - minx + 1), (int)(maxy - miny + 1), 1, img.spectrum(), 0);
    cimg_forXY(res, x, y) {
        double pyx = x + minx;
        double pyy = y + miny;
        double rawx = f * tan(pyx / s) + midw;
        double rawy = f * (pyy / s) / cos(pyx / s) + midh;
        if (rawx >= 0 && rawx < w && rawy >= 0 && rawy < h) {
            cimg_forC(res, c) {
                res(x, y, 0, c) = (unsigned int)img.linear_atXYZC(rawx, rawy, 0, c);
            }
        }
    }
}

void cylindral::setParams()
{
    focal_length = 8.0; // 单位：mm
    CCD_width = 1.0 / 1.8; // 单位：英寸
    double danwei = 0.0393701; // 1mm=0.0393701in
    f = max(w, h) * focal_length / CCD_width * danwei; // 单位：像素
    // 通常设成s=f
    s = f;
}
