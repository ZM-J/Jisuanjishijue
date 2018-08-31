#include "CImg.h"
#include "sift.h"
#include "siftmatch.h"
#include "ransac.h"
#include "mosaic.h"
#include <iostream>
#include <fstream>
using namespace cimg_library;
using namespace std;

int notmain(int argc, const char * argv[]) {
    CImg<unsigned int> img1("Dataset/1/2.bmp");
    CImg<unsigned int> img2("Dataset/1/1.bmp");
    unsigned int red[] = { 255,0,0 };
    unsigned int green[] = { 0,255,0 };
    sift sf1(img1);
    sift sf2(img2);
    siftmatch matching(sf1, sf2);
    // 看效果
    CImg<unsigned int> dispimg(img1.width() + img2.width(), img1.height(), 1, 3);
    for (int x = 0; x < img1.width(); x++)
    for (int y = 0; y < img1.height(); y++)
    for (int c = 0; c < 3; c++)
    dispimg(x, y, 0, c) = img1(x, y, 0, c);
    for (int x = 0; x < img2.width(); x++)
    for (int y = 0; y < img2.height(); y++)
    for (int c = 0; c < 3; c++)
    dispimg(img1.width() + x, y, 0, c) = img2(x, y, 0, c);
    for (auto i : matching.matches) {
    dispimg.draw_arrow(sf1.points[i.first].ix, sf1.points[i.first].iy,
    img1.width() + sf2.points[i.second].ix, sf2.points[i.second].iy,
    red);
    }
    dispimg.display();

    ransac rs(sf1, sf2, matching);

    // RANSAC特征点匹配效果

    CImg<double> imgrs(img1.width(), img1.height(), 1, 3, 0.0);
    for (auto i : matching.matches) {
    VlSiftKeypoint p1 = sf1.points[i.first], p2 = sf2.points[i.second];
    CImg<double> d2(1, 3, 1, 1,
    p2.x + 1.0,
    p2.y + 1.0,
    1.0);
    CImg<double> d1 = rs.H.get_invert() * d2;
    double x1hat = d1(0, 0) / d1(0, 2) - 1.0, y1hat = d1(0, 1) / d1(0, 2) - 1.0;
    imgrs.draw_point(p1.ix, p1.iy, red);
    imgrs.draw_point(x1hat, y1hat, green);
    }
    imgrs.display();

    mosaic ms(img1, img2, rs);


    return 0;
}

