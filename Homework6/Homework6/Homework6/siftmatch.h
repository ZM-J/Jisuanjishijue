#pragma once
#include "sift.h"

extern "C"{
#include <vl/kdtree.h>
}

class siftmatch {
public:
    siftmatch(const sift & s1, const sift & s2);
    vector<pair<int, int>> matches;
private:
    VlKDForest * kdt;
};