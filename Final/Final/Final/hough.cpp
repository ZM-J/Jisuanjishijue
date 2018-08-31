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
	// ��Ҫɾ����С�ıߣ�ֻ���´��
	DeleteSmallComponent();
	// ��ɾ�����߿��н��ı�
	DeleteBorderComponent();
	// �ٽ�������������һȦ�ı�
	// TODO
#ifdef DEBUG
		hEdge.display();
#endif
	// ͶƱ����Ŀ��
	unsigned int votew = 768;
	unsigned int voteh = 512;
	vote = Mat(votew, voteh, 1, 1, 0); // ͶƱ�������п���ǵ���Ч����Ҫ�㡣
	double sigma = 0.5; // Hough Transform smoothing
	// rho, theta�����ֵ
	double rhomax = sqrt(w*w + h*h) / 2;
	double thetamax = 2 * PI;
	// rho, theta�Ĳ���
	rhostep = rhomax / votew;
	thetastep = thetamax / voteh;
	// ͶƱ����
	
	cimg_forXY(hEdge, x, y) {
		if (hEdge(x, y) == 1) { //����Ǳ߽�Ļ�
			// ���任ʹ������ԭ����ͼ��������
			double coordx = x - w / 2.0;
			double coordy = y - h / 2.0;
			// ɨ���е��������ҵ����������
			for (int i_th = 0; i_th < voteh / 2.0; i_th++) {
				double theta = i_th * thetastep;
				int th = i_th;
				double rho = coordx * cos(theta) + coordy * sin(theta);
				// ֻȡrho�Ǹ�������
				if (rho < 0) {
					th += voteh / 2.0;
					rho = -rho;
				}
				int r = (unsigned int)(rho / rhostep + 0.5);
				vote(r, th) += 1;
			}
			//cout << coordx << ' ' << coordy << endl;
			//vote.display();
			//return;
		}
	}
	
	vote.blur(sigma);
#ifdef DEBUG
	Mat vote2(vote);
	cimg_forXY(vote2, x, y) {
		vote2(x, y) = log(1 + vote2(x, y));
	}
	vote2.display();
#endif

	hEdge.clear();
}

void hough::DeleteSmallComponent()
{
	// �涨����һ������ظ���С�ڶ��پ�Ҫ��ɾ��
	int Thresh = min(w, h) / 4;
	Mat vis(w, h, 1, 1, 0);
	const int dx[] = { -1,-1,-1, 0, 0, 1, 1, 1 };
	const int dy[] = { -1, 0, 1,-1, 1,-1, 0, 1 };

	/*for (int x = 0; x < w; x++)
		for (int y = 0; y < h; y++)
			if (vis(x, y) == 0) {
				vis(x, y) = 1;
				if (hEdge(x, y) == 1) {
					queue<int> qx, qy;
					int tot = 1;
					qx.push(x), qy.push(y);
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
									tot++;
									vis(nxtx, nxty) = 1;
								}
							}
						}
					}
					Thresh = max(Thresh, tot);
				}
			}

	vis = Mat(w, h, 1, 1, 0);*/

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

void hough::DeleteBorderComponent()
{
	for (int x = 0; x < w; x++)
		for (int y = 0; y < BorderWidth; y++)
			DeleteComponent(x, y);
	for (int x = 0; x < w; x++)
		for (int y = h - BorderWidth; y < h; y++)
			DeleteComponent(x, y);
	for (int x = 0; x < BorderWidth; x++)
		for (int y = 0; y < h; y++)
			DeleteComponent(x, y);
	for (int x = w - BorderWidth; x < w; x++)
		for (int y = 0; y < h; y++)
			DeleteComponent(x, y);
}

void hough::DeleteComponent(int x, int y) // �ĳ�SearchConnect
{
	const int dx[] = { -1,-1,-1, 0, 0, 1, 1, 1 };
	const int dy[] = { -1, 0, 1,-1, 1,-1, 0, 1 };
	if (hEdge(x, y) == 1) {
		queue<int> qx, qy;
		qx.push(x), qy.push(y);
		hEdge(x, y) = 0;
		// ��չ��
		while (!qx.empty()) {
			int nowx = qx.front(), nowy = qy.front();
			qx.pop(), qy.pop();
			for (int dir = 0; dir < 8; dir++) {
				int nxtx = nowx + dx[dir];
				int nxty = nowy + dy[dir];
				if (nxtx >= 0 && nxtx < w && nxty >= 0 && nxty < h) {
					if (hEdge(nxtx, nxty) == 1) {
						qx.push(nxtx), qy.push(nxty);
						hEdge(nxtx, nxty) = 0;
					}
				}
			}
		}
	}
}
