#include <iostream>
#include <cmath>
#include <vector>
#include "CImg.h"
#include "canny.h"
#include "hough.h"
#include "cluster.h"
#include "result.h"
#include "warping.h"

using namespace std;
using namespace cimg_library;

int main()
{
	for (char i = '1'; i <= '6'; i++) {
		string inFolderPath = "Dataset/Dataset/";
		string filePath = "IMG_";
		filePath += i;
		filePath += ".bmp"; //Filepath of input image
		canny cny(inFolderPath + filePath);
		hough h(cny.thres);
		cluster c(h.vote, 4);
		result r(cny.img, c.center, h.rhostep, h.thetastep);
		warping w(cny.img, r.jd);
		string outFolderPath = "Dataset/myresult/";
		string outPath = outFolderPath + filePath;
		w.paper.save(outPath.c_str());
	}
	/*
	Mat a(3, 3, 1, 1,
		2, 3, -5,
		1, -2, 1,
		3, 1, 3);
	Mat b(1, 3, 1, 1,
		3,
		0,
		7);
	b.solve(a);
	cout << b(1) << endl;
	*/
	/*Mat a(2, 2, 1, 1,
		1, 2,
		2, 4);
	Mat b(1, 2, 1, 1,
		3,
		6);
	b.solve(a).display();*/
	system("pause");
    return 0;
}

