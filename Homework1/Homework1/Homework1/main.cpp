#include "CImg.h"
using namespace cimg_library;

int main() {
	// 1.	读取1.bmp文件，并用CImg.display() 显示
	CImg<unsigned char> image("1.bmp");
	image.display();
	// 2.	把1.bmp文件的白色区域变成红色，黑色区域变成绿色
	// 颜色常量定义
	const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
	const unsigned char white[] = { 255,255,255 }, black[] = { 0,0,0 }, yellow[] = {255, 255, 0};
	// 开搞
	cimg_forXYZ(image, x, y, z) {
		bool isWhite = true, isBlack = true;
		cimg_forC(image, c) {
			int val = image(x, y, z, c);
			if (val != white[c]) {
				isWhite = false;
			}
			if (val != black[c]) {
				isBlack = false;
			}
		}
		if (isWhite) {
			cimg_forC(image, c) {
				image(x, y, z, c) = red[c];
			}
		}
		if (isBlack) {
			cimg_forC(image, c) {
				image(x, y, z, c) = green[c];
			}
		}
	}
	image.display();
	// 3.	在图上绘制一个圆形区域，圆心坐标(50, 50)，半径为30，填充颜色为蓝色。
	image.draw_circle(50, 50, 30, blue);
	image.display();
	// 4.	在图上绘制一个圆形区域，圆心坐标(50,50)，半径为3，填充颜色为黄色。
	image.draw_circle(50, 50, 3, yellow);
	image.display();
	return 0;
}