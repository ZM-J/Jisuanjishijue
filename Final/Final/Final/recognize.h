#pragma once
#include "utility.h"
#include "segment.h"
#include "svm.h"

class recognize {
public:
    recognize(const segment &seg);
    vector<string> result;
private:
    svm_node* x;
    int max_nr_attr = 64;

    svm_model* model;
    int predict_probability = 0;
    void load_model();
    char predict(const Mat &img);
};