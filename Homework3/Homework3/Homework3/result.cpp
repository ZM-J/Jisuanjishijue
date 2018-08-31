#include "result.h"
#include <iostream>
using namespace std;

result::result(const Mat & img, const vector<Point> & params, double rhostep, double thetastep)
{
	w = img.width();
	h = img.height();
	const PixelType red[] = { 255,0,0 }, yellow[] = {255,255,0};
	res = img;
	double lambda = 5 * max(w, h);
	vector<double> rhos, thetas;
	for (auto param : params) {
		double rho = param.real() * rhostep;
		double theta = param.imag() * thetastep;
		// scaling back 主要是针对rho的参数变换
		if (w > h) {
			if (w > MAX_WH) {
				rho = rho * w / MAX_WH;
			}
		}
		else {
			if (h > MAX_WH) {
				rho = rho * h / MAX_WH;
			}
		}
		rhos.push_back(rho);
		thetas.push_back(theta);
		double x = w / 2 + rho*cos(theta);
		double y = h / 2 + rho*sin(theta);
		int x0 = x + lambda * sin(theta);
		int y0 = y - lambda * cos(theta);
		int x1 = x - lambda * sin(theta);
		int y1 = y + lambda * cos(theta);
		res.draw_line(x0, y0, x1, y1, red);
	}
	findJD(rhos, thetas);
	for (auto p : jd) {
		res.draw_circle(p.real(), p.imag(), 7, yellow, 1);
		cout << p.real() << ' ' << p.imag() << endl;
	}
#ifdef DEBUG
	res.display();
#endif
}

void result::findJD(const vector<double>& rhos, const vector<double>& thetas)
{
	int l_params = rhos.size();
	double eps = 1e-3;
	for (int i = 0; i < l_params; i++)
		for (int j = 0; j < i; j++) {
			// 用Cramer法则解交点
			double x = rhos[i] * sin(thetas[j]) - rhos[j] * sin(thetas[i]);
			double y = rhos[j] * cos(thetas[i]) - rhos[i] * cos(thetas[j]);
			double fm = sin(thetas[j] - thetas[i]);
			// 分母太小的话不能算交点
			if (fabs(fm) > eps) {
				// 记得要平移才是结果
				x = x / fm + w / 2;
				y = y / fm + h / 2;
				if (x >= 0 && x < w && y >= 0 && y < h) {
					jd.push_back(Point(x, y));
				}
			}
		}
	
}

void result::selectJD(const vector<double>& rhos, const vector<double>& thetas)
{
	int l_params = rhos.size();
	vector<Point> rawJD = jd;
	int l_raw = rawJD.size();
	vector<int> kind(l_raw), discarded(l_raw);
	double rThresh = 2.33;
	// 定义点在线的状态
	int inLine = 0, upLine = 1, downLine = -1;
	// 删掉不合乎矩形的交点
	for (int i = 0; i < l_params; i++) {
		int upCount = 0, downCount = 0;
		// 计算
		for (int j = 0; j < l_raw; j++) {
			double val = rawJD[j].real() * cos(thetas[i]) + rawJD[j].imag() * sin(thetas[i]) - rhos[i];
			if (fabs(val) < rThresh) {
				kind[j] = inLine;
			}
			else if (val > 0) {
				kind[j] = upLine;
				upCount++;
			}
			else {
				kind[j] = downLine;
				downCount++;
			}
		}
		// 标记要删的点
		if (upCount == 1) {
			for (int j = 0; j < l_raw; j++)
				if (kind[j] == upLine) {
					discarded[j] = 1;
				}
		}
		if (downCount == 1) {
			for (int j = 0; j < l_raw; j++)
				if (kind[j] == downLine) {
					discarded[j] = 1;
				}
		}
	}
	jd.clear();
	for (int i = 0; i < l_raw; i++)
		if (discarded[i] == 0) {
			jd.push_back(rawJD[i]);
		}
}
