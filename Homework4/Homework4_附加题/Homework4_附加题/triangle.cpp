#include "triangle.h"

triangle::triangle()
{
}

triangle::triangle(int a, int b, int c)
{
	id[0] = a;
	id[1] = b;
	id[2] = c;
}

CImg<double> triangle::getGravCoord(const vector<Point> &pts, const Point & p)
{
	double Ax = pts[id[0]].real(), Ay = pts[id[0]].imag();
	double Bx = pts[id[1]].real(), By = pts[id[1]].imag();
	double Cx = pts[id[2]].real(), Cy = pts[id[2]].imag();
	CImg<double> solving(3, 3, 1, 1,
		Ax,  Bx,  Cx,
		Ay,  By,  Cy,
		1.0, 1.0, 1.0);
	CImg<double> ret(1, 3, 1, 1,
		p.real(),
		p.imag(),
		1.0);
	return ret.solve(solving);
}

Point triangle::getGravPoint(const vector<Point>& pts, const CImg<double>& lambda)
{
	double Ax = pts[id[0]].real(), Ay = pts[id[0]].imag();
	double Bx = pts[id[1]].real(), By = pts[id[1]].imag();
	double Cx = pts[id[2]].real(), Cy = pts[id[2]].imag();
	CImg<double> solving(3, 2, 1, 1,
		Ax, Bx, Cx,
		Ay, By, Cy);
	CImg<double> vec = solving * lambda;
	return Point(vec(0), vec(1));
}

bool triangle::operator==(const triangle & b) const
{
	set<int> s, bs;
	for (auto i : id) {
		s.insert(i);
	}
	for (auto i : b.id) {
		bs.insert(i);
	}
	return s == bs;
}

bool triangle::circleContain(const vector<Point> &pts, int did) const
{
	double Ax = pts[id[0]].real(), Ay = pts[id[0]].imag();
	double Bx = pts[id[1]].real(), By = pts[id[1]].imag();
	double Cx = pts[id[2]].real(), Cy = pts[id[2]].imag();
	double Dx = pts[did].real(), Dy = pts[did].imag();
	// When A, B and C are sorted in a counterclockwise order
	if (sgn(((pts[id[1]] - pts[id[0]]) / (pts[id[2]] - pts[id[0]])).imag()) == 1) {
		swap(Bx, Cx);
		swap(By, Cy);
	}
	CImg<double> pd(4, 4, 1, 1,
		Ax, Ay, Ax*Ax + Ay*Ay, 1.0,
		Bx, By, Bx*Bx + By*By, 1.0,
		Cx, Cy, Cx*Cx + Cy*Cy, 1.0,
		Dx, Dy, Dx*Dx + Dy*Dy, 1.0);
	return sgn(pd.det()) == 1;
}

bool triangle::hasEdge(const pair<int, int>& e) const
{
	set<int> unionPoint;
	for (auto i : id) {
		unionPoint.insert(i);
	}
	unionPoint.insert(e.first);
	unionPoint.insert(e.second);
	return unionPoint.size() == 3;
}

pair<int, int> triangle::getEdge(int i)
{
	return make_pair(id[i], id[(i+1)%3]);
}
