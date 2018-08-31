#include "jsontopoints.h"

jsontopoints::jsontopoints(const string & dir)
{
	// ���ļ�����
	ifstream ifs(dir);
	string content;
	getline(ifs, content);
	ifs.close();
	// ��ζ������
	convert(content);
}

void jsontopoints::convert(const string & js) {
	int state = 0;
	int x, y;
	int x1, y1, x2, y2;
	for (auto ch : js) {
		if (state == 0 && ch == '[') {
			state = 1;
		}
		else if (state == 1 && ch == '{') {
			state = 2;
		}
		else if (state == 2 && ch == '{') {
			state = 3; // ׼����
		}
		else if (state == 3 && ch == '{') {
			state = 4; // y ׼��
		}
		else if (state == 4 && ch >= '0' && ch <= '9') {
			y = ch - '0';
			state = 5; // ���ڶ�y
		}
		else if (state == 5) {
			if (ch >= '0' && ch <= '9') {
				y = y * 10 + ch - '0';
			}
			else {
				state = 6; // x ׼��
			}
		}
		else if (state == 6 && ch >= '0' && ch <= '9') {
			x = ch - '0';
			state = 7; // ���ڶ�x
		}
		else if (state == 7) {
			if (ch >= '0' && ch <= '9') {
				x = x * 10 + ch - '0';
			}
			else {
				// ����
				keypoint.push_back(Point(x, y));
				state = 3; // ׼������һ��
			}
		}
		else if (state == 3 && ch == '}') {
			state = 8; // ׼��������
		}
		else if (state == 8 && ch == '{') {
			state = 9; // w ׼��
		}
		else if (state == 9 && ch >= '0' && ch <= '9') {
			w = ch - '0';
			state = 10; // ���ڶ�w
		}
		else if (state == 10) {
			if (ch >= '0' && ch <= '9') {
				w = w * 10 + ch - '0';
			}
			else {
				state = 11; // y1 ׼��
			}
		}
		else if (state == 11 && ch >= '0' && ch <= '9') {
			y1 = ch - '0';
			state = 12; // ���ڶ�y1
		}
		else if (state == 12) {
			if (ch >= '0' && ch <= '9') {
				y1 = y1 * 10 + ch - '0';
			}
			else {
				state = 13; // x1 ׼��
			}
		}
		else if (state == 13 && ch >= '0' && ch <= '9') {
			x1 = ch - '0';
			state = 14; // ���ڶ�x1
		}
		else if (state == 14) {
			if (ch >= '0' && ch <= '9') {
				x1 = x1 * 10 + ch - '0';
			}
			else {
				state = 15; // h ׼��
			}
		}
		else if (state == 15 && ch >= '0' && ch <= '9') {
			h = ch - '0';
			state = 16; // ���ڶ�h
		}
		else if (state == 16) {
			if (ch >= '0' && ch <= '9') {
				h = h * 10 + ch - '0';
			}
			else {
				state = 17; // ����
			}
		}
	}
	x1--, y1--;
	w += 3, h += 3;
	// ��(x2, y2)
	x2 = x1 + w - 1;
	y2 = y1 + h - 1;
	// ����Ҳ�ӵ��ؼ��㼯��
	keypoint.push_back(Point(x1, y1));
	keypoint.push_back(Point(x1, y2));
	keypoint.push_back(Point(x2, y1));
	keypoint.push_back(Point(x2, y2));
}