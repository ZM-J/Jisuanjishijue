#include <bits/stdc++.h>
using namespace std;
const int MAXN = 357, MAXM = 309, DIM = 128;
double a[MAXN][DIM], b[MAXM][DIM];

struct PX {
    double d;
    int id;
    bool operator <(const PX &b) const {
        return d < b.d;
    }
}px[MAXM];

int main() {
    freopen("raw1.log","r",stdin);
    for (int i = 0; i < MAXN; i++)
        for (int j = 0; j < DIM; j++)
            cin >> a[i][j];
    freopen("raw2.log","r",stdin);
    for (int i = 0; i < MAXM; i++)
        for (int j = 0; j < DIM; j++)
            cin >> b[i][j];
    freopen("zjl.log","w",stdout);
    cout << fixed << setprecision(6);
    for (int i = 0; i < MAXN; i++) {
        for (int j = 0; j < MAXM; j++) {
            px[j].id = j;
            px[j].d = 0;
            for (int k = 0; k < DIM; k++) {
                double dx = a[i][k] - b[j][k];
                px[j].d += dx * dx;
            }
            px[j].d = sqrt(px[j].d);
        }
        sort(px, px + MAXM);
        if (px[0].d * 1.5 <= px[1].d) {
            cout << i << ' ' << px[0].id << ' ' << px[0].d << ' ' << px[1].d << endl;
        }
    }
    return 0;
}
