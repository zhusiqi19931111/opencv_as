//
// Created by cherish on 2025/5/18.
//

#ifndef OPENCV_DEFINOPENCVIMPL_HPP
#define OPENCV_DEFINOPENCVIMPL_HPP

#include <jni.h>
#include "opencv2/opencv.hpp"
#include "MatFun.h"

using namespace cv;

extern "C"
JNIEXPORT jlong JNICALL
Java_com_yaxiu_opencv_Mat_nCrateMatIII(JNIEnv *env, jobject thiz, jint rows, jint cols, jint type) {
    Mat *m = new Mat(rows, cols, type);
    return reinterpret_cast<jlong>(m);
}
extern "C"
JNIEXPORT jlong JNICALL
Java_com_yaxiu_opencv_Mat_nCrateMat(JNIEnv *env, jobject thiz) {
    Mat *m = new Mat();
    return reinterpret_cast<jlong>(m);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_Mat_nReleaseMat(JNIEnv *env, jobject thiz, jlong jni_mat_ads) {
    Mat *mat = reinterpret_cast<Mat *>(jni_mat_ads);
    delete mat;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_Mat_nBitmap2mat(JNIEnv *env, jobject thiz, jlong jni_mat_ads,
                                      jobject bitmap) {
    Mat *mat = reinterpret_cast<Mat *>(jni_mat_ads);
    MatFun f; //注意是否需要初始化构造函数
    f.bitmap2mat(env, *mat, bitmap);


}

#endif //OPENCV_DEFINOPENCVIMPL_HPP


