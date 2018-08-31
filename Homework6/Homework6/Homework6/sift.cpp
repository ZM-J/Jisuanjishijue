#include "sift.h"

sift::sift(const CImg<unsigned int>& img)
{
    //ofstream f("raw2.log");
    // SIFT�˲�������
    int noctaves = 4, nlevels = 2, o_min = 0;

    vl_sift_pix *ImageData = new vl_sift_pix[img.size()];
    cimg_forXY(img, x, y) {
        ImageData[y*img.width() + x] = (unsigned int)(0.3*img(x, y, 0, 0) + 0.6*img(x, y, 0, 1) + 0.1*img(x, y, 0, 2));
    }
    VlSiftFilt *SiftFilt = NULL;
    // ����һ���µ�sift�˲���  
    SiftFilt = vl_sift_new(img.width(), img.height(), noctaves, nlevels, o_min);

    int KeyPoint = 0;
    if (vl_sift_process_first_octave(SiftFilt, ImageData) != VL_ERR_EOF)
    {
        do
        {
            //����ÿ���еĹؼ���    
            vl_sift_detect(SiftFilt);
            //����������ÿ����    
            KeyPoint = SiftFilt->nkeys;//��⵽�Ĺؼ������Ŀ
            VlSiftKeypoint *pKeyPoint = SiftFilt->keys;//��⵽�Ĺؼ���
            for (int i = 0; i < SiftFilt->nkeys; i++, pKeyPoint++)
            {
                //���㲢����ÿ����ķ���    
                double angles[4];
                int angleCount = vl_sift_calc_keypoint_orientations(SiftFilt, angles, pKeyPoint);//����ؼ���ķ���  
                for (int j = 0; j < angleCount; j++)
                {
                    //printf("%d: %f\n", j, TemptAngle);
                    //����ÿ�����������    
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
            //��һ��
        } while (vl_sift_process_next_octave(SiftFilt) != VL_ERR_EOF);
    }
    vl_sift_delete(SiftFilt);
    delete[]ImageData;
    //f.close();
}
