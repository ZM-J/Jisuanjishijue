#include "cluster.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <map>
using namespace std;

cluster::cluster(const Mat & img, int n_cluster)
{
	w = img.width(), h = img.height();
	// ɸѡ���ϸ�Ʊ���ĵ�
	binarize(img);
	// shift
	shift();
	// ����
	// �ĳ���Kruskal�㷨
	kruskal(n_cluster);
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
}

void cluster::binarize(const Mat & img)
{
	const int NumImportant = 97; // #97��ι��ι��ι
	PixelType Thresh = img.kth_smallest(w*h-NumImportant);
	cimg_forXY(img, x, y) {
		if (img(x, y) > Thresh) {
			p.push_back(Point(x, y));
			weight.push_back(img(x, y));
		}
	}
}

void cluster::kruskal(int n_cluster)
{
	// �����֮���������ֵ
	double rThresh = 2.33;
	int n_point = p.size();
	// ��ʼ������
	group.resize(n_point);
	for (int i = 0; i < n_point; i++) {
		group[i] = i;
	}
	for (int i = 0; i < n_point; i++)
		for (int j = 0; j < i; j++) {
			int gi = getGroup(i), gj = getGroup(j);
			if (gi != gj && abs(p[i] - p[j]) < rThresh) {
				group[gi] = gj;
			}
		}
	unordered_map<int, tuple<int, double, Point>> cent;
	for (int i = 0; i < n_point; i++) {
		int gi = getGroup(i);
		get<0>(cent[gi])++; // ͳ�Ƹ���++
		if (weight[i] > get<1>(cent[gi])) {
			get<1>(cent[gi]) = weight[i];
			get<2>(cent[gi]) = p[i];
		}
	}
	// ���յ����Ĵ�С��������ֻȡǰk��ľ���
	multimap<int, Point> rnk;
	int cnt;
	for (auto p : cent) {
		rnk.insert(make_pair(get<1>(p.second), get<2>(p.second)));
	}
	auto it = rnk.rbegin();
	// ȡǰk�����Ϊ�������ģ����)
	for (cnt = 1; cnt <= 4 && it != rnk.rend(); cnt++, it++) {
		center.push_back(it->second);
	}
}

int cluster::getGroup(int id)
{
	if (group[id] == id) {
		return id;
	}
	else {
		return group[id] = getGroup(group[id]);
	}
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
