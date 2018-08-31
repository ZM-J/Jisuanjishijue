#include "recognize.h"
#include <iostream>
using namespace std;

recognize::recognize(const segment & seg)
{
    // load model (filename: svm-mnist.model)
    load_model();
    // yuce
    for (auto row : seg.res) {
        result.push_back("");
        for (auto item : row) {
            char res = predict(item.first);
            result.back() += res;
        }
    }
    // boom shakalaka
    svm_free_and_destroy_model(&model);
    free(x);
}

void recognize::load_model()
{
    model = svm_load_model("svm-mnist.model");
    if (model == 0)
    {
        fprintf(stderr, "can't open model file svm-mnist.model\n");
        exit(1);
    }
    x = (struct svm_node *) malloc(max_nr_attr * sizeof(struct svm_node));
}

char recognize::predict(const Mat & img)
{
    double predict_label;
    int i = 0;
    cimg_forX(img, xx) {
        cimg_forY(img, yy) {
            if (i >= max_nr_attr - 1)	// need one more for index = -1
            {
                max_nr_attr *= 2;
                x = (struct svm_node *) realloc(x, max_nr_attr * sizeof(struct svm_node));
            }
            x[i].index = i + 1;
            x[i].value = img(xx, yy);
            ++i;
        }
    }
    x[i].index = -1;
    predict_label = svm_predict(model, x);
    /*
    for (int i = 1; i <= 28 * 28; i++) {
        cout << x[i].value << ' ';
    }
    cout << endl;
    cout << predict_label << endl;
    */
    return (char)(predict_label)+'0';
}
