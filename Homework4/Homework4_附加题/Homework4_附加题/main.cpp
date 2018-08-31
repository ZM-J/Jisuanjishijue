#include "utility.h"
#include "jsontopoints.h"
#include "selroi.h"
#include "sjpf.h"
#include "morphing.h"

int main()
{
	string dir = "Dataset/4/";
	CImg<double> begimg((dir + "begin.bmp").c_str());
	CImg<double> endimg((dir + "end.bmp").c_str());
	jsontopoints begjp(dir + "begin.json");
	jsontopoints endjp(dir + "end.json");
	int roiw = min(begjp.w, endjp.w), roih = min(begjp.h, endjp.h);
	selroi begROI(begimg, begjp.keypoint, roiw, roih);
	selroi endROI(endimg, endjp.keypoint, roiw, roih);

	sjpf s(begjp.keypoint);

	int n_pic = 24;
	morphing m(begROI.roi, begROI.keypoint, endROI.roi, endROI.keypoint, s.tris, n_pic, dir + "Morphing/");

	/*double red[] = { 255,0,0 };
	for (auto t : s.tris) {
		for (int i = 0; i < 3; i++) {
			pair<int, int> e = t.getEdge(i);
			Point p1(begROI.keypoint[e.first]), p2(begROI.keypoint[e.second]);
			begROI.roi.draw_line(p1.real(), p1.imag(), p2.real(), p2.imag(), red);
		}
	}
	for (auto t : s.tris) {
		for (int i = 0; i < 3; i++) {
			pair<int, int> e = t.getEdge(i);
			Point p1(endROI.keypoint[e.first]), p2(endROI.keypoint[e.second]);
			endROI.roi.draw_line(p1.real(), p1.imag(), p2.real(), p2.imag(), red);
		}
	}
	endROI.roi.display();*/

	/*
	double Ax = 64, Ay = 152;
	double Bx = 252, By = 152;
	double Cx = 64, Cy = 340;
	double Dx = 166, Dy = 337;
	CImg<double> pd(4, 4, 1, 1,
		Ax, Ay, Ax*Ax + Ay*Ay, 1.0f,
		Bx, By, Bx*Bx + By*By, 1.0f,
		Cx, Cy, Cx*Cx + Cy*Cy, 1.0f,
		Dx, Dy, Dx*Dx + Dy*Dy, 1.0f);
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			cout << pd(i, j) << ' ';
		cout << endl;
	}
	cout << pd.det() << endl;
	system("pause");*/
	
	/*CImg<double> pd(5, 3, 1, 1,
		1.1, 1.2, 1.3, 1.4, 1.5,
		2.1, 2.2, 2.3, 2.4, 2.5,
		3.1, 3.2, 3.3, 3.4, 3.5);
	CImg<double> d(1, 5, 1, 1,
		1,
		1,
		1,
		1,
		1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 5; j++)
			cout << pd(i, j) << ' ';
		cout << endl;
	}
	(pd*d).display();
	system("pause");*/

	return 0;
}