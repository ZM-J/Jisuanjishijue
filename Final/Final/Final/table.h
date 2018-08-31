#pragma once
#include "utility.h"
#include <vector>
#include <xlnt/xlnt.hpp>
using namespace std;

class table {
public:
    table();
    void addLine(const string & name, const Point &jd1, const Point &jd2, const Point &jd3, const Point &jd4, const string & sj, const string & sfz);
    void tofile(const string &filename);
private:
    xlnt::workbook wb;
    xlnt::worksheet ws;
    int curRow;
    void makeTH();
    void addPointValue(const string &coord, const Point &pt);
};