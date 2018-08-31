#include "segment.h"
#include <queue>
#include <iostream>
using namespace std;

segment::segment(const Mat &img)
{
    w = img.width(), h = img.height();
    // 归一化
    Mat normImg = (img - img.min()) / (img.max() - img.min());
    visited = Mat(w, h, 1, 1, UNVISITED);
    for (int i = margin; i < w - margin; i++)
        for (int j = margin; j < h - margin; j++) {
            if (visited(i, j) == UNVISITED && normImg(i, j) < thresh1) {
                // cout << i << ' ' << j << ' ' << img(i, j) << endl;
                segment_floodfill(normImg, i, j);
            }
        }
    // visited.display();
    prevMinY = h;
    prevMaxY = 0;
    for (int j = h - margin - 1; j >= margin; j--) {
        for (int i = margin; i < w - margin; i++) {
            if (visited(i, j) == VISITED) {
                number_floodfill(normImg, i, j);
            }
        }
    }
    reverse(res.begin(), res.end());
    for (auto &row : res) {
        sort(row.begin(), row.end(), [](const pair<Mat,Point> &a, const pair<Mat,Point> &b) -> bool {
            return a.second.real() < b.second.real();
        });
    }
}

void segment::segment_floodfill(const Mat &img, int x, int y)
{
    // 队列
    queue<Point> q;
    // 初始
    visited(x, y) = VISITED;
    q.push(Point(x, y));
    while (!q.empty()) {
        int nowx = q.front().real();
        int nowy = q.front().imag();
        q.pop();
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nxtx = nowx + dx;
                int nxty = nowy + dy;
                if (nxtx >= margin && nxtx < w - margin && nxty >= margin && nxty < h - margin) {
                    if (visited(nxtx, nxty) == UNVISITED && img(nxtx, nxty) < thresh2) { //fabs(img(nxtx, nxty) - img(nowx, nowy) <= deltaMax)
                        visited(nxtx, nxty) = VISITED;
                        q.push(Point(nxtx, nxty));
                    }
                }
            }
        }
    }
    // TEST: mark
    /*
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int nxtx = x + dx;
            int nxty = y + dy;
            visited(nxtx, nxty) = 2.0;
        }
    }
    */
}

void segment::number_floodfill(const Mat &img, int x, int y)
{
    // 队列
    queue<Point> q;
    curPoint.clear();
    // 初始
    visited(x, y) = SELECTED;
    q.push(Point(x, y));
    curPoint.push_back(Point(x, y));
    minx = x, maxx = x;
    miny = y, maxy = y;
    while (!q.empty()) {
        int nowx = q.front().real();
        int nowy = q.front().imag();
        q.pop();
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nxtx = nowx + dx;
                int nxty = nowy + dy;
                if (nxtx >= margin && nxtx < w - margin && nxty >= margin && nxty < h - margin) {
                    if (visited(nxtx, nxty) == VISITED) {
                        visited(nxtx, nxty) = SELECTED;
                        q.push(Point(nxtx, nxty));
                        curPoint.push_back(Point(nxtx, nxty));
                        minx = min(minx, nxtx), maxx = max(maxx, nxtx);
                        miny = min(miny, nxty), maxy = max(maxy, nxty);
                    }
                }
            }
        }
    }
    int fiveX = -1, fiveY = -1;
    int xiaomargin = 3;
    // 处理5的那个横
    for (int i = (minx + maxx) / 2; i < maxx + xiaomargin && fiveX == -1; i++) {
        for (int j = miny - xiaomargin; j < (miny + maxy) / 2 && fiveX == -1; j++) {
            if (visited(i, j) == VISITED) {
                fiveX = i;
                fiveY = j;
                break;
            }
        }
    }
    // 如果有的话 那就搜
    if (fiveX != -1) {
        // 初始
        visited(fiveX, fiveY) = SELECTED;
        q.push(Point(fiveX, fiveY));
        curPoint.push_back(Point(fiveX, fiveY));
        minx = min(minx, fiveX), maxx = max(maxx, fiveX);
        miny = min(miny, fiveY), maxy = max(maxy, fiveY);
        while (!q.empty()) {
            int nowx = q.front().real();
            int nowy = q.front().imag();
            q.pop();
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    int nxtx = nowx + dx;
                    int nxty = nowy + dy;
                    if (nxtx >= margin && nxtx < w - margin && nxty >= margin && nxty < h - margin) {
                        if (visited(nxtx, nxty) == VISITED) {
                            visited(nxtx, nxty) = SELECTED;
                            q.push(Point(nxtx, nxty));
                            curPoint.push_back(Point(nxtx, nxty));
                            minx = min(minx, nxtx), maxx = max(maxx, nxtx);
                            miny = min(miny, nxty), maxy = max(maxy, nxty);
                        }
                    }
                }
            }
        }
    }
    int curW = maxx - minx + 1, curH = maxy - miny + 1;
    Mat tempRes(curW, curH, 1, 1, 0.0);
    Point c((maxx + minx) / 2.0, (maxy + miny) / 2.0);
    for (auto p : curPoint) {
        /*
        if (p.real() < minx || p.real() > maxx || p.imag() < miny || p.imag() > maxy) {
            cout << p.real() << ',' << p.imag() << endl;
            cout << minx << ',' << miny << endl;
            cout << maxx << ',' << maxy << endl;
        }
        */
        tempRes(p.real() - minx, p.imag() - miny) = 1.0 - img(p.real(), p.imag()); // 1.0
    }
    int maxCurWH = max(curW, curH);
    double resCenter = (maxCurWH - 1.0) / 2.0;
    Mat curRes(maxCurWH, maxCurWH, 1, 1, 0.0);
    cimg_forXY(curRes, x, y) {
        double tempX = x - resCenter + c.real() - minx;
        double tempY = y - resCenter + c.imag() - miny;
        if (tempX >= 0 && tempX < curW && tempY >= 0 && tempY < curH) {
            curRes(x, y) = tempRes.linear_atXY(tempX, tempY);
        }
    }
    // 不能改笔触的粗细
    // TODO: 是不是要改一改resize的规则？
    tempRes = curRes.get_resize(20, 20, 1, 1, 3); // dilate？
    Mat binRes(tempRes);
    cimg_forXY(binRes, x, y) {
        binRes(x, y) = tempRes(x, y) > 0.05 ? 1.0 : 0.0;
    }
    binRes = thinning(binRes);
    binRes.dilate(3);
    curRes = Mat(28, 28, 1, 1, 0.0);
    double tempMax = tempRes.max();
    cimg_forXY(tempRes, x, y) {
        curRes(x + 4, y + 4) = 0.25 * binRes(x, y) * tempRes.max() + 0.75 * tempRes(x, y);// tempRes(x, y) tempRes(x, y) / maxGrayscale : 0.0;
    }
    double maxGrayscale = curRes.max();
    cimg_forXY(curRes, x, y) {
        curRes(x, y) /= maxGrayscale;
        curRes(x, y) == curRes(x, y) < thresh1 ? 0.0: curRes(x, y);
    }
    if (res.empty()) {
        res.push_back(vector<pair<Mat, Point>>(1,make_pair(curRes, c)));
    }
    else {
        double prevCenterY = res.back().back().second.imag();
        if (max(prevMinY, miny) <= min(prevMaxY, maxy)) {
            res.back().push_back(make_pair(curRes, c));
        }
        else {
            res.push_back(vector<pair<Mat, Point>>(1, make_pair(curRes, c)));
        }
    }
    prevMinY = miny;
    prevMaxY = maxy;
}

Mat segment::thinning(const Mat & src) {
    // thres -> thin
    // 细化。边缘宽度 = 1
    int w = src.width(), h = src.height();
    int count = 0;  //记录迭代次数
    Mat dst(src);
    while (true)
    {
        vector<Point> mFlag; //用于标记需要删除的点
                                    //对点标记
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                //如果满足四个条件，进行标记
                //  p9 p2 p3
                //  p8 p1 p4
                //  p7 p6 p5
                double p1 = dst(i,j);
                if (p1 != 1.0) continue;
                double p4 = (i == w - 1) ? 0.0 : dst(i + 1,j);
                double p8 = (i == 0) ? 0.0 : dst(i - 1, j);
                double p2 = (j == 0) ? 0.0 : dst(i, j - 1);
                double p3 = (j == 0 || i == w - 1) ? 0.0 : dst(i + 1, j - 1);
                double p9 = (j == 0 || i == 0) ? 0.0 : dst(i - 1, j - 1);
                double p6 = (j == h - 1) ? 0.0 : dst(i, j + 1);
                double p5 = (j == h - 1 || i == w - 1) ? 0.0 : dst(i + 1, j + 1);
                double p7 = (j == h - 1 || i == 0) ? 0.0 : dst(i - 1, j + 1);
                if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2.0 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6.0)
                {
                    int ap = 0;
                    if (p2 == 0.0 && p3 == 1.0) ++ap;
                    if (p3 == 0.0 && p4 == 1.0) ++ap;
                    if (p4 == 0.0 && p5 == 1.0) ++ap;
                    if (p5 == 0.0 && p6 == 1.0) ++ap;
                    if (p6 == 0.0 && p7 == 1.0) ++ap;
                    if (p7 == 0.0 && p8 == 1.0) ++ap;
                    if (p8 == 0.0 && p9 == 1.0) ++ap;
                    if (p9 == 0.0 && p2 == 1.0) ++ap;

                    if (ap == 1 && p2 * p4 * p6 == 0.0 && p4 * p6 * p8 == 0.0)
                    {
                        //标记
                        mFlag.push_back(Point(i,j));
                    }
                }
            }
        }

        //将标记的点删除
        for (auto p : mFlag) {
            dst(p.real(), p.imag()) = 0.0;
        }

        //直到没有点满足，算法结束
        if (mFlag.empty())
        {
            break;
        }
        else
        {
            mFlag.clear();//将mFlag清空
        }

        //对点标记
        for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
                //如果满足四个条件，进行标记
                //  p9 p2 p3
                //  p8 p1 p4
                //  p7 p6 p5
                double p1 = dst(i, j);
                if (p1 != 1.0) continue;
                double p4 = (i == w - 1) ? 0.0 : dst(i + 1, j);
                double p8 = (i == 0) ? 0.0 : dst(i - 1, j);
                double p2 = (j == 0) ? 0.0 : dst(i, j - 1);
                double p3 = (j == 0 || i == w - 1) ? 0.0 : dst(i + 1, j - 1);
                double p9 = (j == 0 || i == 0) ? 0.0 : dst(i - 1, j - 1);
                double p6 = (j == h - 1) ? 0.0 : dst(i, j + 1);
                double p5 = (j == h - 1 || i == w - 1) ? 0.0 : dst(i + 1, j + 1);
                double p7 = (j == h - 1 || i == 0) ? 0.0 : dst(i - 1, j + 1);

                if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2.0 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6.0)
                {
                    int ap = 0;
                    if (p2 == 0 && p3 == 1) ++ap;
                    if (p3 == 0 && p4 == 1) ++ap;
                    if (p4 == 0 && p5 == 1) ++ap;
                    if (p5 == 0 && p6 == 1) ++ap;
                    if (p6 == 0 && p7 == 1) ++ap;
                    if (p7 == 0 && p8 == 1) ++ap;
                    if (p8 == 0 && p9 == 1) ++ap;
                    if (p9 == 0 && p2 == 1) ++ap;

                    if (ap == 1 && p2 * p4 * p8 == 0.0 && p2 * p6 * p8 == 0.0)
                    {
                        //标记
                        mFlag.push_back(Point(i, j));
                    }
                }
            }
        }

        //将标记的点删除
        for (auto p : mFlag) {
            dst(p.real(), p.imag()) = 0.0;
        }

        //直到没有点满足，算法结束
        if (mFlag.empty())
        {
            break;
        }
        else
        {
            mFlag.clear();//将mFlag清空
        }
    }
    return dst;
}
