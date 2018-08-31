#include "result.h"
#include <iostream>
using namespace std;

result::result(const Mat & img, const vector<Point> & params, double rhostep, double thetastep)
{
	w = img.width();
	h = img.height();
	vector<double> rhos, thetas;
	for (auto param : params) {
		double rho = param.real() * rhostep;
		double theta = param.imag() * thetastep;
		// scaling back ��Ҫ�����rho�Ĳ����任
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

	}
	findJD(rhos, thetas);
}

void result::findJD(const vector<double>& rhos, const vector<double>& thetas)
{
	int l_params = rhos.size();
	double eps = 1e-3;
	for (int i = 0; i < l_params; i++)
		for (int j = 0; j < i; j++) {
			// ��Cramer����⽻��
			double x = rhos[i] * sin(thetas[j]) - rhos[j] * sin(thetas[i]);
			double y = rhos[j] * cos(thetas[i]) - rhos[i] * cos(thetas[j]);
			double fm = sin(thetas[j] - thetas[i]);
			// ��ĸ̫С�Ļ������㽻��
			if (fabs(fm) > eps) {
				// �ǵ�Ҫƽ�Ʋ��ǽ��
				x = x / fm + w / 2;
				y = y / fm + h / 2;
				if (x >= 0 && x < w && y >= 0 && y < h) {
					jd.push_back(Point(x, y));
				}
			}
		}
}