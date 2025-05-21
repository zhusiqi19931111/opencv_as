//
// Created by cherish on 2025/5/20.
//

#ifndef OPENCV_IMAGEPROC_H
#define OPENCV_IMAGEPROC_H

#include <opencv2/opencv.hpp>
#include "log.h"
#include "string"
#include "stdio.h"
#include "iostream"
#include <cmath>
using namespace std;
using namespace cv;

class ImageProc {
public:
    void pyrUpOrDown(const Mat &src, Mat &dst, const Size &dstsize);
    void dRemap(const Mat &src,  Mat &dst, Mat & mat_x, Mat & mat_y);
    void calcHist(const Mat &src,  Mat &dst);
    void normalize(const Mat &src,  Mat &dst,int max_v);
    void equalizeHist(const Mat & src, Mat &dst);
};


#endif //OPENCV_IMAGEPROC_H
