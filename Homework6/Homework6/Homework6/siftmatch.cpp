#include "siftmatch.h"

siftmatch::siftmatch(const sift & s1, const sift & s2)
{
    //ofstream f("3.log");
    // A descriptor D1 is matched to a descriptor D2 only if the
    // distance d(D1, D2) multiplied by THRESH is not greater than the
    // distance of D1 to all other descriptors.
    // The default value of THRESH is 1.5.
    double thresh = 1.5;
    // ��s2�е�sift��������kdtree
    kdt = vl_kdforest_new(VL_TYPE_DOUBLE, 128, 1, VlDistanceL2);
    // ת��һά������ʽ
    double *s2d = new double[s2.d.size() * 128];
    for (int i = 0; i < s2.d.size(); i++)
        for (int j = 0; j < 128; j++)
            s2d[i * 128 + j] = s2.d[i][j];
    vl_kdforest_build(kdt, s2.d.size(), s2d);
    // s1��
    for (int i = 0; i < s1.d.size(); i++) {
        VlKDForestNeighbor nei[2];
        vl_kdforest_query(kdt, nei, 2, s1.d[i].data());
        if (nei[0].distance * thresh <= nei[1].distance) {
            matches.push_back(make_pair(i, nei[0].index));
            //f << i << ' ' << nei[0].index << ' ' << nei[0].distance << ' ' << nei[1].distance << endl;
        }
    }
    // kd��������
    vl_kdforest_delete(kdt);
    delete[]s2d;
    // ��־��¼
    //f.close();
}

