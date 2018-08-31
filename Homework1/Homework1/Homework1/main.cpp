#include "CImg.h"
using namespace cimg_library;

int main() {
	// 1.	��ȡ1.bmp�ļ�������CImg.display() ��ʾ
	CImg<unsigned char> image("1.bmp");
	image.display();
	// 2.	��1.bmp�ļ��İ�ɫ�����ɺ�ɫ����ɫ��������ɫ
	// ��ɫ��������
	const unsigned char red[] = { 255,0,0 }, green[] = { 0,255,0 }, blue[] = { 0,0,255 };
	const unsigned char white[] = { 255,255,255 }, black[] = { 0,0,0 }, yellow[] = {255, 255, 0};
	// ����
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
	// 3.	��ͼ�ϻ���һ��Բ������Բ������(50, 50)���뾶Ϊ30�������ɫΪ��ɫ��
	image.draw_circle(50, 50, 30, blue);
	image.display();
	// 4.	��ͼ�ϻ���һ��Բ������Բ������(50,50)���뾶Ϊ3�������ɫΪ��ɫ��
	image.draw_circle(50, 50, 3, yellow);
	image.display();
	return 0;
}