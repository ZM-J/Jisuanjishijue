#include "jsontopoints.h"

jsontopoints::jsontopoints(const string & dir)
{
	// 读文件内容
	ifstream ifs(dir);
	string content;
	getline(ifs, content);
	ifs.close();
	// 土味解析器
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
			state = 3; // 准备读
		}
		else if (state == 3 && ch == '{') {
			state = 4; // y 准备
		}
		else if (state == 4 && ch >= '0' && ch <= '9') {
			y = ch - '0';
			state = 5; // 正在读y
		}
		else if (state == 5) {
			if (ch >= '0' && ch <= '9') {
				y = y * 10 + ch - '0';
			}
			else {
				state = 6; // x 准备
			}
		}
		else if (state == 6 && ch >= '0' && ch <= '9') {
			x = ch - '0';
			state = 7; // 正在读x
		}
		else if (state == 7) {
			if (ch >= '0' && ch <= '9') {
				x = x * 10 + ch - '0';
			}
			else {
				// 后处理
				keypoint.push_back(Point(x, y));
				state = 3; // 准备读下一个
			}
		}
		else if (state == 3 && ch == '}') {
			state = 8; // 准备读矩形
		}
		else if (state == 8 && ch == '{') {
			state = 9; // w 准备
		}
		else if (state == 9 && ch >= '0' && ch <= '9') {
			w = ch - '0';
			state = 10; // 正在读w
		}
		else if (state == 10) {
			if (ch >= '0' && ch <= '9') {
				w = w * 10 + ch - '0';
			}
			else {
				state = 11; // y1 准备
			}
		}
		else if (state == 11 && ch >= '0' && ch <= '9') {
			y1 = ch - '0';
			state = 12; // 正在读y1
		}
		else if (state == 12) {
			if (ch >= '0' && ch <= '9') {
				y1 = y1 * 10 + ch - '0';
			}
			else {
				state = 13; // x1 准备
			}
		}
		else if (state == 13 && ch >= '0' && ch <= '9') {
			x1 = ch - '0';
			state = 14; // 正在读x1
		}
		else if (state == 14) {
			if (ch >= '0' && ch <= '9') {
				x1 = x1 * 10 + ch - '0';
			}
			else {
				state = 15; // h 准备
			}
		}
		else if (state == 15 && ch >= '0' && ch <= '9') {
			h = ch - '0';
			state = 16; // 正在读h
		}
		else if (state == 16) {
			if (ch >= '0' && ch <= '9') {
				h = h * 10 + ch - '0';
			}
			else {
				state = 17; // 结束
			}
		}
	}
	x1--, y1--;
	w += 3, h += 3;
	// 求(x2, y2)
	x2 = x1 + w - 1;
	y2 = y1 + h - 1;
	// 顶点也加到关键点集后
	keypoint.push_back(Point(x1, y1));
	keypoint.push_back(Point(x1, y2));
	keypoint.push_back(Point(x2, y1));
	keypoint.push_back(Point(x2, y2));
}