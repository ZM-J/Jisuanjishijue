#include "selroi.h"

selroi::selroi(const CImg<double>& img, const vector<Point>& ps, int w, int h)
{
	this->w = w;
	this->h = h;
	int lp = ps.size();
	p1 = ps[lp - 4], p2 = ps[lp - 1];
	getROI(img);
	getKeypoint(ps);
}

void selroi::getROI(const CImg<double>& img)
{
	roi = CImg<double>(w, h, 1, 3, 0);
	cimg_forXYC(roi, x, y, c) {
		Point rawP = getRawPoint(Point(x, y));
		roi(x, y, 0, c) = img.linear_atXYZC(rawP.real(), rawP.imag(), 0, c);
	}
}

void selroi::getKeypoint(const vector<Point>& ps)
{
	for (auto p : ps) {
		Point keyP = getPoint(p);
		keypoint.push_back(keyP);
	}
}

Point selroi::getRawPoint(const Point & p)
{
	double x = p.real(), y = p.imag();
	double x1 = p1.real(), y1 = p1.imag();
	double x2 = p2.real(), y2 = p2.imag();
	return Point(x1 + x*(x2 - x1) / (w - 1.0), y1 + y*(y2 - y1) / (h - 1.0));
}

Point selroi::getPoint(const Point & p)
{
	double x = p.real(), y = p.imag();
	double x1 = p1.real(), y1 = p1.imag();
	double x2 = p2.real(), y2 = p2.imag();
	return Point((w - 1.0)*(x - x1) / (x2 - x1), (h - 1.0)*(y - y1) / (y2 - y1));
}
