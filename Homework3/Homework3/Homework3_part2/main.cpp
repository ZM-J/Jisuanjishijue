#include <iostream>
#include <cmath>
#include <vector>
#include "CImg.h"
#include "canny.h"
#include "hough.h"
#include "cluster.h"
#include "result.h"

using namespace std;
using namespace cimg_library;

int main()
{
	for (char i = '1'; i <= '6'; i++) {
		string inFolderPath = "Dataset2/Dataset/";
		string filePath = "IMG_";
		filePath += i;
		filePath += ".bmp"; //Filepath of input image
		canny cny(inFolderPath + filePath);
		hough h(cny.thres);
		cluster c(h.vote);
		result r(cny.img, c.center, h.xstep, h.ystep, h.rstep);
		string outFolderPath = "Dataset2/myresult/";
		string outPath = outFolderPath + filePath;
		r.res.save(outPath.c_str());
	}

    return 0;
}

