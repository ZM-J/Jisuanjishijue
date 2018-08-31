#include "cluster.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <map>
using namespace std;

cluster::cluster(const Mat & img)
{
	w = img.width(), h = img.height(), d = img.depth();
	// ɸѡ���ϸ�Ʊ���ĵ�
	binarize(img);
	// ����
	// �ĳ���Kruskal�㷨
	kruskal();

	p.clear();
	group.clear();
	weight.clear();
}

void cluster::binarize(const Mat & img)
{
	// const int NumImportant = 888;
	// PixelType Thresh = img.kth_smallest(w*h*d-NumImportant);
	PixelType Thresh = cimg::PI * 1.30; // 1.30 ��䣬���ر�����
	//ofstream f("points.txt");
	cimg_forXYZ(img, x, y, z) {
		if (img(x, y, z) > Thresh) {
			p.push_back(point3d(x, y, z));
			weight.push_back(img(x, y, z));
			//f << x << ' ' << y << ' ' << z << endl;
		}
	}
	cout << "Num of P: " << p.size() << endl;
	//f.close();
}

void cluster::kruskal()
{
	// �����֮���������ֵ
	double rThresh = 2.33; // 2.33
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
	unordered_map<int, tuple<int, double, point3d>> cent;
	for (int i = 0; i < n_point; i++) {
		int gi = getGroup(i);
		get<0>(cent[gi])++; // ͳ�Ƹ���++
		if (weight[i] > get<1>(cent[gi])) {
			get<1>(cent[gi]) = weight[i];
			get<2>(cent[gi]) = p[i];
		}
	}
	// ���յ����Ĵ�С��������ֻȡ�ϴ�ľ���
	multimap<int, point3d> rnk;
	int LeastPointSize = 15;
	for (auto p : cent) {
		cout << get<0>(p.second) << endl;
		if (get<0>(p.second) >= LeastPointSize) {
			center.push_back(get<2>(p.second));
		}
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