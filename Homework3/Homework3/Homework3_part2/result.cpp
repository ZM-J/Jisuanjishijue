#include "result.h"
#include <iostream>
using namespace std;

result::result(const Mat & img, const vector<point3d> & params, double xstep, double ystep, double rstep)
{
	w = img.width();
	h = img.height();
	const PixelType red[] = { 255,0,0 }, yellow[] = { 255,255,0 };
	res = img;
	for (auto param : params) {
		double x0 = param.x * xstep;
		double y0 = param.y * ystep;
		double r = param.z * rstep;
		// scaling back 主要是针对rho的参数变换
		if (w > h) {
			if (w > MAX_WH) {
				x0 = x0 * w / MAX_WH;
				y0 = y0 * w / MAX_WH;
				r = r * w / MAX_WH;
			}
		}
		else {
			if (h > MAX_WH) {
				x0 = x0 * h / MAX_WH;
				y0 = y0 * h / MAX_WH;
				r = r * h / MAX_WH;
			}
		}
		res.draw_circle(x0, y0, r, red, 0.2);
	}
	ostringstream ss;
	ss << "circle num: " << params.size();
	res.draw_text(7, 7, ss.str().c_str(), red, yellow, 1, 13);
#ifdef DEBUG
	res.display();
#endif
}
