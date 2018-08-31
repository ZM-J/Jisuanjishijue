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
    double minx = 0.0, miny = 0.0;
    double maxx = w1-1.0, maxy = h1-1.0;
    cimg_forX(range_judge, x) {
        range_judge(x, 0) = range_judge(x, 0) / range_judge(x, 2) - 1.0;
        range_judge(x, 1) = range_judge(x, 1) / range_judge(x, 2) - 1.0;
        minx = min(minx, range_judge(x, 0));
        maxx = max(maxx, range_judge(x, 0));
        miny = min(miny, range_judge(x, 1));
        maxy = max(maxy, range_judge(x, 1));
    }
    CImg<double> temp_res((int)(maxx - minx + 1.0), (int)(maxy - miny + 1.0), 1, img1.spectrum(), invalid_pix);
    CImg<double> temp_res1(temp_res), temp_res2(temp_res);
    int tw = temp_res.width(), th = temp_res.height();
    cimg_forXY(img1, x, y) {
        int tempx = x - minx;
        int tempy = y - miny;
        cimg_forC(img1, c) {
            temp_res1(tempx, tempy, 0, c) = img1(x, y, 0, c);
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
    mulres(temp_res1, temp_res2);
    res.display();
    cout << res.min() <<  ' ' << res.max() << endl;
    //temp_res1.save("m1.bmp");
    //temp_res2.save("m2.bmp");
    //temp_res.display();
    // 特征点的匹配
    
}

void mosaic::mulres(const CImg<double>& img1, const CImg<double>& img2)
{
    CImg<double> mask(img1.width(), img1.height(), 1, 1);
    cimg_forXY(mask, x, y) {
        if (img1(x, y) == invalid_pix && img2(x, y) == invalid_pix) {
            mask(x, y) = 0.5;
        }
        if (img1(x, y) == invalid_pix && img2(x, y) != invalid_pix) {
            mask(x, y) = 0.0;
        }
        if (img1(x, y) != invalid_pix && img2(x, y) == invalid_pix) {
            mask(x, y) = 1.0;
        }
        if (img1(x, y) != invalid_pix && img2(x, y) != invalid_pix) {
            mask(x, y) = 0.5;
        }
    }
    int numLayer = 7;
    // gaussian pyramid
    vector<CImg<double>> im1p = get_gaussian_pyramid(img1, numLayer);
    vector<CImg<double>> im2p = get_gaussian_pyramid(img2, numLayer);
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
    res = imp.back();
    for (int i = imp.size() - 2; i >= 0; i--) {
        res.get_resize(imp[i]).display();
        res = imp[i] + res.get_resize(imp[i]);
        
    }
    cimg_forXYC(res, x, y, c) {
        res(x, y, c) = max(res(x, y, c), 0.0);
        res(x, y, c) = min(res(x, y, c), 255.0);
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
