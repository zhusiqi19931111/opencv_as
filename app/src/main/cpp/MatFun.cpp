//
// Created by cherish on 2025/4/28.
//

#include "MatFun.h"


MatFun::MatFun() {
    LOGI("MatFun 执行了无参构造函数");
}

MatFun::MatFun(const MatFun &mat) {
    LOGI("MatFun 执行copy构造函数");
}

MatFun::~MatFun() {
    LOGI("MatFun 执行析构函数");
}

void MatFun::matObj(JNIEnv *env, jstring path) {

    jobject bitmap = createBitmapByDecodeFile(env, path);
    Mat mat;
    int result = bitmap2mat(env, mat, bitmap);
    if (result < 0) {
        LOGE("matObj bitmap2Mat error");
        return;
    }
    Mat mat1;
    //浅拷贝
    mat1 = mat;
    Mat mat2 = mat;
    //深拷贝
    Mat mat3;
    mat.copyTo(mat3);
    Mat mat4;
    mat4 = mat3.clone();
    LOGI("matObj bitmap2Mat success");

}

void MatFun::loadBitmapPixelEdit(JNIEnv *env, jobject bitmap) {
    Mat mat;
    int result = bitmap2mat(env, mat, bitmap);
    if (result < 0) {
        LOGE("loadBitmapPixelEdit bitmap2Mat error");
        return;
    }
    LOGI("loadBitmapPixelEdit Mat type:%d channels:%d", mat.type(), mat.channels());
    if (mat.channels() == 4) {
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                Vec4b pixel = mat.at<Vec4b>(i, j);
                uchar b = pixel[0];
                uchar g = pixel[1];
                uchar r = pixel[2];
                uchar a = pixel[3];
                uchar gray = b + 4 * g + 3 * r;
                mat.at<Vec4b>(i, j)[0] = gray;
                mat.at<Vec4b>(i, j)[1] = gray;
                mat.at<Vec4b>(i, j)[2] = gray;
                mat.at<Vec4b>(i, j)[3] = a;
            }
        }
    }else if (mat.channels() == 3) {
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                Vec3b pixel = mat.at<Vec3b>(i, j);
                uchar b = pixel[0];
                uchar g = pixel[1];
                uchar r = pixel[2];
                uchar gray = b + 4 * g + 3 * r;
                mat.at<Vec3b>(i, j)[0] = gray;
                mat.at<Vec3b>(i, j)[1] = gray;
                mat.at<Vec3b>(i, j)[2] = gray;

            }
        }
    } else if (mat.channels() == 1) {
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                uchar pixel = mat.at<uchar>(i, j);
                uchar gray = 255 - pixel;
                mat.at<Vec3b>(i, j) = gray;
            }
        }
    } else {
        LOGE("loadBitmapPixelEdit  error");
    }
    mat2bitmap(env, mat, bitmap);

}

jobject MatFun::pixelEdit(JNIEnv *env, jstring path) {

    //1.转成灰度图
    jobject bitmap = createBitmapByDecodeFile(env, path);
    Mat mat;
    int result = bitmap2mat(env, mat, bitmap);
    if (result < 0) {
        LOGE("pixelEdit bitmap2Mat error");
        return bitmap;
    }
    LOGI("pixelEdit Mat type:%d channels:%d", mat.type(), mat.channels());
    if (mat.channels() == 4) {
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                Vec4b pixel = mat.at<Vec4b>(i, j);
                uchar b = pixel[0];
                uchar g = pixel[1];
                uchar r = pixel[2];
                uchar a = pixel[3];
                uchar gray = 0.11 * b + 0.59 * g + 0.3 * r;
                mat.at<Vec4b>(i, j)[0] = gray;
                mat.at<Vec4b>(i, j)[1] = gray;
                mat.at<Vec4b>(i, j)[2] = gray;
                mat.at<Vec4b>(i, j)[3] = a;
            }
        }
    }else if (mat.channels() == 3) {
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                Vec3b pixel = mat.at<Vec3b>(i, j);
                uchar b = pixel[0];
                uchar g = pixel[1];
                uchar r = pixel[2];
                uchar gray = b + 4 * g + 3 * r;
                mat.at<Vec3b>(i, j)[0] = gray;
                mat.at<Vec3b>(i, j)[1] = gray;
                mat.at<Vec3b>(i, j)[2] = gray;

            }
        }
    }  else if (mat.channels() == 1) {
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                uchar pixel = mat.at<uchar>(i, j);
                uchar gray = pixel / 3;
                mat.at<Vec3b>(i, j) = gray;
            }
        }
    } else {
        LOGE("pixelEdit  error");
    }
    mat2bitmap(env, mat, bitmap);
    return bitmap;

}

jobject MatFun::imaAdd(JNIEnv *env, jstring path1, jstring path2) {
    //1.两张图片合成
    jobject bitmap1 = createBitmapByDecodeFile(env, path1);
    jobject bitmap2 = createBitmapByDecodeFile(env, path2);
    Mat mat1;
    Mat mat2;
    Mat mat3;
    int result1 = bitmap2mat(env, mat1, bitmap1);
    int result2 = bitmap2mat(env, mat2, bitmap2);
    if (result1 < 0 || result2 < 0) {
        LOGE("imaAdd bitmap2Mat error");
        return bitmap1;
    }
    LOGI("imaAdd mat1 type:%d channels:%d mat2 type:%d channels:%d", mat1.type(),
         mat1.channels(), mat2.type(), mat2.channels());

    int minRows = min(mat1.rows, mat2.rows);
    int minCols = min(mat1.cols, mat2.cols);
    LOGI("imaAdd mat1 rows:%d cols:%d mat2 rows:%d cols:%d", mat1.rows, mat1.cols, mat2.rows,
         mat2.cols);

    LOGI("imaAdd mat minRows:%d minCols:%d", minRows, minCols);

    mat3.create(minRows, minCols, CV_8UC4);

    for (int i = 0; i < minRows; ++i) {
        for (int j = 0; j < minCols; ++j) {
            if (mat1.channels() == 4 && mat1.channels() == 4) {
                Vec4b pixel1 = mat1.at<Vec4b>(i, j);
                Vec4b pixel2 = mat2.at<Vec4b>(i, j);
                uchar b = pixel1[0] + pixel2[0];
                uchar g = pixel1[1] + pixel2[1];
                uchar r = pixel1[2] + pixel2[2];
                uchar a = pixel1[3] + pixel2[3];
                mat3.at<Vec4b>(i, j)[0] =  saturate_cast<uchar>(b);
                mat3.at<Vec4b>(i, j)[1] =  saturate_cast<uchar>(g);
                mat3.at<Vec4b>(i, j)[2] =  saturate_cast<uchar>(r);
                mat3.at<Vec4b>(i, j)[3] =  saturate_cast<uchar>(a);
            } else if (mat1.channels() == 1 && mat1.channels() == 4) {
                uchar pixel1 = mat1.at<uchar>(i, j);
                Vec4b pixel2 = mat2.at<Vec4b>(i, j);
                pixel2[0] = pixel1 + pixel2[0];
                mat3.at<Vec4b>(i, j)[0] = saturate_cast<uchar>(pixel2[0]);
                mat3.at<Vec4b>(i, j)[1] = saturate_cast<uchar>(pixel2[1]);
                mat3.at<Vec4b>(i, j)[2] = saturate_cast<uchar>(pixel2[2]);
                mat3.at<Vec4b>(i, j)[3] = saturate_cast<uchar>(pixel2[3]);
            } else if (mat1.channels() == 4 && mat1.channels() == 1) {
                uchar pixel2 = mat1.at<uchar>(i, j);
                Vec4b pixel1 = mat2.at<Vec4b>(i, j);
                pixel1[0] = pixel2 + pixel1[0];
                mat3.at<Vec4b>(i, j)[0] = saturate_cast<uchar>(pixel1[0]);
                mat3.at<Vec4b>(i, j)[1] = saturate_cast<uchar>(pixel1[1]);
                mat3.at<Vec4b>(i, j)[2] = saturate_cast<uchar>(pixel1[2]);
                mat3.at<Vec4b>(i, j)[3] = saturate_cast<uchar>(pixel1[3]);
            } else if (mat1.channels() == 1 && mat1.channels() == 1) {
                uchar pixel1 = mat1.at<uchar>(i, j);
                uchar pixel2 = mat2.at<uchar>(i, j);
                mat3.at<Vec4b>(i, j)[0] = saturate_cast<uchar>(pixel1 + pixel2);
                mat3.at<Vec4b>(i, j)[1] = saturate_cast<uchar>(pixel1 + pixel2);
                mat3.at<Vec4b>(i, j)[2] = saturate_cast<uchar>(pixel1 + pixel2);
                mat3.at<Vec4b>(i, j)[3] = saturate_cast<uchar>(pixel1 + pixel2);
            }else {
                LOGE("imaAdd mat2Bitmap error ,chanel 对应的单个像素的类型未实现，请自行补充");
            }

        }
    }
    jobject resultBitmap = createBitmap(env, minCols, minRows, 5);
    mat2bitmap(env, mat3, resultBitmap);
    LOGI("imaAdd mat2Bitmap sucess");
    return resultBitmap;

}

jobject MatFun::saturationBrightnessContrast(JNIEnv *env, jstring path) {
    jobject bitmap = createBitmapByDecodeFile(env, path);
    Mat mat;
    int result = bitmap2mat(env, mat, bitmap);
    if (result < 0) {
        LOGE("saturationBrightnessContrast bitmap2Mat error");
        return bitmap;
    }
    LOGI("saturationBrightnessContrast Mat type:%d channels:%d", mat.type(), mat.channels());
    int alpha = 1.2;
    int beta = 20;
    if (mat.channels() == 4) {
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                Vec4b pixel = mat.at<Vec4b>(i, j);
                uchar b = pixel[0];
                uchar g = pixel[1];
                uchar r = pixel[2];
                uchar a = pixel[3];
                mat.at<Vec4b>(i, j)[0] = saturate_cast<uchar>(alpha * b+beta);
                mat.at<Vec4b>(i, j)[1] = saturate_cast<uchar>(alpha * g + beta);
                mat.at<Vec4b>(i, j)[2] = saturate_cast<uchar>(alpha * r + beta);
                mat.at<Vec4b>(i, j)[3] = saturate_cast<uchar>(alpha * a + beta);
            }
        }
    } else  if (mat.channels() == 3) {
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                Vec3b pixel = mat.at<Vec3b>(i, j);
                uchar b = pixel[0];
                uchar g = pixel[1];
                uchar r = pixel[2];
                mat.at<Vec3b>(i, j)[0] = saturate_cast<uchar>(alpha * b+beta);
                mat.at<Vec3b>(i, j)[1] = saturate_cast<uchar>(alpha * g + beta);
                mat.at<Vec3b>(i, j)[2] = saturate_cast<uchar>(alpha * r + beta);
            }
        }
    } else if (mat.channels() == 1) {
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                uchar pixel = mat.at<uchar>(i, j);
                mat.at<uchar>(i, j) = saturate_cast<uchar>(alpha * pixel + beta);
            }
        }
    } else {
        LOGE("saturationBrightnessContrast  error");
    }
    mat2bitmap(env, mat, bitmap);
    return bitmap;
}

jobject MatFun::createBitmapByDecodeFile(JNIEnv *env, jstring path) {
    //BitmapFactory.decodeFile()
    jclass jclazz = env->FindClass("android/graphics/BitmapFactory");
    jmethodID decodeFileMethodID = env->GetStaticMethodID(jclazz, "decodeFile",
                                                          "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
    jobject bitmap = env->CallStaticObjectMethod(jclazz, decodeFileMethodID, path);
    env->DeleteLocalRef(jclazz);
    LOGI("createBitmapByDecodeFile success");
    return bitmap;

}

jobject MatFun::createBitmap(JNIEnv *env, jint width, jint height, int type) {
    // Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
    int wid = width;
    int hei = height;

    jclass configClass = env->FindClass("android/graphics/Bitmap$Config");
    char *name = "ARGB_8888";
    jstring configValue = env->NewStringUTF(name);
    jmethodID configmethodID = env->GetStaticMethodID(configClass, "valueOf",
                                                      "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    jobject configParams = env->CallStaticObjectMethod(configClass, configmethodID, configValue);

    jclass bitmapClass = env->FindClass("android/graphics/Bitmap");

    jmethodID jmethod = env->GetStaticMethodID(bitmapClass, "createBitmap",
                                               "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jobject bitmap = env->CallStaticObjectMethod(bitmapClass, jmethod, wid, hei, configParams);
    LOGI("createBitmap success");
    env->DeleteLocalRef(configClass);
    env->DeleteLocalRef(bitmapClass);
    return bitmap;
}

/**
 * 将bitmap 转成mat
 */
int MatFun::bitmap2mat(JNIEnv *env, Mat &mat, jobject &bitmap) {

    AndroidBitmapInfo bitmap_info;
    void *pixelPtr;
    int getInfoRes = AndroidBitmap_getInfo(env, bitmap, &bitmap_info);
    if (getInfoRes < 0) {
        return getInfoRes;
    }
    // 检查 Bitmap 格式
    CV_Assert(bitmap_info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              bitmap_info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    int lockPixelsRes = AndroidBitmap_lockPixels(env, bitmap, &pixelPtr);
    if (lockPixelsRes < 0) {
        return lockPixelsRes;
    }
    LOGI("bitmap2Mat current format:%d", bitmap_info.format);

    mat.create(bitmap_info.height, bitmap_info.width, CV_8UC4);
    if (bitmap_info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {//对应CV_8UC4
        Mat temp = Mat(bitmap_info.height, bitmap_info.width, CV_8UC4, pixelPtr);
        temp.copyTo(mat);

    } else if (bitmap_info.format == ANDROID_BITMAP_FORMAT_RGB_565) {//对应CV_8UC2
        Mat temp = Mat(bitmap_info.height, bitmap_info.width, CV_8UC2, pixelPtr);
        cvtColor(temp, mat, COLOR_BGR5652RGB);
    } else {
        LOGE("bitmap2Mat current format:%d unknown don't handler", bitmap_info.format);
        return -1;
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    return 0;

}

/**
 * 将mat 转成bitmap
 */
int MatFun::mat2bitmap(JNIEnv *env, Mat &mat, jobject &bitmap) {
    AndroidBitmapInfo bitmap_info;
    void *pixelPtr;
    int getInfoRes = AndroidBitmap_getInfo(env, bitmap, &bitmap_info);
    if (getInfoRes < 0) {
        return getInfoRes;
    }
    // 检查 Bitmap 格式
    CV_Assert(bitmap_info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              bitmap_info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    int lockPixelsRes = AndroidBitmap_lockPixels(env, bitmap, &pixelPtr);
    if (lockPixelsRes < 0) {
        return lockPixelsRes;
    }
    LOGI("mat2Bitmap current format:%d", bitmap_info.format);

    if (bitmap_info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {//对应CV_8UC4
        Mat temp = Mat(bitmap_info.height, bitmap_info.width, CV_8UC4, pixelPtr);
        if (mat.type() == CV_8UC1) {
            cvtColor(mat, temp, cv::COLOR_GRAY2RGBA);
        } else if (mat.type() == CV_8UC3) {
            cvtColor(mat, temp, cv::COLOR_RGB2RGBA);
        } else if (mat.type() == CV_8UC4) {
            mat.copyTo(temp);
        }

    } else if (bitmap_info.format == ANDROID_BITMAP_FORMAT_RGB_565) {//对应CV_8UC2
        Mat temp = Mat(bitmap_info.height, bitmap_info.width, CV_8UC2, pixelPtr);
        if (mat.type() == CV_8UC1) {
            cvtColor(mat, temp, cv::COLOR_GRAY2BGR565);
        } else if (mat.type() == CV_8UC3) {
            cvtColor(mat, temp, cv::COLOR_RGB2BGR565);
        } else if (mat.type() == CV_8UC4) {
            cvtColor(mat, temp, cv::COLOR_RGBA2BGR565);
        }

    } else {
        LOGE("mat2Bitmap current format:%d unknown don't handler", bitmap_info.format);
        return -1;
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    return 0;
}

/**
 * 浮雕特效
 */
void MatFun::reliefSpecialEffects(JNIEnv *env, jobject bitmap) {

}

/**
 * 马赛克特效
 */
void MatFun::mosaicSpecialEffects(JNIEnv *env, jobject bitmap) {

}

/**
 * 镜像特效
 */
void MatFun::mirrorSpecialEffects(JNIEnv *env, jobject bitmap) {

}

/**
 * 逆世界特效
 */
void MatFun::inverseWorldSpecialEffects(JNIEnv *env, jobject bitmap) {

}

/**
 * 毛玻璃特效
 */
void MatFun::glassSpecialEffects(JNIEnv *env, jobject bitmap) {

}

/**
 * 油画特效
 */
void MatFun::oilPaintingSpecialEffects(JNIEnv *env, jobject bitmap) {

}

/**
 * bitmap 裁剪
 */
void MatFun::croppingBitmap(JNIEnv *env, jobject bitmap) {

}

/**
 * 鱼眼特效
 * @param env
 * @param bitmap
 */
void MatFun::fishEyeSpecialEffects(JNIEnv *env, jobject bitmap) {

}

/**
 * 放大镜特效
 * @param env
 * @param bitmap
 */
void MatFun::magnifierSpecialEffects(JNIEnv *env, jobject bitmap){

}


