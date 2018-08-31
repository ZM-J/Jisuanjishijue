#include "morphing.h"

morphing::morphing(const CImg<double>& from, const vector<Point> &from_kps, \
	const CImg<double>& to, const vector<Point> &to_kps, const vector<triangle>& tris \
	, int n, const string & dir)
{
	int w = from.width(), h = from.height();
	// 存开始的图片
	stringstream ssbegin;
	ssbegin << dir << 0 << ".bmp";
	from.save(ssbegin.str().c_str());
	for (int i = 1; i <= n; i++) {
		double scale = 1.0 * i / (n + 1.0);
		// 首先求出点集的中间点
		int lp = from_kps.size();
		vector<Point> cur_kps(lp);
		for (int i = 0; i < lp; i++) {
			cur_kps[i] = (1.0-scale) * from_kps[i] + scale * to_kps[i];
		}
		CImg<double> dest(w, h, 1, 3, 0);
		cimg_forXY(dest, x, y) {
			morphing_point(from, from_kps, to, to_kps, cur_kps, tris, x, y, scale, dest);
		}
		stringstream ss;
		ss << dir << i << ".bmp";
		dest.save(ss.str().c_str());
	}
	// 存结束的图片
	stringstream ssend;
	ssend<< dir << n+1 << ".bmp";
	to.save(ssend.str().c_str());
}

void morphing::morphing_point(const CImg<double>& from, const vector<Point> &from_kps, \
	const CImg<double>& to, const vector<Point> &to_kps, const vector<Point> &cur_kps, \
	const vector<triangle>& tris, int x, int y, double scale, CImg<double> &dest)
{
	triangle cur_t;
	CImg<double> lambda;

	for (auto t : tris) {
		lambda = t.getGravCoord(cur_kps, Point(x, y));
		// 判断点在三角形内的依据：重心坐标各分量均非负
		bool ouligay = true;
		cimg_forXY(lambda, x, y) {
			if (sgn(lambda(x, y)) == -1) {
				ouligay = false;
			}
		}
		if (ouligay) {
			cur_t = t;
			break;
		}
	}

	Point from_p = cur_t.getGravPoint(from_kps, lambda);
	Point to_p = cur_t.getGravPoint(to_kps, lambda);
	cimg_forC(dest, c) {
		dest(x, y, 0, c) = (1.0 - scale) * from.linear_atXYZC(from_p.real(), from_p.imag(), 0, c) \
			+ scale * to.linear_atXYZC(to_p.real(), to_p.imag(), 0, c);
	}
}
