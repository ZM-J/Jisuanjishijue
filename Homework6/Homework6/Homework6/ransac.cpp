#include "ransac.h"

ransac::ransac(const sift & sf1, const sift & sf2, const siftmatch & sm)
{
    // 随机采样的次数
    int nSample = 1000;
    // 初始化
    okcnt = 0;
    srand(time(NULL));
    // 采样
    while (nSample--) {
        genDrift(sf1, sf2, sm);
        if (curokcnt > okcnt) {
            okcnt = curokcnt;
            H = curH;
        }
    }
    okratio = (double)okcnt / (double)sm.matches.size();
    cout << "OK ratio: " << okratio << endl;
    DriftOptimal(sf1, sf2, sm);
}

void ransac::genH(const sift & sf1, const sift & sf2, const siftmatch & sm)
{
    // 构建解向量与矩阵
    int l = sm.matches.size();
    CImg<double> solve_matrix(8, 8, 1, 1, 0.0);
    CImg<double> solve_vector(1, 8, 1, 1, 0.0);
    for (int i = 0; i < 8; i += 2) {
        int mid = rand() % l;
        buildSolve(i, sf1.points[sm.matches[mid].first], sf2.points[sm.matches[mid].second],
            solve_matrix, solve_vector);
    }
    // 解系数
    solve_vector.solve(solve_matrix);
    // 构建变换矩阵
    curH = CImg<double>(3, 3, 1, 1, 1.0);
    cimg_forY(solve_vector, y) {
        curH(y % 3, y / 3) = solve_vector(y);
    }
    // 计算curokcnt
    curokcnt = 0;
    // double okthresh = 6.0; // 参数来源matlab中ransac代码
    for (auto mid : sm.matches) {
        CImg<double> homo1(1, 3, 1, 1,
            sf1.points[mid.first].x+1.0,
            sf1.points[mid.first].y+1.0,
            1.0);
        CImg<double> homo2 = curH * homo1;
        double du = homo2(0, 0) / homo2(0, 2) - 1.0 - sf2.points[mid.second].x;
        double dv = homo2(0, 1) / homo2(0, 2) - 1.0 - sf2.points[mid.second].y;
        curokcnt += (int)(du*du + dv * dv < okthresh * okthresh);
    }
}

void ransac::genDrift(const sift & sf1, const sift & sf2, const siftmatch & sm)
{
    int l = sm.matches.size();
    // 构建变换矩阵
    int mid = rand() % l;
    curH = CImg<double>(3, 3, 1, 1,
        1.0, 0.0, sf2.points[sm.matches[mid].second].x - sf1.points[sm.matches[mid].first].x,
        0.0, 1.0, sf2.points[sm.matches[mid].second].y - sf1.points[sm.matches[mid].first].y,
        0.0, 0.0, 1.0);
    // 计算curokcnt
    curokcnt = 0;
    // double okthresh = 6.0; // 参数来源matlab中ransac代码
    for (auto mid : sm.matches) {
        CImg<double> homo1(1, 3, 1, 1,
            sf1.points[mid.first].x + 1.0,
            sf1.points[mid.first].y + 1.0,
            1.0);
        CImg<double> homo2 = curH * homo1;
        double du = homo2(0, 0) / homo2(0, 2) - 1.0 - sf2.points[mid.second].x;
        double dv = homo2(0, 1) / homo2(0, 2) - 1.0 - sf2.points[mid.second].y;
        curokcnt += (int)(du*du + dv * dv < okthresh * okthresh);
    }
}

void ransac::DriftOptimal(const sift & sf1, const sift & sf2, const siftmatch & sm)
{
    int nMatches = 0;
    double sumx = 0.0, sumy = 0.0;
    for (auto mid : sm.matches) {
        double dx = sf2.points[mid.second].x - sf1.points[mid.first].x - H(2, 0);
        double dy = sf2.points[mid.second].y - sf1.points[mid.first].y - H(2, 1);
        if (dx*dx + dy*dy < okthresh * okthresh) {
            nMatches++;
            sumx += sf2.points[mid.second].x - sf1.points[mid.first].x;
            sumy += sf2.points[mid.second].y - sf1.points[mid.first].y;
        }
    }
    H(2, 0) = sumx / nMatches;
    H(2, 1) = sumy / nMatches;
}

void ransac::buildSolve(int offset, const VlSiftKeypoint & from, const VlSiftKeypoint & to,
    CImg<double> & solve_matrix, CImg<double> &solve_vector)
{
    double fx = from.x + 1.0, fy = from.y + 1.0;
    double tx = to.x + 1.0, ty = to.y + 1.0;
    for (int i = 0; i <= 1; i++) {
        // 前半段
        solve_matrix(3 * i, offset + i) = fx;
        solve_matrix(3 * i + 1, offset + i) = fy;
        solve_matrix(3 * i + 2, offset + i) = 1.0;
    }
    // 后半段
    solve_matrix(6, offset) = -fx * tx;
    solve_matrix(7, offset) = -fy * tx;
    solve_vector(offset) = tx;
    solve_matrix(6, offset + 1) = -fx * ty;
    solve_matrix(7, offset + 1) = -fy * ty;
    solve_vector(offset + 1) = ty;
}
