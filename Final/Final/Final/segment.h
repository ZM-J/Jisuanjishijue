#pragma once
#include "utility.h"
#include <vector>

class segment {
public:
    segment(const Mat& img);
    vector<vector<pair<Mat, Point>>> res;
private:
    const double L = 255.0;
    const double deltaMax = 2.33 / L;
    const int margin = 32;
    const double UNVISITED = 0.0;
    const double VISITED = 1.0;
    const double SELECTED = 2.0;
    double thresh1 = 0.333; // ÀàËÆcannyµÄË«ãÐÖµ
    double thresh2 = 0.666;
    int minx, maxx;
    int miny, maxy;
    int prevMinY;
    int prevMaxY;
    int w, h;
    Mat visited;
    vector<Point> curPoint;
    void segment_floodfill(const Mat &img, int x, int y);
    void number_floodfill(const Mat &img, int x, int y);
    Mat thinning(const Mat & img);
};