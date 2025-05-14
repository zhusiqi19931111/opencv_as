//
// Created by cherish on 2025/4/28.
//

#ifndef OPENCV_MATFUN_H
#define OPENCV_MATFUN_H
#include <string>
#include <jni.h>
#include "log.h"
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>

using namespace cv;
using namespace std;

class MatFun {
public:
    MatFun();
    MatFun(const MatFun& mat);
    ~MatFun();
public:
    jobject createBitmap(JNIEnv *env, jint width, jint height, int type);
    jobject createBitmapByDecodeFile(JNIEnv *env,jstring path);

    int bitmap2mat(JNIEnv *env, Mat &mat, jobject &bitmap);

    int mat2bitmap(JNIEnv *env, Mat &mat, jobject &bitmap);

    void matObj(JNIEnv *env,jstring path);

    void loadBitmapPixelEdit(JNIEnv *env, jobject bitmap);

    jobject pixelEdit(JNIEnv *env,jstring path);

    jobject imaAdd(JNIEnv *env,jstring path1, jstring path2);

    jobject saturationBrightnessContrast(JNIEnv *env,jstring path);

    void reliefSpecialEffects(JNIEnv *env, jobject bitmap);

    void mosaicSpecialEffects(JNIEnv *env, jobject bitmap);

    void mirrorSpecialEffects(JNIEnv *env, jobject bitmap);

    void inverseWorldSpecialEffects(JNIEnv *env, jobject bitmap);

    void glassSpecialEffects(JNIEnv *env, jobject bitmap);

    void oilPaintingSpecialEffects(JNIEnv *env, jobject bitmap);

    void croppingBitmap(JNIEnv *env, jobject bitmap);

    void fishEyeSpecialEffects(JNIEnv *env, jobject bitmap);

    void magnifierSpecialEffects(JNIEnv *env, jobject bitmap);


};


#endif //OPENCV_MATFUN_H
