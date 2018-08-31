#include "cluster.h"
#include <iostream>
#include <fstream>
#include <iomanip>
using namespace std;

cluster::cluster(const Mat & img, int n_cluster)
{
	w = img.width(), h = img.height();
	// ɸѡ���ϸ�Ʊ���ĵ�
	binarize(img);
	// shift
	shift();
	// ���ࣨ�����ȡkmeans�㷨��
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
	const int NumImportant = 97; // #97��ι��ι��ι
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
	// ȡ��ʼֵ��
	// ������kmeans++�ķ�����
	kmeans_pick(n_cluster);
	// ��ʼkmeansѭ������ÿ�������������Լ���ֵ�����ģ�
	int changed;
	do {
		// ���¾����ֵ�����ģ�
		for (int i = 0; i < n_cluster; i++) {
			center[i] = Point(0, 0);
		}
		vector<int> cnt(n_cluster);
		// ͳ�ƺ�
		for (int i = 0; i < p.size(); i++) {
			int nowg = group[i];
			cnt[nowg]++;
			center[nowg] += p[i];
		}
		// ���Ե���
		for (int i = 0; i < n_cluster; i++) {
			center[i] /= cnt[i];
		}
		// ͨ�����ӽ���ĸı������жϾ����㷨�Ƿ��Ѿ�����
		changed = 0;
		// ����ÿ�������������
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
		// �����
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
	// ��һ�������ĵ�p[nowi]�Լ��������������������������
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
		// ��һ��
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
