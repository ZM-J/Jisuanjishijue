#include "sift.h"

sift::sift(const CImg<unsigned int>& img)
{
    //ofstream f("raw2.log");
    // SIFT滤波器参数
    int noctaves = 4, nlevels = 2, o_min = 0;

    vl_sift_pix *ImageData = new vl_sift_pix[img.size()];
    cimg_forXY(img, x, y) {
        ImageData[y*img.width() + x] = (unsigned int)(0.3*img(x, y, 0, 0) + 0.6*img(x, y, 0, 1) + 0.1*img(x, y, 0, 2));
    }
    VlSiftFilt *SiftFilt = NULL;
    // 创建一个新的sift滤波器  
    SiftFilt = vl_sift_new(img.width(), img.height(), noctaves, nlevels, o_min);

    int KeyPoint = 0;
    if (vl_sift_process_first_octave(SiftFilt, ImageData) != VL_ERR_EOF)
    {
        do
        {
            //计算每组中的关键点    
            vl_sift_detect(SiftFilt);
            //遍历并绘制每个点    
            KeyPoint = SiftFilt->nkeys;//检测到的关键点的数目
            VlSiftKeypoint *pKeyPoint = SiftFilt->keys;//检测到的关键点
            for (int i = 0; i < SiftFilt->nkeys; i++, pKeyPoint++)
            {
                //计算并遍历每个点的方向    
                double angles[4];
                int angleCount = vl_sift_calc_keypoint_orientations(SiftFilt, angles, pKeyPoint);//计算关键点的方向  
                for (int j = 0; j < angleCount; j++)
                {
                    //printf("%d: %f\n", j, TemptAngle);
                    //计算每个方向的描述    
                    float *Descriptors = new float[128];
                    vl_sift_calc_keypoint_descriptor(SiftFilt, Descriptors, pKeyPoint, angles[j]);
                    points.push_back(*pKeyPoint);
                    vector<double> tmpd(Descriptors, Descriptors+128);
                    ang.push_back(angles[j]);
                    d.push_back(tmpd);
                    //f << pKeyPoint->x << ' ' << pKeyPoint->y << ' ' << pKeyPoint->sigma << ' ' << angles[j];
                    /*for (int i = 0; i < 128; i++) {
                        f << ' ' << tmpd[i];
                    }
                    f << endl;*/
                    delete[]Descriptors;
                }
            }
            //下一阶
        } while (vl_sift_process_next_octave(SiftFilt) != VL_ERR_EOF);
    }
    vl_sift_delete(SiftFilt);
    delete[]ImageData;
    //f.close();
}
