//
// Created by cherish on 2025/4/28.
//

#ifndef OPENCV_MATFUN_H
#define OPENCV_MATFUN_H

#include <string>
#include <jni.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include "log.h"
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include "Random.hpp"
#include "ImageProc.h"

using namespace cv;
using namespace std;

class MatFun {
public:
    CascadeClassifier cascadeClassifier;
public:
    MatFun();

    MatFun(const MatFun &mat);

    ~MatFun();



public:
    jobject createBitmap(JNIEnv *env, jint width, jint height, int type);

    jobject createBitmapByDecodeFile(JNIEnv *env, jstring path);

    int bitmap2mat(JNIEnv *env, Mat &mat, jobject &bitmap);

    int mat2bitmap(JNIEnv *env, Mat &mat, jobject &bitmap);

    void matObj(JNIEnv *env, jstring &path);

    void loadBitmapPixelEdit(JNIEnv *env, jobject &bitmap);

    jobject pixelEdit(JNIEnv *env, jstring &path);

    jobject imaAdd(JNIEnv *env, jstring &path1, jstring &path2);

    jobject saturationBrightnessContrast(JNIEnv *env, jstring &path);

    void reliefSpecialEffects(JNIEnv *env, jobject &bitmap);

    void mosaicSpecialEffects(JNIEnv *env, jobject &bitmap);

    void faceMosaicSpecialEffects(JNIEnv *env, jstring &filName, jobject &bitmap);

    void mirrorSpecialEffects(JNIEnv *env, jobject &bitmap);

    void inverseWorldSpecialEffects(JNIEnv *env, jobject &bitmap);

    void glassSpecialEffects(JNIEnv *env, jobject &bitmap);

    void oilPaintingSpecialEffects(JNIEnv *env, jobject &bitmap);

    void oilPaintingSpecialEffects2(JNIEnv *env, jobject &bitmap);

    jobject croppingBitmap(JNIEnv *env, jobject &bitmap);

    void fishEyeSpecialEffects(JNIEnv *env, jobject &bitmap);

    void magnifierSpecialEffects(JNIEnv *env, jobject &bitmap);

    void magnifierSpecialEffects(JNIEnv *env, jfloat x, jfloat y, jobject &bitmap);

    void advancedMagnifierEffect(JNIEnv *env, jobject bitmap, Mat &src, Mat &dst, Point center,
                                 int radius, float magnification,
                                 bool lensEffect, bool fishEye);



    jobject rotateImage(JNIEnv *env, jobject &bitmap);

    void matrixTransform(JNIEnv *env, jobject &bitmap);

    jobject reSize(JNIEnv *pEnv, jobject &bitmap);

    void remap(JNIEnv *env, jobject &bitmap);
    void equalizeHist(JNIEnv *env, jobject &bitmap);
    jobject calcuHist(JNIEnv *env, jobject &bitmap);
    void matLight(JNIEnv *env, jobject &bitmap);

    jobject codeVerification(JNIEnv *env, jobject &bitmap);
    jobject codeTiltVerification(JNIEnv *env, jobject &bitmap);
    jobject codeRoundVerification(JNIEnv *env, jobject &bitmap);
    //getMouseCallbackUserData 暂时用不到
    friend void *getMouseCallbackUserData();


    jobject hog(JNIEnv *env, jobject &bitmap);
    jobject lbp(JNIEnv *env, jobject &bitmap);
    jobject haar(JNIEnv *env, jobject &bitmap);

};


#endif //OPENCV_MATFUN_H
