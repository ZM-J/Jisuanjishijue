#include "table.h"
#include <sstream>
using namespace std;

table::table()
{
    ws = wb.active_sheet();
    makeTH();
    curRow = 2;
}

void table::addLine(const string & name, const Point &jd1, const Point &jd2, const Point &jd3, const Point &jd4, const string & sj, const string & sfz)
{
    stringstream ss;
    ss << "A" << curRow;
    ws.cell(ss.str()).value(name);
    ss.str("");
    ss << "B" << curRow;
    addPointValue(ss.str(), jd1);
    ss.str("");
    ss << "C" << curRow;
    addPointValue(ss.str(), jd2);
    ss.str("");
    ss << "D" << curRow;
    addPointValue(ss.str(), jd3);
    ss.str("");
    ss << "E" << curRow;
    addPointValue(ss.str(), jd4);
    ss.str("");
    ss << "F" << curRow;
    ws.cell(ss.str()).value(sj);
    ss.str("");
    ss << "G" << curRow;
    ws.cell(ss.str()).value(sfz);
    // �кŵ���
    curRow++;
}

void table::tofile(const string & filename)
{
    wb.save(filename);
}

void table::makeTH()
{
    ws.cell("A1").value(u8"ͼ����");
    ws.cell("B1").value(u8"�ǵ�1");
    ws.cell("C1").value(u8"�ǵ�2");
    ws.cell("D1").value(u8"�ǵ�3");
    ws.cell("E1").value(u8"�ǵ�4");
    ws.cell("F1").value(u8"�ֻ���");
    ws.cell("G1").value(u8"���֤��");
}

void table::addPointValue(const string & coord, const Point & pt)
{
    stringstream ss;
    // ��������ǵ�
    ss << (int)pt.real() << ',' << (int)pt.imag();
    ws.cell(coord).value(ss.str());
}
