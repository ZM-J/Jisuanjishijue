#include "cluster.h"
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

cluster::cluster(const Mat & img, int n_cluster)
{
	w = img.width(), h = img.height();
	// 筛选出较高票数的点
	binarize(img);
	// shift
	shift();
	// 聚类（这里采取kmeans算法）
	kmeans(n_cluster);
	// shiftback
	shiftback();
#ifdef DEBUG
	Mat showtime = img;
	PixelType clr[] = { showtime.max() };
	for (auto c : center) {
		showtime.draw_circle(c.real(), c.imag(), 5, clr, 1);
	}
	showtime.display();
#endif

	p.clear();
	group.clear();
	d.clear();
}

void cluster::binarize(const Mat & img)
{
	const int NumImportant = 97; // #97苏喂苏喂苏喂
	vector<PixelType> v(w*h);
	int cnt = 0;
	cimg_forXY(img, x, y) {
		v[cnt++] = img(x, y);
	}
	sort(v.begin(), v.end());
	double Thresh = v[w*h-NumImportant];
	cimg_forXY(img, x, y) {
		if (img(x, y) > Thresh) {
			p.push_back(Point(x, y));
		}
	}
}

void cluster::kmeans(int n_cluster)
{
	// 取初始值。
	// 我们用kmeans++的方法。
	kmeans_pick(n_cluster);
	// 开始kmeans循环更新每个点所属分类以及均值（中心）
	int changed;
	do {
		// 更新聚类均值（中心）
		for (int i = 0; i < n_cluster; i++) {
			center[i] = Point(0, 0);
		}
		vector<int> cnt(n_cluster);
		// 统计和
		for (int i = 0; i < p.size(); i++) {
			int nowg = group[i];
			cnt[nowg]++;
			center[nowg] += p[i];
		}
		// 除以点数
		for (int i = 0; i < n_cluster; i++) {
			center[i] /= cnt[i];
		}
		// 通过监视结果的改变量来判断聚类算法是否已经收敛
		changed = 0;
		// 更新每个点的所属分类
		for (int i = 0; i < p.size(); i++) {
			int mini = nearest(i, n_cluster);
			if (mini != group[i]) {
				changed++;
				group[i] = mini;
			}
		}
	} while (changed > (p.size() >> 10));
}

void cluster::kmeans_pick(int n_center)
{
	double sum;
	d.resize(p.size());
	center.resize(n_center);
	group.resize(p.size());

	cimg::srand(time(0));
	center[0] = p[rand() % p.size()];
	for (int n_cluster = 1; n_cluster < n_center; n_cluster++) {
		sum = 0;
		for (int i = 0; i < p.size(); i++) {
			nearest(i, n_cluster);
			sum += d[i];
		}
		// 随机化
		sum = cimg::rand(sum);
		for (int i = 0; i < p.size(); i++) {
			sum -= d[i];
			if (sum > 0) continue;
			center[n_cluster] = p[i];
			break;
		}
	}
	for (int i = 0; i < p.size(); i++) {
		group[i] = nearest(i, n_center);
	}
}

int cluster::nearest(int nowi, int n_cluster)
{
	// 对一个给定的点p[nowi]以及聚类数，给出距其最近的中心
	double mind;
	int mini;
	for (int i = 0; i < n_cluster; i++) {
		mind = HUGE_VAL;
		mini = group[nowi];
		for (int j = 0; j < n_cluster; j++) {
			double nowd = abs(center[j] - p[nowi]);
			if (mind > nowd) {
				mind = nowd;
				mini = j;
			}
		}
	}
	d[nowi] = mind;
	return mini;
}

void cluster::shift()
{
	b = 0;
	double eps = h/100.0;
	double step = h/23.3;
	bool gg = false;
	do {
		gg = false;
		for (auto nowp : p) {
			if (fabs(nowp.imag() - 0) < eps || fabs(nowp.imag() - (h - 1)) < eps) {
				gg = true;
				break;
			}
		}
		if (!gg) {
			break;
		}
		// 加一步
		b += step;
		for (auto &nowp : p) {
			nowp += Point(0, step);
			if (nowp.imag() >= h) {
				nowp -= Point(0, h);
			}
		}
		if (b > 1000 * step) {
			cout << "Failed" << endl;
			return;
		}
	} while (true);
}

void cluster::shiftback()
{
	for (auto &c : center) {
		c -= Point(0, b);
		if (c.imag() < 0) {
			c += Point(0, h);
		}
	}
}
