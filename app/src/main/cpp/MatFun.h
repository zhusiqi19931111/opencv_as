//
// Created by cherish on 2025/4/28.
//

#ifndef OPENCV_MATFUN_H
#define OPENCV_MATFUN_H
#include <opencv2/opencv.hpp>
#include <string>
using namespace cv;
using namespace std;

class MatFun {
public:
    void matObj(string path);
    void pixelEdit(string path);
    void imaAdd(string path1,string path2);
    void saturationBrightnessContrast(string path);
};


#endif //OPENCV_MATFUN_H
