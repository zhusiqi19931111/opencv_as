//
// Created by cherish on 2025/4/28.
//

#include "MatFun.h"

void MatFun::matObj(string path) {
    cout << "matObj path:" << path << endl;
    Mat src = imread(path, IMREAD_COLOR_BGR);

    //rows 代表 高 ，cols 代表宽
    Mat mat1 = Mat(4, 3, IMREAD_COLOR_BGR);
    Mat mat2 = Mat(Size(3, 4), IMREAD_COLOR_BGR);
    cout << "matObj mat1:" << mat1 << "mat2" << mat2 << endl;
    // imshow("show.jpg",src);


}

void MatFun::pixelEdit(string path) {
    cout << "pixelEdit path:" << path << endl;
}

void MatFun::imaAdd(string path1, string path2) {
    cout << "imaAdd path:" << path1 << "path2" << path2 << endl;
}

void MatFun::saturationBrightnessContrast(string path) {
    cout << "saturationBrightnessContrast path:" << path << endl;
}
