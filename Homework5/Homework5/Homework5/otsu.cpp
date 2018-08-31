#include "otsu.h"

otsu::otsu(const CImg<double>& img)
{
    double th = findThresh(img);
    getRes(img, th);
}

double otsu::findThresh(const CImg<double>& img)
{
    double th = 0.0, g = 0.0;
    for (double cur_th = 0.0; cur_th <= L; cur_th += step) {
        // 求前景与背景的点数比例与平均灰度
        int n1 = 0, n2 = 0;
        double u1 = 0, u2 = 0;
        cimg_forXY(img, x, y) {
            if (img(x, y) < cur_th) {
                u1 += img(x, y);
                n1++;
            }
            else {
                u2 += img(x, y);
                n2++;
            }
        }
        // 防止被零除
        if (n1 == 0 || n2 == 0) {
            continue;
        }
        u1 /= n1, u2 /= n2;
        double w1 = (double)n1 / img.size(), w2 = (double)n2 / img.size();
        double cur_g = w1 * w2 * (u1 - u2) * (u1 - u2);
        if (cur_g > g) {
            g = cur_g;
            th = cur_th;
        }
    }
    return th;
}

void otsu::getRes(const CImg<double>& img, double thresh)
{
    res.resize(img);
    cimg_forXY(img, x, y) {
        res(x, y) = img(x, y) < thresh ? 0 : L;
    }
}
