#include <bits/stdc++.h>
using namespace std;
const int MAXN = 357, MAXM = 309, DIM = 128;
vector<vector<double>> v;
void *d;

int main() {
    freopen("raw1.log","r",stdin);
    for (int i = 0; i < MAXN; i++) {
        vector<double> z;
        for (int j = 0; j < DIM; j++) {
            double x;
            cin >> x;
            z.push_back(x);
        }
        v.push_back(z);
    }
    d=v.data()->data();
    cout << *((double const*)(d)+130) << endl;   
}
