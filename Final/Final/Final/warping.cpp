#include "warping.h"
#include <iostream>
using namespace std;

warping::warping(const Mat & img, const vector<Point>& jd)
{
	w = img.width(), h = img.height();
	// 左上
	zuoshang = getPoint(jd, Point(0, 0));
	// 左下
	zuoxia = getPoint(jd, Point(0, h-1));
	// 右上
	youshang = getPoint(jd, Point(w-1, 0));
	// 右下
	youxia = getPoint(jd, Point(w-1, h-1));
	getPaperSize();
	// 创建纸的矩阵
	paper = Mat(paper_w, paper_h, 1, 3);
	// 构建解矩阵与向量
	solve_matrix = Mat(8, 8, 1, 1, 0);
	solve_vector = Mat(1, 8, 1, 1, 0);
	buildSolve(0, Point(1, 1), zuoshang);
	buildSolve(2, Point(1, paper_h), zuoxia);
	buildSolve(4, Point(paper_w, 1), youshang);
	buildSolve(6, Point(paper_w, paper_h), youxia);
	// 解系数
	solve_vector.solve(solve_matrix);
	// 构建变换矩阵
	warp_matrix = Mat(3, 3, 1, 1, 1);
	cimg_forY(solve_vector, y) {
		warp_matrix(y%3, y/3) = solve_vector(y);
	}
	// 应用坐标变换
	cimg_forXY(paper, u, v) {
		Mat paper_pos(1, 3, 1, 1,
			u + 1,
			v + 1,
			1);
		Mat origin_pos = warp_matrix * paper_pos;
		double x = origin_pos(0) / origin_pos(2);
		double y = origin_pos(1) / origin_pos(2);
		cimg_forC(paper, c) {
			paper(u, v, 0, c) = img.linear_atXY(x,y,0,c);
		}
	}
	AssessAverage();
}

Point warping::getPoint(const vector<Point> &jd, const Point &origin)
{
	Point ret;
	double dist = HUGE_VAL;
	for (auto p : jd) {
		if (abs(p - origin) < dist) {
			dist = abs(p - origin);
			ret = p;
		}
	}
	return ret;
}

void warping::getPaperSize()
{
	// 几何平均
	paper_w = sqrt(abs(youshang - zuoshang)*abs(youxia - zuoxia));
	paper_h = sqrt(abs(zuoxia - zuoshang)*abs(youxia - youshang));
}

void warping::buildSolve(int offset, const Point & from, const Point & to)
{
	for (int i = 0; i <= 1; i++) {
		// 前半段
		solve_matrix(3*i, offset + i) = from.real();
		solve_matrix(3*i+1, offset + i) = from.imag();
		solve_matrix(3*i+2, offset + i) = 1;
	}
	// 后半段
	solve_matrix(6, offset) = -from.real() * to.real();
	solve_matrix(7, offset) = -from.imag() * to.real();
	solve_vector(offset) = to.real();
	solve_matrix(6, offset+1) = -from.real() * to.imag();
	solve_matrix(7, offset+1) = -from.imag() * to.imag();
	solve_vector(offset+1) = to.imag();
}

void warping::AssessAverage() {
	// 算面积
	double area = 0;
	Point p1, p2;
	p1 = zuoshang, p2 = youshang;
	area += p1.imag() * p2.real() - p2.imag() * p1.real();
	p1 = youshang, p2 = youxia;
	area += p1.imag() * p2.real() - p2.imag() * p1.real();
	p1 = youxia, p2 = zuoxia;
	area += p1.imag() * p2.real() - p2.imag() * p1.real();
	p1 = zuoxia, p2 = zuoshang;
	area += p1.imag() * p2.real() - p2.imag() * p1.real();
	area = abs(area / 2.0);
	cout << "Real area: " << area << endl;
	double arith_w = (abs(youshang - zuoshang) + abs(youxia - zuoxia)) / 2.0;
	double arith_h = (abs(zuoxia - zuoshang) + abs(youxia - youshang)) / 2.0;
	double arith_area = arith_w * arith_h;
	cout << "AM area: " << arith_area << " Rel Err: " << RelErr(arith_area, area) << endl;
	double geo_w = sqrt(abs(youshang - zuoshang)*abs(youxia - zuoxia));
	double geo_h = sqrt(abs(zuoxia - zuoshang)*abs(youxia - youshang));
	double geo_area = geo_w * geo_h;
	cout << "GM area: " << geo_area << " Rel Err: " << RelErr(geo_area, area) << endl;
	double har_w = 2 / (1 / abs(youshang - zuoshang) + 1 / abs(youxia - zuoxia));
	double har_h = 2 / (1 / abs(zuoxia - zuoshang) + 1 / abs(youxia - youshang));
	double har_area = har_w * har_h;
	cout << "HM area: " << har_area << " Rel Err: " << RelErr(har_area, area) << endl;
}

double warping::RelErr(double x, double y) {
	return (x - y) / y;
}