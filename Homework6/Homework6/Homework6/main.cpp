#include "CImg.h"
#include "sift.h"
#include "siftmatch.h"
#include "ransac.h"
#include "mosaic.h"
#include "spherical.h"
#include "cylindral.h"
#include <iostream>
#include <sstream>
using namespace cimg_library;
using namespace std;

/*
int main(int argc, const char * argv[]) {
    CImg<unsigned int> res("Dataset/1/1.bmp");
    for (int i = 2; i <= 4; i++) {
        stringstream ss;
        ss << "Dataset/1/" << i << ".bmp";
        CImg<unsigned int> nxt(ss.str().c_str());
        sift sfres(res);
        sift sfnxt(nxt);
        siftmatch matching(sfres, sfnxt);
        ransac rs(res, nxt, matching);
        mosaic ms(res, nxt, rs);
        res = ms.res;
    }
    res.save("Dataset/1/stitch.bmp");

    return 0;
}
*/

int main(int argc, const char * argv[]) {
    vector<CImg<unsigned int>> res;
    for (int i = 1; i <= 18; i++) {
        stringstream ss;
        ss << "Dataset/2/" << i << ".bmp";
        CImg<unsigned int> tmp(ss.str().c_str());
        // spherical sph(tmp);
        cylindral cyl(tmp);
        // 不限制大小
        // res.push_back(cyl.res);
        // 限制大小
        int resH = 512;
        int resW = tmp.width() * 512 / tmp.height();
        CImg<unsigned int> tt = cyl.res.get_resize(resW, resH);
        // tt.display();
        res.push_back(tt);
    }
    while (res.size() > 1) {
        vector<CImg<unsigned int>> tempres;
        for (int i = 0; i + 1 < res.size(); i += 2) {
            sift sf1(res[i]);
            sift sf2(res[i + 1]);
            siftmatch matching(sf1, sf2);
            ransac rs(res[i], res[i+1], matching);
            mosaic ms(res[i], res[i+1], rs);
            tempres.push_back(ms.res);
        }        
        res = tempres;
    }
    res[0].save("Dataset/2/stitch.bmp");
    // system("pause");
    return 0;
}