#include <iostream>
#include <queue>
#include "canny.h"

using namespace std;
using namespace cimg_library;
using namespace cimg;

canny::canny(const string & filename)
{
	img.load(filename.c_str());
	
	if (!img.data()) // Check for invalid input
	{
		cout << "Could not open or find the image" << endl;
	}
	else
	{
		Mat res; // Resized Image
		Mat grayscaled; // Grayscale
		Mat gFiltered; // Gradient
		Mat sFiltered; // Sobel Filtered
		Mat angles; // Angle Map
		Mat nonMaxSupped; // Non-maxima supp.
		w = img.width(), h = img.height();
		// scaling放缩
		if (w > h) {
			if (w > MAX_WH) {
				h = 1.0 * h * MAX_WH / w;
				w = MAX_WH;
			}
		}
		else {
			if (h > MAX_WH) {
				w = 1.0 * w * MAX_WH / h;
				h = MAX_WH;
			}
		}
		res = img.get_resize(w, h);
		Mat filter = createGaussianFilter(3, 3, 1);
		// 灰度转换的另一个可能的写法：直接用toGrayScale
		toGrayScale(res, grayscaled);

		// 高斯模糊的另一个可能的写法：调用get_blur函数
		// gFiltered = grayscaled.get_blur(1);
		gFiltered = grayscaled.get_convolve(filter); //Gaussian Filter
		sobel(gFiltered, sFiltered, angles); //Sobel Filter

		// 选择阈值
		selectThresholds(sFiltered);

		nonMaxSupp(sFiltered, angles, nonMaxSupped); //Non-Maxima Suppression
		threshold(nonMaxSupped, lowThresh, highThresh, thres); //Double Threshold and Finalize
		cout << lowThresh << ' ' << highThresh << endl;
		// 细化
#ifdef DEBUG
		res.display("Resized image");
		thres.display("Edge");
#endif
	}
}

void canny::toGrayScale(const Mat & img, Mat & grayscaled)
{
	// img -> grayscaled
    grayscaled = Mat(w, h); //To one channel
	cimg_forXY(img, x, y) {
		PixelType b = img(x, y, 0, 0);
		PixelType g = img(x, y, 0, 1);
		PixelType r = img(x, y, 0, 2);

		double newValue = (r * 0.2126 + g * 0.7152 + b * 0.0722);
		grayscaled(x, y) = newValue;
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

void canny::sobel(const Mat &gFiltered, Mat &sFiltered, Mat &angles)
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
	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++) {
			PixelType fx = filteredX(i, j), fy = filteredY(i, j);
			sFiltered(i, j) = sqrt(fx * fx + fy * fy);
			angles(i, j) = atan2(fy, fx);
		}
	PixelType max_sF = sFiltered.max();
	for (int i = 0; i < w; i++)
		for (int j = 0; j < h; j++)
			sFiltered(i, j) /= max_sF;
}

void canny::selectThresholds(const Mat &sFiltered) {
	// 自动选择阈值
	// 前提假设：PercentOfPixelsNotEdges与ThresholdRatio
	// 需要先得到sFiltered
	const double PercentOfPixelsNotEdges = 0.997; // 人人有功练，尊师孙笑川
	const double ThresholdRatio = 0.4;
	highThresh = sFiltered.kth_smallest(PercentOfPixelsNotEdges * w * h);
	lowThresh = ThresholdRatio * highThresh;
}

void canny::nonMaxSupp(const Mat &sFiltered, const Mat &angles, Mat &nonMaxSupped)
{
	// (sFiltered, angles) -> nonMaxSupped
	nonMaxSupped = sFiltered;
    for (int i=1; i < w-1; i++) {
        for (int j=1; j< h-1; j++) {
            float Tangent = angles(i,j);
            //Horizontal Edge
            if (((-cimg::PI/8 < Tangent) && (Tangent <= cimg::PI/8)) || (cimg::PI*7/8 < Tangent) || (Tangent <= -cimg::PI*7/8))
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

void canny::threshold(const Mat &nonMaxSupped, double low, double high, Mat &thres)
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
						if (nxti >= 0 && nxti < w && nxtj >= 0 && nxtj < h)
							if (nonMaxSupped(nxti, nxtj) > low && thres(nxti, nxtj) == 0) {
								qi.push(nxti), qj.push(nxtj);
								thres(nxti, nxtj) = 1;
							}
					}
				}
			}
}