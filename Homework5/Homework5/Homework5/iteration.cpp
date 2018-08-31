#include "iteration.h"

iteration::iteration(const CImg<double>& img)
{
    double th = findThresh(img);
    getRes(img, th);
}

double iteration::findThresh(const CImg<double>& img)
{
    // 初始值：直接取中位数
    double th = img.median();
    double mean1 = 0, mean2 = 0;
    for (int i = 1; i <= MAX_ITER; i++) {
        // 更新均值
        int n1 = 0, n2 = 0;
        cimg_forXY(img, x, y) {
            if (img(x, y) < th) {
                n1++;
                mean1 += img(x, y);
            }
            else {
                n2++;
                mean2 += img(x, y);
            }
        }
        mean1 /= n1;
        mean2 /= n2;
        // 更新划分值
        th = (mean1 + mean2) / 2.0;
    }
    return th;
}

void iteration::getRes(const CImg<double> & img, double thresh)
{
    res.resize(img);
    cimg_forXY(img, x, y) {
        res(x, y) = img(x, y) < thresh ? 0 : L;
    }
}
