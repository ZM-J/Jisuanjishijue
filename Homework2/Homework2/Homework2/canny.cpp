//
//  canny.cpp
//  Canny Edge Detector
//
//  Created by Hasan Akgün on 21/03/14.
//  Copyright (c) 2014 Hasan Akgün. All rights reserved.
//  Hasaki is SB 
//

#include <iostream>
#include <cmath>
#include <queue>
#include "CImg.h"
#include "canny.h"

using namespace std;
using namespace cimg_library;

canny::canny(string filename)
{
	img.load(filename.c_str());
	
	if (!img.data()) // Check for invalid input
	{
		cout << "Could not open or find the image" << endl;
	}
	else
	{
		w = img.width(), h = img.height();
		Mat filter = createGaussianFilter(3, 3, 1);
		// 灰度转换的另一个可能的写法：直接用toGrayScale
		// grayscaled = img.RGBtoYCbCr().channel(0);
		toGrayScale(); //Grayscale the image

		// 高斯模糊的另一个可能的写法：调用get_blur函数
		// gFiltered = grayscaled.get_blur(1);
		gFiltered = grayscaled.get_convolve(filter); //Gaussian Filter
		sobel(); //Sobel Filter

		// 选择阈值
		selectThresholds();

		nonMaxSupp(); //Non-Maxima Suppression
		threshold(lowThresh, highThresh); //Double Threshold and Finalize
		cout << lowThresh << ' ' << highThresh << endl;

		// 细化
		thinning();

		img.display("Original");
		grayscaled.display("GrayScaled");
		gFiltered.display("Gaussian Blur");
		sFiltered.display("Sobel Filtered");
		nonMaxSupped.display("Non-Maxima Supp.");
		thres.display("Double Threshold");
		thin.display("Thinning");
	}
}

void canny::toGrayScale()
{
	// img -> grayscaled
    grayscaled = Mat(w, h); //To one channel
	cimg_forXYZ(img, x, y, z) {
		PixelType b = img(x, y, z, 0);
		PixelType g = img(x, y, z, 1);
		PixelType r = img(x, y, z, 2);

		double newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
		grayscaled(x, y, z) = newValue;
	}
}

Mat canny::createGaussianFilter(int row, int column, double sigmaIn)
{
	Mat filter(row, column);

	double coordSum;
	double constant = 2.0 * sigmaIn * sigmaIn;

	// Sum is for normalization
	double sum = 0.0;

	for (int x = -row/2; x <= row/2; x++)
	{
		for (int y = -column/2; y <= column/2; y++)
		{
			coordSum = x*x + y*y;
			filter(x + row/2, y + column/2) = (exp(-(coordSum) / constant)) / (PI * constant);
			sum += filter(x + row/2, y + column/2);
		}
	}

	// Normalize the Filter
	cimg_forXY(filter, x, y) {
		filter(x, y) /= sum;
	}

	return filter;

}

void canny::sobel()
{
	// gFiltered -> (sFiltered, angles)

    // Sobel X Filter
	// 需要旋转180度以做卷积
	Mat sobelX(3, 3, 1, 1,
		1, 2, 1,
		0, 0, 0,
		-1, -2, -1);
    
    // Sobel Y Filter
	// 需要旋转180度以做卷积
	Mat sobelY(3, 3, 1, 1,
		1, 0, -1,
		2, 0, -2,
		1, 0, -1);

	Mat filteredX = gFiltered.get_convolve(sobelX), filteredY = gFiltered.get_convolve(sobelY);
	sFiltered = Mat(w, h), angles = Mat(w, h);
	PixelType max_sF = 0;
	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++) {
			PixelType fx = filteredX(i, j), fy = filteredY(i, j);
			sFiltered(i, j) = sqrt(fx * fx + fy * fy);
			max_sF = max(max_sF, sFiltered(i, j));
			angles(i, j) = atan2(fy, fx);
		}
	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
			sFiltered(i, j) /= max_sF;
}

void canny::selectThresholds() {
	// 自动选择阈值
	// 前提假设：PercentOfPixelsNotEdges与ThresholdRatio
	// 需要先得到sFiltered
	const double PercentOfPixelsNotEdges = 0.7777777; // clearlove4396
	const double ThresholdRatio = 0.4;
	vector<PixelType> v(w*h);
	int cnt = 0;
	cimg_forXY(sFiltered, x, y) {
		v[cnt++] = sFiltered(x, y);
	}
	sort(v.begin(), v.end());
	highThresh = v[(int)(PercentOfPixelsNotEdges * w * h)];
	lowThresh = ThresholdRatio * highThresh;
}

void canny::nonMaxSupp()
{
	// (sFiltered, angles) -> nonMaxSupped
	nonMaxSupped = sFiltered;
    for (int i=1; i < w-1; i++) {
        for (int j=1; j< h-1; j++) {
            float Tangent = angles(i,j);
            //Horizontal Edge
            if (((-PI/8 < Tangent) && (Tangent <= PI/8)) || (PI*7/8 < Tangent) || (Tangent <= -PI*7/8))
            {
                if ((sFiltered(i,j) < sFiltered(i,j+1)) || (sFiltered(i,j) < sFiltered(i,j-1)))
                    nonMaxSupped(i, j) = 0;
            }
            //Vertical Edge
            if (((-PI*5/8 < Tangent) && (Tangent <= -PI*3/8)) || ((PI*3/8 < Tangent) && (Tangent <= PI*5/8)))
            {
                if ((sFiltered(i,j) < sFiltered(i+1,j)) || (sFiltered(i,j) < sFiltered(i-1,j)))
                    nonMaxSupped(i, j) = 0;
            }
            
            //-45 Degree Edge
            if (((-PI*3/8 < Tangent) && (Tangent <= -PI/8)) || ((PI*5/8 < Tangent) && (Tangent <= PI*7/8)))
            {
                if ((sFiltered(i,j) < sFiltered(i-1,j+1)) || (sFiltered(i,j) < sFiltered(i+1,j-1)))
                    nonMaxSupped(i, j) = 0;
            }
            
            //45 Degree Edge
            if (((-PI*7/8 < Tangent) && (Tangent <= -PI*5/8)) || ((PI/8 < Tangent) && (Tangent <= PI*3/8)))
            {
                if ((sFiltered(i,j) < sFiltered(i+1,j+1)) || (sFiltered(i,j) < sFiltered(i-1,j-1)))
                    nonMaxSupped(i, j) = 0;
            }
        }
    }
}

void canny::threshold(double low, double high)
{
	// nonMaxSupped -> thres
	thres = Mat(w, h);
	Mat strong(w, h);
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			strong(i, j) = (nonMaxSupped(i, j) > high);
		}
	}
	// 处理中间的值
	// 连接。从强边8连通扩展
	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
			thres(i, j) = 0;
	const int di[] = {-1,-1,-1, 0, 0, 1, 1, 1};
	const int dj[] = {-1, 0, 1,-1, 1,-1, 0, 1};
	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
			if (thres(i, j) == 0 && strong(i, j) == 1) {
				queue<int> qi, qj;
				qi.push(i), qj.push(j);
				while (!qi.empty()) {
					int nowi = qi.front(), nowj = qj.front();
					qi.pop(), qj.pop();
					thres(nowi, nowj) = 1;
					for (int dir = 0; dir < 8; dir++) {
						int nxti = nowi + di[dir];
						int nxtj = nowj + dj[dir];
						if (nxti > 0 && nxti < w && nxtj > 0 && nxtj < h)
							if (nonMaxSupped(nxti, nxtj) > low && thres(nxti, nxtj) == 0) {
								qi.push(nxti), qj.push(nxtj);
								thres(nxti, nxtj) = 1;
							}
					}
				}
			}
}

void canny::thinning() {
	// thres -> thin
	// 细化。边缘宽度 = 1
	thin = thres;
	int curDeleted;
	do {
		curDeleted = 0;
		for (int x = 1; x < w-1; x++)
			for (int y = 1; y < h-1; y++) {
				bool p = thin(x, y);
				bool n = thin(x - 1, y), s = thin(x + 1, y), w = thin(x, y - 1), e = thin(x, y + 1);
				bool nw = thin(x - 1, y - 1), sw = thin(x + 1, y - 1), ne = thin(x - 1, y + 1), se = thin(x + 1, y + 1);
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
						thin(x, y) = 0;
						curDeleted++;
					}
				}
			}
	} while (curDeleted > 0);
}