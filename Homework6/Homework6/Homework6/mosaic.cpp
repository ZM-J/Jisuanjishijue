#include "mosaic.h"

mosaic::mosaic(const CImg<double>& img1, const CImg<double>& img2, const ransac & rs)
{
    int w1 = img1.width(), h1 = img1.height();
    int w2 = img2.width(), h2 = img2.height();
    CImg<double> borders(4, 3, 1, 1,
        1.0, (double)w2 , 1.0, (double)w2,
        1.0, 1.0, (double)h2 , (double)h2,
        1.0, 1.0, 1.0, 1.0);
    CImg<double> invH = rs.H.get_invert();
    CImg<double> range_judge = invH * borders;
    // option 1: 黑的部分也考虑进来
    double minx = 0.0, miny = 0.0;
    double maxx = w1 - 1.0, maxy = h1 - 1.0;
    cimg_forX(range_judge, x) {
        range_judge(x, 0) = range_judge(x, 0) / range_judge(x, 2) - 1.0;
        range_judge(x, 1) = range_judge(x, 1) / range_judge(x, 2) - 1.0;
        minx = min(minx, range_judge(x, 0));
        maxx = max(maxx, range_judge(x, 0));
        miny = min(miny, range_judge(x, 1));
        maxy = max(maxy, range_judge(x, 1));
    }
    // option 2: 只考虑那些有像素的
    /*
    double minx = min(max(range_judge(0,0) / range_judge(0,2), range_judge(2,0) / range_judge(2,2)),1.0)-1.0;
    double maxx = max(min(range_judge(1,0) / range_judge(1,2), range_judge(3,0) / range_judge(3,2)),(double)w1)-1.0;
    double miny = max(max(range_judge(0,1) / range_judge(0,2), range_judge(1,1) / range_judge(1,2)), 1.0) - 1.0;
    double maxy = min(min(range_judge(2,1) / range_judge(2,2), range_judge(3,1) / range_judge(3,2)),(double)h1)-1.0;
    */
    CImg<double> temp_res((int)(maxx - minx + 1.0), (int)(maxy - miny + 1.0), 1, img1.spectrum(), invalid_pix);
    CImg<double> temp_res1(temp_res), temp_res2(temp_res);
    cimg_forXY(temp_res1, tempx, tempy) {
        int x = tempx + minx;
        int y = tempy + miny;
        if (x >= 0 && x < img1.width() && y >= 0 && y < img1.height()) {
            cimg_forC(img1, c) {
                temp_res1(tempx, tempy, 0, c) = img1(x, y, 0, c);
            }
        }
    }
    // invH.display();

    // 反向线性插值
    cimg_forXY(temp_res, x, y) {
        CImg<double> coord(1, 3, 1, 1,
            x + 1.0 + minx,
            y + 1.0 + miny,
            1.0);
        CImg<double> rawcoord = rs.H * coord;
        double rawx = rawcoord(0, 0) / rawcoord(0, 2) - 1.0;
        double rawy = rawcoord(0, 1) / rawcoord(0, 2) - 1.0;
        // 值合法
        if (rawx >= 0 && rawx <= w2 - 1 && rawy >= 0 && rawy <= h2 - 1) {
            cimg_forC(temp_res2, c) {
                temp_res2(x, y, 0, c) = img2.linear_atXYZC(rawx, rawy, 0, c);
            }
        }
    }
    // temp_res1.display();
    // temp_res2.display();
    mulres(temp_res1, temp_res2);
    // res.display();
    //temp_res1.save("m1.bmp");
    //temp_res2.save("m2.bmp");
}

void mosaic::mulres(const CImg<double>& img1, const CImg<double>& img2)
{
    /*res = img1;
    cimg_forXYC(img2, x, y, c) {
        if (img2(x, y, c) != invalid_pix) {
            res(x, y, c) = img2(x, y, c);
        }
    }*/
    CImg<double> mask(img1.width(), img1.height(), 1, 1);
    int minx = img1.width(), maxx = 0;
    cimg_forXY(mask, x, y) {
        if (img1(x, y) != invalid_pix && img2(x, y) != invalid_pix) {
            minx = min(minx, x);
            maxx = max(maxx, x);
        }
    }
    int leftcnt = 0;
    cimg_forY(mask, y) {
        if (img1(0, y) != invalid_pix && img2(0, y) == invalid_pix) {
            leftcnt++;
        }
        if (img1(0, y) == invalid_pix && img2(0, y) != invalid_pix) {
            leftcnt--;
        }
    }
    int midx = (minx + maxx) / 2;
    cimg_forXY(mask, x, y) {
        if (x < midx) {
            mask(x, y) = leftcnt > 0 ? 1.0 : 0.0;
        }
        else if (x > midx) {
            mask(x, y) = leftcnt > 0 ? 0.0 : 1.0;
        }
        else {
            mask(x, y) = 0.5;
        }
    }
    CImg<double> img1_toblend(img1), img2_toblend(img2);
    cimg_forXY(img1_toblend, x, y) {
        if (!(img1_toblend(x, y) != invalid_pix && img2_toblend(x, y) != invalid_pix)) {
            cimg_forC(img1_toblend, c) {
                img1_toblend(x, y, 0, c) = invalid_pix;
            }
            cimg_forC(img2_toblend, c) {
                img2_toblend(x, y, 0, c) = invalid_pix;
            }
        }
    }
    int numLayer = 7;
    // gaussian pyramid
    vector<CImg<double>> im1p = get_gaussian_pyramid(img1_toblend, numLayer);
    vector<CImg<double>> im2p = get_gaussian_pyramid(img2_toblend, numLayer);
    vector<CImg<double>> mp = get_gaussian_pyramid(mask, numLayer);
    // laplacian pyramid
    get_laplacian_pyramid(im1p);
    get_laplacian_pyramid(im2p);
    // result pyramid
    vector<CImg<double>> imp;
    for (int i = 0; i < im1p.size(); i++) {
        CImg<double> temp(im1p[i]);
        cimg_forXYC(temp, x, y, c) {
            temp(x, y, c) = im1p[i](x, y, c) * mp[i](x, y) + im2p[i](x, y, c) * (1.0 - mp[i](x, y));
        }
        imp.push_back(temp);
    }
    CImg<double> temp_res = imp.back();
    for (int i = imp.size() - 2; i >= 0; i--) {
        temp_res = imp[i] + temp_res.get_resize(imp[i]);
    }
    res = CImg<unsigned int>(temp_res.width(), temp_res.height(), 1, temp_res.spectrum());
    cimg_forXYC(temp_res, x, y, c) {
        temp_res(x, y, 0, c) = max(temp_res(x, y, 0, c), 0.0);
        temp_res(x, y, 0, c) = min(temp_res(x, y, 0, c), 255.0);
        res(x, y, 0, c) = (unsigned int)temp_res(x, y, 0, c);
    }
    // 加上img1与img2的独占部分
    cimg_forXY(res, x, y) {
        if (img1(x, y) != invalid_pix && img2(x, y) == invalid_pix) {
            cimg_forC(res, c) {
                res(x, y, 0, c) = img1(x, y, 0, c);
            }
        }
        else if (img1(x, y) == invalid_pix && img2(x, y) != invalid_pix) {
            cimg_forC(res, c) {
                res(x, y, 0, c) = img2(x, y, 0, c);
            }
        }
    }
}

vector<CImg<double>> mosaic::get_gaussian_pyramid(const CImg<double>& img, int numLayer)
{
    vector<CImg<double>> res;
    res.push_back(img);
    CImg<double> temp(img);
    for (int i = 0; i < numLayer; i++) {
        temp.blur(2.87f, true, true).resize_halfXY(); // temp.blur(2.87f, true, true).resize_halfXY()
        res.push_back(temp);
        //temp.display();
    }
    return res;
}

void mosaic::get_laplacian_pyramid(vector<CImg<double>>& pyr)
{
    for (int i = 0; i < pyr.size() - 1; i++) {
        CImg<double> g_k = pyr[i];
        CImg<double> g_k_plus_1 = pyr[i+1];
        CImg<double> lapl = g_k - g_k_plus_1.resize(g_k); // g_k_plus_1.resize(g_k).blur(2.87f, true, true)
        pyr[i] = lapl;
    }

}
