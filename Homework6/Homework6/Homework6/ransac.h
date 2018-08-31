#pragma once
#include "sift.h"
#include "siftmatch.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
using namespace std;

class ransac {
public:
    ransac(const sift &sf1, const sift &sf2, const siftmatch &sm);
    CImg<double> H;
    double okratio;
private:
    void genH(const sift &sf1, const sift &sf2, const siftmatch &sm);
    void genDrift(const sift &sf1, const sift &sf2, const siftmatch &sm);
    void DriftOptimal(const sift &sf1, const sift &sf2, const siftmatch &sm);
    void buildSolve(int offset, const VlSiftKeypoint & from, const VlSiftKeypoint & to,
        CImg<double> & solve_matrix, CImg<double> &solve_vector);
    CImg<double> curH;
    int okcnt, curokcnt;
    const double okthresh = 6.0; // 参数来源matlab中ransac代码
};