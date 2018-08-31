#include <iostream>
#include <cmath>
#include <vector>
#include <sstream>
#include <fstream>
#include "CImg.h"
#include "canny.h"
#include "hough.h"
#include "cluster.h"
#include "result.h"
#include "warping.h"
#include "segment.h"
#include "recognize.h"
#include "table.h"

using namespace std;
using namespace cimg_library;

int main()
{
	string inFolderPath = "Dataset/Dataset/";
	string fileName = "15331029"; //Filepath of input image
	canny cny(inFolderPath + fileName + ".bmp");
	hough h(cny.thres);
	cluster c(h.vote, 4);
	result r(cny.img, c.center, h.rhostep, h.thetastep);
	warping w(cny.img, r.jd);
    segment seg(w.paper.get_RGBtoYUV().get_channel(0));
    
#ifdef DEBUG
    // 输出每个数字到文件夹里面看一看瞧一瞧
    for (int i = 0; i < 9; i++) {
        stringstream ss;
        ss << "Dataset/myresult/" << i + 1 << '/';
        for (int j = 0; j < seg.res[i].size(); j++) {
            stringstream tt;
            tt << ss.str() << j + 1 << ".bmp";
            Mat ex = seg.res[i][j].first * 255.0;
            ex.save(tt.str().c_str());
        }
    }
#endif
    // 魔改
    /*
    seg.res.clear();
    vector<pair<Mat, Point>> gao;
    for (int i = 1; i <= 100; i++) {
        stringstream ss;
        ss << "Dataset/testImg/" << i << ".bmp";
        Point p;
        Mat timg(ss.str().c_str());
        timg.RGBtoYUV().channel(0);
        gao.push_back(make_pair(timg, p));
    }
    seg.res.push_back(gao);
    */


    recognize rec(seg);
   
#ifdef DEBUG
    for (auto i : rec.result) {
        cout << i << endl;
    }
#endif
    
	string outFolderPath = "Dataset/myresult/";
	string outName = outFolderPath + fileName;

    w.paper.save((outName + ".bmp").c_str());
    table tb;
    for (int i = 0; i < rec.result.size(); i += 3) {
        string xuehao = i < rec.result.size() ? rec.result[i] : "x";
        string shoujihao = i + 1 < rec.result.size() ? rec.result[i + 1] : "x";
        string shenfenzheng = i + 2 < rec.result.size() ? rec.result[i + 2] : "x";
        tb.addLine(xuehao, w.zuoshang, w.youshang, w.youxia, w.zuoxia, shoujihao, shenfenzheng);
    }
    tb.tofile(outName + ".xlsx");
    
    return 0;
}

