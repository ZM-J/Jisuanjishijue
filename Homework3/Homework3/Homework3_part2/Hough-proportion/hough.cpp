#include "hough.h"
#include <iostream>
#include <queue>

using namespace cimg;
using namespace std;

hough::hough(const Mat & edge)
{
	w = edge.width();
	h = edge.height();
	hEdge = edge;
	// 先利用闭操作来连断边
	// hEdge.dilate(3).erode(3);
	// 不采用，因为连断边的同时会把毛刺连起来，造成干扰
	// 先要删掉短小的边，只留下大边
	DeleteSmallComponent();
	// 利用闭操作
	// hEdge.dilate(3).erode(3);
	// 再细化
	// Thinning();
#ifdef DEBUG
		hEdge.display();
#endif
	// 投票矩阵的宽高深
	unsigned int votew = 256;
	unsigned int voteh = 256;
	unsigned int voted = 362;
	vote = Mat(votew, voteh, voted, 1, 0); // 投票矩阵，其中宽高深是调整效果的要点。
	double sigma = 2.5; // Hough Transform smoothing
	// x, y, r的最大值
	double xmax = w;
	double ymax = h;
	double rmax = sqrt(w*w + h*h);
	// x, y, r的步数
	xstep = xmax / votew;
	ystep = ymax / voteh;
	rstep = rmax / voted;
	// 投票机制
	
	cimg_forXY(hEdge, x, y) {
		if (hEdge(x, y) == 1) { //如果是边界的话
			// 扫所有的数组来找到满足的条件
			for (int i = 0; i < votew; i++) {
				double x0 = i * xstep;
				for (int j = 0; j < voteh; j++) {
					double y0 = j * ystep;
					double r = sqrt((x - x0)*(x - x0) + (y - y0)*(y - y0));
					int k = (int)(r / rstep + 0.5);
					vote(i, j, k) += 1;
				}
			}
		}
	}
	
	vote.blur(sigma);
#ifdef DEBUG
	Mat vote2(vote);
	cimg_forXYZ(vote2, x, y, z) {
		vote2(x, y, z) = log(1 + vote2(x, y, z));
	}
	vote2.display();
#endif

	hEdge.clear();
}

void hough::DeleteSmallComponent()
{
	// 规定连在一起的像素个数小于多少就要被删掉
	int Thresh = min(w, h) / 8;
	Mat vis(w, h, 1, 1, 0);
	const int dx[] = { -1,-1,-1, 0, 0, 1, 1, 1 };
	const int dy[] = { -1, 0, 1,-1, 1,-1, 0, 1 };

	for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++)
			if (vis(x, y) == 0) {
				vis(x, y) = 1;
				if (hEdge(x, y) == 1) {
					queue<int> qx, qy, recx, recy;
					qx.push(x), qy.push(y);
					recx.push(x), recy.push(y);
					// 扩展点
					while (!qx.empty()) {
						int nowx = qx.front(), nowy = qy.front();
						qx.pop(), qy.pop();
						for (int dir = 0; dir < 8; dir++) {
							int nxtx = nowx + dx[dir];
							int nxty = nowy + dy[dir];
							if (nxtx >= 0 && nxtx < w && nxty >= 0 && nxty < h) {
								if (vis(nxtx, nxty) == 0 && hEdge(nxtx, nxty) == 1) {
									qx.push(nxtx), qy.push(nxty);
									recx.push(nxtx), recy.push(nxty);
									vis(nxtx, nxty) = 1;
								}
							}
						}
					}
					// cout << x << ' ' << y << ' ' << recx.size() << endl;

					// 看点的数量是否太小
					if (recx.size() < Thresh) {
						while (!recx.empty()) {
							int nowx = recx.front(), nowy = recy.front();
							recx.pop(), recy.pop();
							hEdge(nowx, nowy) = 0;
						}
					}
				}
			}
}

void hough::Thinning() {
	// 细化。边缘宽度 = 1
	// 不然可能导致之后投票的权重不一致
	int curDeleted;
	do {
		curDeleted = 0;
		for (int x = 1; x < w - 1; x++)
			for (int y = 1; y < h - 1; y++) {
				bool p = hEdge(x, y);
				bool n = hEdge(x - 1, y), s = hEdge(x + 1, y), w = hEdge(x, y - 1), e = hEdge(x, y + 1);
				bool nw = hEdge(x - 1, y - 1), sw = hEdge(x + 1, y - 1), ne = hEdge(x - 1, y + 1), se = hEdge(x + 1, y + 1);
				// 每次细化分4步（不去除只有一个邻点）
				// 八连通下北向边界点（n=0, p=1）可删除条件，即没有上邻点
				bool p1 = w && !s && e || !w && nw || ne && !e || !e && se && !s || !s && sw && !w;
				// 八连通下南向边界点（s=0, p=1）可删除条件，即没有下邻点
				bool p2 = w && !n && e || !w && sw || se && !e || !e && ne && !n || !n && nw && !w;
				// 八连通下西向边界点（w=0, p=1）可删除条件，即没有左邻点
				bool p3 = n && !e && s || !s && sw || nw && !n || !e && ne && !n || !s && se && !e;
				// 八连通下东向边界点（e=0, p=1）可删除条件，即没有右邻点
				bool p4 = n && !w && s || !s && se || ne && !n || !e && nw && !n || !s && sw && !w;
				if (p) {
					// 孤立点或孤立线均不可去除
					if (n + s + w + e + nw + sw + ne + se <= 1) {
						continue;
					}
					// 判断删除条件
					if (!n && !p1 || !s && !p2 || !w && !p3 || !e && !p4) {
						hEdge(x, y) = 0;
						curDeleted++;
					}
				}
			}
	} while (curDeleted > 0);
}