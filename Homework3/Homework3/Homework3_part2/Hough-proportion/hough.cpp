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
	// �����ñղ��������ϱ�
	// hEdge.dilate(3).erode(3);
	// �����ã���Ϊ���ϱߵ�ͬʱ���ë������������ɸ���
	// ��Ҫɾ����С�ıߣ�ֻ���´��
	DeleteSmallComponent();
	// ���ñղ���
	// hEdge.dilate(3).erode(3);
	// ��ϸ��
	// Thinning();
#ifdef DEBUG
		hEdge.display();
#endif
	// ͶƱ����Ŀ����
	unsigned int votew = 256;
	unsigned int voteh = 256;
	unsigned int voted = 362;
	vote = Mat(votew, voteh, voted, 1, 0); // ͶƱ�������п�����ǵ���Ч����Ҫ�㡣
	double sigma = 2.5; // Hough Transform smoothing
	// x, y, r�����ֵ
	double xmax = w;
	double ymax = h;
	double rmax = sqrt(w*w + h*h);
	// x, y, r�Ĳ���
	xstep = xmax / votew;
	ystep = ymax / voteh;
	rstep = rmax / voted;
	// ͶƱ����
	
	cimg_forXY(hEdge, x, y) {
		if (hEdge(x, y) == 1) { //����Ǳ߽�Ļ�
			// ɨ���е��������ҵ����������
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
	// �涨����һ������ظ���С�ڶ��پ�Ҫ��ɾ��
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
					// ��չ��
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

					// ����������Ƿ�̫С
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
	// ϸ������Ե��� = 1
	// ��Ȼ���ܵ���֮��ͶƱ��Ȩ�ز�һ��
	int curDeleted;
	do {
		curDeleted = 0;
		for (int x = 1; x < w - 1; x++)
			for (int y = 1; y < h - 1; y++) {
				bool p = hEdge(x, y);
				bool n = hEdge(x - 1, y), s = hEdge(x + 1, y), w = hEdge(x, y - 1), e = hEdge(x, y + 1);
				bool nw = hEdge(x - 1, y - 1), sw = hEdge(x + 1, y - 1), ne = hEdge(x - 1, y + 1), se = hEdge(x + 1, y + 1);
				// ÿ��ϸ����4������ȥ��ֻ��һ���ڵ㣩
				// ����ͨ�±���߽�㣨n=0, p=1����ɾ����������û�����ڵ�
				bool p1 = w && !s && e || !w && nw || ne && !e || !e && se && !s || !s && sw && !w;
				// ����ͨ������߽�㣨s=0, p=1����ɾ����������û�����ڵ�
				bool p2 = w && !n && e || !w && sw || se && !e || !e && ne && !n || !n && nw && !w;
				// ����ͨ������߽�㣨w=0, p=1����ɾ����������û�����ڵ�
				bool p3 = n && !e && s || !s && sw || nw && !n || !e && ne && !n || !s && se && !e;
				// ����ͨ�¶���߽�㣨e=0, p=1����ɾ����������û�����ڵ�
				bool p4 = n && !w && s || !s && se || ne && !n || !e && nw && !n || !s && sw && !w;
				if (p) {
					// �����������߾�����ȥ��
					if (n + s + w + e + nw + sw + ne + se <= 1) {
						continue;
					}
					// �ж�ɾ������
					if (!n && !p1 || !s && !p2 || !w && !p3 || !e && !p4) {
						hEdge(x, y) = 0;
						curDeleted++;
					}
				}
			}
	} while (curDeleted > 0);
}