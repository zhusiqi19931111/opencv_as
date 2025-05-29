#include "TupleVaArgFun.hpp"
#include <string>
#include "stdio.h"
#include <android/log.h>
#include <opencv2/opencv.hpp>
#include <android/bitmap.h>
#include "MatFun.h"
#include "log.h"
#include "TupleVaArgFun.hpp"
#include "JniThreadFun.hpp"

using namespace std;
using namespace cv;

CascadeClassifier cascadeClassifier;


extern "C" {

void bitmap2Mat(JNIEnv *env, Mat &mat, jobject bitmap);
void mat2Bitmap(JNIEnv *env, Mat &mat, jobject bitmap);
void nativeBitmapPixel(JNIEnv *env, jobject thiz, jobject bitmap);
void nativeBitmapPixel2(JNIEnv *env, jobject thiz, jobject bitmap);
}
JavaVM *g_vm = nullptr; // 全局JavaVM指针
// 在JNI_OnLoad中保存JavaVM
JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    g_vm = vm;
    return JNI_VERSION_1_6;
}

void bitmap2Mat(JNIEnv *env, Mat &mat, jobject bitmap) {
    LOGE("bitmap2Mat");
    // Mat 里面有个 type ： CV_8UC4 刚好对上我们的 Bitmap 中 ARGB_8888 , CV_8UC2 刚好对象我们的 Bitmap 中 RGB_565
    // 1. 获取 bitmap 信息
    AndroidBitmapInfo info;
    void *pixels;
    AndroidBitmap_getInfo(env, bitmap, &info);

    // 锁定 Bitmap 画布
    AndroidBitmap_lockPixels(env, bitmap, &pixels);
    // 指定 mat 的宽高和type  BGRA
    mat.create(info.height, info.width, CV_8UC4);

    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        // 对应的 mat 应该是  CV_8UC4
        Mat temp(info.height, info.width, CV_8UC4, pixels);
        // 把数据 temp 复制到 mat 里面
        temp.copyTo(mat);
    } else if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        // 对应的 mat 应该是  CV_8UC2
        Mat temp(info.height, info.width, CV_8UC2, pixels);
        // mat 是 CV_8UC4 ，CV_8UC2 -> CV_8UC4
        cvtColor(temp, mat, COLOR_BGR5652BGRA);
    } else {
        LOGE("bitmap2Mat info.format:%d", info.format);
    }
    // 其他要自己去转

    // 解锁 Bitmap 画布
    AndroidBitmap_unlockPixels(env, bitmap);

}

void mat2Bitmap(JNIEnv *env, Mat &mat, jobject bitmap) {
    // 1. 获取 bitmap 信息
    AndroidBitmapInfo info;
    void *pixels;
    AndroidBitmap_getInfo(env, bitmap, &info);

    // 锁定 Bitmap 画布
    AndroidBitmap_lockPixels(env, bitmap, &pixels);

    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {// C4
        Mat temp(info.height, info.width, CV_8UC4, pixels);
        if (mat.type() == CV_8UC4) {
            mat.copyTo(temp);
        } else if (mat.type() == CV_8UC2) {
            cvtColor(mat, temp, COLOR_BGR5652BGRA);
        } else if (mat.type() == CV_8UC1) {// 灰度 mat
            cvtColor(mat, temp, COLOR_GRAY2BGRA);
        } else {
            LOGE("mat2Bitmap mat.type:%d", mat.type());
        }
    } else if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {// C2
        Mat temp(info.height, info.width, CV_8UC2, pixels);
        if (mat.type() == CV_8UC4) {
            cvtColor(mat, temp, COLOR_BGRA2BGR565);
        } else if (mat.type() == CV_8UC2) {
            mat.copyTo(temp);

        } else if (mat.type() == CV_8UC1) {// 灰度 mat
            cvtColor(mat, temp, COLOR_GRAY2BGR565);
        } else {
            LOGE("mat2Bitmap mat.type:%d", mat.type());
        }
    } else {
        LOGE("mat2Bitmap info.format:%d", info.format);
    }
    // 其他要自己去转

    // 解锁 Bitmap 画布
    AndroidBitmap_unlockPixels(env, bitmap);

}

extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_loadCascade(JNIEnv *env, jobject thiz, jstring absolute_path) {

    const char *path = env->GetStringUTFChars(absolute_path, JNI_FALSE);
    bool result = cascadeClassifier.load(path);
    LOGE("加载分类器path%s", path);
    if (result) {
        LOGE("加载分类器文件成功");
    } else {
        LOGE("加载分类器文件失败");
    }

    env->ReleaseStringUTFChars(absolute_path, path);

}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_faceDetectionSaveInfo(JNIEnv *env, jobject thiz,
                                                          jobject bitmap) {

    // 检测人脸  , opencv 有一个非常关键的类是 Mat ，opencv 是 C 和 C++ 写的，只会处理 Mat , android里面是Bitmap
    // 1. Bitmap 转成 opencv 能操作的 C++ 对象 Mat , Mat 是一个矩阵
    Mat mat;
    bitmap2Mat(env, mat, bitmap);

    // 处理灰度 opencv 处理灰度图, 提高效率，一般所有的操作都会对其进行灰度处理 高斯模糊
    Mat gray_mat;
    cvtColor(mat, gray_mat, COLOR_BGRA2GRAY);

    // 再次处理 直方均衡补偿
    Mat equalize_mat;
    equalizeHist(gray_mat, equalize_mat);

    // 识别人脸，当然我们可以直接用 彩色图去做,识别人脸要加载人脸分类器文件
    std::vector<Rect> faces;
    cascadeClassifier.detectMultiScale(equalize_mat, faces, 1.1, 5);
    LOGE("人脸个数：%d", faces.size());
    if (faces.size() == 1) {
        Rect faceRect = faces[0];
        LOGE("rectangle");
        // 在人脸部分花个图
        rectangle(equalize_mat, faceRect, Scalar(255, 155, 155), 8);
        // 把 mat 我们又放到 bitmap 里面
        mat2Bitmap(env, equalize_mat, bitmap);
        // 保存人脸信息 Mat , 图片 jpg
        Mat face_info_mat(equalize_mat, faceRect);
        // 保存 face_info_mat
        return;
    }
    mat2Bitmap(env, equalize_mat, bitmap);

}


extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_opencvUpdateMat(JNIEnv *env, jobject thiz,
                                                    jstring pathsrc, jstring pathout) {

    const char *path = env->GetStringUTFChars(pathsrc, JNI_FALSE);
    Mat src = imread(path);
    if (src.empty()) {
        LOGE("无图片资源");
        return;
    }

    //opencv 提供灰度图源码
    // Mat dst;
    // cvtColor(src,dst,COLOR_BGR2GRAY);
    LOGE("rows:%d cols:%d channels:%d", src.rows, src.cols, src.channels());

    //灰度值
    for (int i = 0; i < src.rows; i++) {
        uchar *start_pixels = src.ptr<uchar>(i);
        for (int j = 0; j < src.cols; j++) {
            uchar b = start_pixels[0];
            uchar g = start_pixels[1];
            uchar r = start_pixels[2];
            //grey=0.11*b+0.59*g+0.3*r
            uchar grey = 0.11 * b + 0.59 * g + 0.3 * r;

            start_pixels[0] = grey;
            start_pixels[1] = grey;
            start_pixels[2] = grey;
            start_pixels += src.channels();
        }

    }
    const char *pathOut_ = env->GetStringUTFChars(pathout, JNI_FALSE);
    imwrite(pathOut_, src);


    Mat src2 = imread(path);

    //底片值
    for (int i = 0; i < src2.rows; i++) {
        uchar *start_pixels = src2.ptr<uchar>(i);
        for (int j = 0; j < src2.cols; j++) {
            uchar b = start_pixels[0];
            uchar g = start_pixels[1];
            uchar r = start_pixels[2];

            start_pixels[0] = 255 - b;
            start_pixels[1] = 255 - g;
            start_pixels[2] = 255 - r;
            start_pixels += src2.channels();
        }

    }
    imwrite("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/dipian_20250425172853.jpg",
            src2);

    Mat src3 = imread(path);

    //亮色值
    for (int i = 0; i < src3.rows; i++) {
        uchar *start_pixels = src3.ptr<uchar>(i);
        for (int j = 0; j < src3.cols; j++) {
            uchar b = start_pixels[0];
            uchar g = start_pixels[1];
            uchar r = start_pixels[2];


            start_pixels[0] = saturate_cast<uchar>(1.2f * b);
            start_pixels[1] = saturate_cast<uchar>(1.2 * g);
            start_pixels[2] = saturate_cast<uchar>(1.2 * r);
            start_pixels += src3.channels();
        }

    }

    imwrite("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/light_20250425172853.jpg",
            src3);

    Mat src4 = imread(path);

    //黑白图
    for (int i = 0; i < src4.rows; i++) {
        uchar *start_pixels = src4.ptr<uchar>(i);
        for (int j = 0; j < src4.cols; j++) {
            uchar b = start_pixels[0];
            uchar g = start_pixels[1];
            uchar r = start_pixels[2];


            uchar b_w = (b + g + r) / 3 > 125 ? 255 : 0;
            start_pixels[0] = saturate_cast<uchar>(b_w);
            start_pixels[1] = saturate_cast<uchar>(b_w);
            start_pixels[2] = saturate_cast<uchar>(b_w);
            start_pixels += src4.channels();
        }

    }
    imwrite("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/old_20250425172853.jpg",
            src4);
    env->ReleaseStringUTFChars(pathout, pathOut_);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_nativeBitmapPixel(JNIEnv *env, jobject thiz, jobject bitmap) {



    // nativeBitmapPixel(env,thiz, bitmap);
    //方式二
    nativeBitmapPixel2(env, thiz, bitmap);
}

void nativeBitmapPixel(JNIEnv *env, jobject thiz, jobject bitmap) {
    //方式一使用cvtColor 转灰度图
    Mat mat;
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, bitmap, &info);
    void *pixel;
    AndroidBitmap_lockPixels(env, bitmap, &pixel);
    LOGE("format:%d width:%d height:%d", info.format, info.width, info.height);
    mat.create(info.height, info.width, CV_8UC4);
    if (info.format ==
        ANDROID_BITMAP_FORMAT_RGBA_8888) { //ANDROID_BITMAP_FORMAT_RGBA_8888 对应oepncvCV_8UC4
        Mat tem(info.height, info.width, CV_8UC4, pixel);
        tem.copyTo(mat);

    } else if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        Mat tem(info.height, info.width, CV_8UC2, pixel);
        cvtColor(tem, mat, COLOR_BGR5652RGBA);
    } else {
        LOGE("info.format:%d", info.format);
    }
    AndroidBitmap_unlockPixels(env, bitmap);
    Mat greyMat;
    cvtColor(mat, greyMat, COLOR_RGBA2GRAY);
}

void nativeBitmapPixel2(JNIEnv *env, jobject thiz, jobject bitmap) {
    AndroidBitmapInfo info;
    int info_res = AndroidBitmap_getInfo(env, bitmap, &info);
    if (info_res != 0) {
        LOGE("AndroidBitmap_getInfo error");
        return;
    }
    void *pixels;
    AndroidBitmap_lockPixels(env, bitmap, &pixels);
    LOGE("format:%d width:%d height:%d", info.format, info.width, info.height);

    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) { //4通道
        for (int i = 0; i < info.width * info.height; ++i) {
            uint32_t *pixel_p = reinterpret_cast<uint32_t *>(pixels) + i;//i 切勿写成1
            uint32_t pixel = *pixel_p;
            int a = (pixel >> 24) & 0xff;
            int r = (pixel >> 16) & 0xff;
            int g = (pixel >> 8) & 0xff;
            int b = pixel & 0xff;
            int grey = (int) (0.213f * r + 0.715f * g + 0.072f * b);
            *pixel_p = (a << 24) | (grey << 16) | (grey << 8) | grey;
        }

    } else if (info.format == ANDROID_BITMAP_FORMAT_RGB_565) {//
        for (int i = 0; i < info.width * info.height; ++i) {
            uint16_t *pixel_p = reinterpret_cast<uint16_t *>(pixels) + i;//i 切勿写成1
            uint16_t pixel = *pixel_p;
            // 8888 -> 565
            int r = ((pixel >> 11) & 0x1f) << 3;
            int g = ((pixel >> 5) & 0x3f) << 2;
            int b = (pixel & 0x1f) << 3;
            int grey = (int) (0.213f * r + 0.715f * g +
                              0.072f * b); //此灰度值公式仅使用与4通道，如果是2通道的话，需要位移为四通道
            *pixel_p = ((grey >> 3) << 11) | ((grey >> 2) << 5) | (grey >> 3);
        }

    }

    AndroidBitmap_unlockPixels(env, bitmap);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_matObj(JNIEnv *env, jobject thiz) {
    MatFun matfun = MatFun();
    char *path = "/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250508104604.jpg";

    jstring jpath = env->NewStringUTF(path);
    matfun.matObj(env, jpath);
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/io/FileNotFoundException");
        env->ThrowNew(no_such_clz,
                      "Unable to decode file: java.io.FileNotFoundException NoSuchFieldException open failed: ENOENT (No such file or directory)");
        env->DeleteLocalRef(no_such_clz);
    }
    //
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_yaxiu_opencv_FaceDetection_createBitmapByDecodeFile(JNIEnv *env, jobject thiz,
                                                             jstring path) {
    MatFun matfun = MatFun();
    jobject bitmap = matfun.createBitmapByDecodeFile(env, path);
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/io/FileNotFoundException");
        env->ThrowNew(no_such_clz,
                      "Unable to decode file: java.io.FileNotFoundException NoSuchFieldException open failed: ENOENT (No such file or directory)");
        env->DeleteLocalRef(no_such_clz);
        return nullptr;
    }
    LOGD("createBitmapByDecodeFile success");
    return bitmap;
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_yaxiu_opencv_FaceDetection_opencvEditPiexl(JNIEnv *env, jobject thiz, jstring path) {
    MatFun matfun = MatFun();
    jobject bitmap = matfun.pixelEdit(env, path);
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/io/FileNotFoundException");
        env->ThrowNew(no_such_clz,
                      "Unable to decode file: java.io.FileNotFoundException NoSuchFieldException open failed: ENOENT (No such file or directory)");
        env->DeleteLocalRef(no_such_clz);
        return nullptr;
    }
    LOGD("opencvEditPiexl success");
    return bitmap;

}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_yaxiu_opencv_FaceDetection_opencvAddWeight(JNIEnv *env, jobject thiz, jstring path1,
                                                    jstring path2) {
    MatFun matfun = MatFun();
    jobject bitmap = matfun.imaAdd(env, path1, path2);
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/io/FileNotFoundException");
        env->ThrowNew(no_such_clz,
                      "Unable to decode file: java.io.FileNotFoundException NoSuchFieldException open failed: ENOENT (No such file or directory)");
        env->DeleteLocalRef(no_such_clz);
        return nullptr;
    }
    LOGD("addWeight success");
    return bitmap;
}


extern "C"
JNIEXPORT jobject JNICALL
Java_com_yaxiu_opencv_FaceDetection_opencvSaturationBrightnessContrast(JNIEnv *env, jobject thiz,
                                                                       jstring path) {
    MatFun matfun = MatFun();
    jobject bitmap = matfun.saturationBrightnessContrast(env, path);
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/io/FileNotFoundException");
        env->ThrowNew(no_such_clz,
                      "Unable to decode file: java.io.FileNotFoundException NoSuchFieldException open failed: ENOENT (No such file or directory)");
        env->DeleteLocalRef(no_such_clz);
        return nullptr;
    }
    LOGD("opencvSaturationBrightnessContrast success");
    return bitmap;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_loadBitmapEditPiexl(JNIEnv *env, jobject thiz, jobject bitmap,
                                                        jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun* fun){
        fun->run_task([](JNIEnv *env,JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            matfun.loadBitmapPixelEdit(env, bitmap);
            LOGD("loadBitmapEditPiexl success");
            executeJavaOnMainWithRelease(env, self, callback);

        });
        }, safeCallback);
    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }

}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_magnifierSpecialEffects(JNIEnv *env, jobject thiz,
                                                            jobject bitmap, jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self,jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            matfun.magnifierSpecialEffects(env, bitmap);
            LOGD("magnifierSpecialEffects success");
            executeJavaOnMainWithRelease(env, self, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_fishEyeSpecialEffects(JNIEnv *env, jobject thiz, jobject bitmap,
                                                          jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self,jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            matfun.fishEyeSpecialEffects(env, bitmap);
            LOGD("fishEyeSpecialEffects success");
            executeJavaOnMainWithRelease(env, self, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }

}

extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_oilPaintingSpecialEffects(JNIEnv *env, jobject thiz,
                                                              jobject bitmap, jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self,jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            matfun.oilPaintingSpecialEffects(env, bitmap);
            LOGD("oilPaintingSpecialEffects success");
            executeJavaOnMainWithRelease(env, self, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }

}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_glassSpecialEffects(JNIEnv *env, jobject thiz, jobject bitmap,
                                                        jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env,JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            matfun.glassSpecialEffects(env, bitmap);
            LOGD("glassSpecialEffects success");
            executeJavaOnMainWithRelease(env, self, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }

}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_inverseWorldSpecialEffects(JNIEnv *env, jobject thiz,
                                                               jobject bitmap, jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            matfun.inverseWorldSpecialEffects(env, bitmap);
            LOGD("inverseWorldSpecialEffects success");
            executeJavaOnMainWithRelease(env, self, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }

}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_mirrorSpecialEffects(JNIEnv *env, jobject thiz,
                                                         jobject bitmap, jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            matfun.mirrorSpecialEffects(env, bitmap);
            LOGD("mirrorSpecialEffects success");
            executeJavaOnMainWithRelease(env, self, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }

}


extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_mosaicSpecialEffects(JNIEnv *env, jobject thiz,
                                                         jobject bitmap, jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun){
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            matfun.mosaicSpecialEffects(env, bitmap);
            LOGD("mosaicSpecialEffects success");
            executeJavaOnMainWithRelease(env, self, callback);

        });
        }, safeCallback);
    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }


}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_reliefSpecialEffects(JNIEnv *env, jobject thiz,
                                                         jobject bitmap, jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            matfun.reliefSpecialEffects(env, bitmap);
            LOGD("reliefSpecialEffects success");
            executeJavaOnMainWithRelease(env, self, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }

}
jobject fileName;
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_faceMosaicSpecialEffects(JNIEnv *env, jobject thiz,
                                                             jstring file_name, jobject bitmap,
                                                             jobject callback) {
    fileName = env->NewWeakGlobalRef(file_name);
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task(
                [](JNIEnv *env, JniThreadFun *self, jobject &bitmap, jobject callback) -> void {
                    MatFun matfun = MatFun();
                    matfun.faceMosaicSpecialEffects(env,reinterpret_cast<jstring &>(fileName), bitmap);
                    LOGD("faceMosaicSpecialEffects success");
                    executeJavaOnMainWithRelease(env, self, callback);

                });

    }, safeCallback);
    thread.detach();


    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }

}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_clipSpecialEffects(JNIEnv *env, jobject thiz, jobject bitmap,
                                                       jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            jobject newBitmap = matfun.croppingBitmap(env, bitmap);
            LOGD("croppingBitmap success");
            executeJavaOnMainWithReleaseAndResult(env, self,newBitmap, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }

}

JniThreadFun* globleSafeCallback;
MatFun matfun;
float x_f,y_f;
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_moveMagnifierSpecialEffects(JNIEnv *env, jobject thiz, jfloat x,
                                                                jfloat y, jobject bitmap,
                                                                jobject callback) {
    x_f = x;
    y_f = y;
    if(!globleSafeCallback){
        globleSafeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
        LOGD("moveMagnifierSpecialEffects globleSafeCallback create");
         matfun = MatFun();
    }

    thread thread([](JniThreadFun *fun) {
        fun->run_task_globle([](JNIEnv *env,jobject &bitmap, jobject callback) -> void {

            matfun.magnifierSpecialEffects(env,x_f,y_f ,bitmap);
            LOGD("moveMagnifierSpecialEffects success");
            executeJavaOnMain(env, callback);

        });

    }, globleSafeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_rotateImage(JNIEnv *env, jobject thiz, jobject bitmap,
                                                jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            jobject bitmapRotate = matfun.rotateImage(env, bitmap);
            LOGD("rotateImage success");
            executeJavaOnMainWithReleaseAndResult(env, self,bitmapRotate, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_matrixTransform(JNIEnv *env, jobject thiz, jobject bitmap,
                                                    jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            matfun.matrixTransform(env, bitmap);
            LOGD("matrixTransform success");
            executeJavaOnMainWithRelease(env,self,callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_reSize(JNIEnv *env, jobject thiz, jobject bitmap,
                                           jobject runnable) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, runnable);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            jobject newBitmap = matfun.reSize(env, bitmap);
            LOGD("reSize success");
            executeJavaOnMainWithReleaseAndResult(env, self,newBitmap, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_remap(JNIEnv *env, jobject thiz, jobject bitmap,
                                          jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            matfun.remap(env, bitmap);
            LOGD("remap success");
            executeJavaOnMainWithRelease(env,self,callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_yaxiu_opencv_FaceDetection_nCrateMatIII(JNIEnv *env, jobject thiz, jint rows, jint cols, jint type) {
    Mat *m = new Mat(rows, cols, type);
    LOGD("DefineOpencvImpl nCrateMatIII");
    return reinterpret_cast<jlong>(m);
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_yaxiu_opencv_FaceDetection_nCrateMat(JNIEnv *env, jobject thiz) {
    Mat *m = new Mat();
    LOGD("DefineOpencvImpl nCrateMatIII");
    return reinterpret_cast<jlong>(m);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_nReleaseMat(JNIEnv *env, jobject thiz, jlong jni_mat_ads) {
    Mat *mat = reinterpret_cast<Mat *>(jni_mat_ads);
    delete mat;
    mat= nullptr;
    LOGD("DefineOpencvImpl delete mat");
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_nBitmap2mat(JNIEnv *env, jobject thiz, jlong jni_mat_ads,
                                                jobject bitmap) {
    Mat *mat = reinterpret_cast<Mat *>(jni_mat_ads);
    MatFun f = MatFun(); //注意是否需要初始化构造函数
    f.bitmap2mat(env, *mat, bitmap);
    LOGD("DefineOpencvImpl nBitmap2mat");

}

extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_nat(JNIEnv *env, jobject thiz, jlong jni_mat_ads, jint i0, jint i1,
                                        jfloat value) {
    Mat *mat = reinterpret_cast<Mat *>(jni_mat_ads);
    mat->at<float>(i0,i1)=value;
    LOGD("DefineOpencvImpl nat");

}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_nWarpAffine(JNIEnv *env, jobject thiz, jlong jni_mat_ads, jlong dst_mat_ads,
                                                jlong m_mat_ads, jobject bitmap) {
    Mat *mat = reinterpret_cast<Mat *>(jni_mat_ads);
    Mat *dst = reinterpret_cast<Mat *>(dst_mat_ads);
    Mat *m = reinterpret_cast<Mat *>(m_mat_ads);
    warpAffine(*mat, *dst, *m, mat->size(), INTER_LINEAR);
    MatFun().mat2bitmap(env, *dst, bitmap);
    float value=m->at<float>(0,0);
    LOGD("DefineOpencvImpl src cols=%d rows=%d dst cols=%d rows=%d nWarpAffine m=%f", mat->cols, mat->rows,mat->cols, mat->rows,value);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_equalizeHist(JNIEnv *env, jobject thiz, jobject bitmap,
                                                 jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            matfun.equalizeHist(env, bitmap);
            LOGD("equalizeHist success");
            executeJavaOnMainWithRelease(env,self,callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_calcuHist(JNIEnv *env, jobject thiz, jobject bitmap,
                                              jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            jobject newBitmap = matfun.calcuHist(env, bitmap);
            LOGD("calcuHist success");
            executeJavaOnMainWithReleaseAndResult(env, self,newBitmap, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_matLight(JNIEnv *env, jobject thiz, jobject bitmap,
                                             jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            matfun.matLight(env, bitmap);
            LOGD("matLight success");
            executeJavaOnMainWithRelease(env,self,callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }
}


extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_codeVerification(JNIEnv *env, jobject thiz, jobject bitmap,
                                                     jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            jobject newBitmap = matfun.codeVerification(env, bitmap);
            LOGD("codeVerification success");
            executeJavaOnMainWithReleaseAndResult(env, self,newBitmap, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_codeTiltVerification(JNIEnv *env, jobject thiz, jobject bitmap,
                                                         jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            jobject newBitmap = matfun.codeTiltVerification(env, bitmap);
            LOGD("codeVerification success");
            executeJavaOnMainWithReleaseAndResult(env, self,newBitmap, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_codeRoundVerification(JNIEnv *env, jobject thiz, jobject bitmap,
                                                          jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            jobject newBitmap = matfun.codeRoundVerification(env, bitmap);
            LOGD("codeVerification success");
            executeJavaOnMainWithReleaseAndResult(env, self,newBitmap, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_lbp(JNIEnv *env, jobject thiz, jobject bitmap,
                                        jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            jobject newBitmap = matfun.lbp(env, bitmap);
            LOGD("lbp success");
            executeJavaOnMainWithReleaseAndResult(env, self,newBitmap, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_hog(JNIEnv *env, jobject thiz, jobject bitmap,
                                        jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            matfun.hog(env, bitmap);
            LOGD("hog success");
            executeJavaOnMainWithRelease(env,self,callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }

}
extern "C"
JNIEXPORT void JNICALL
Java_com_yaxiu_opencv_FaceDetection_haar(JNIEnv *env, jobject thiz, jobject bitmap,
                                         jobject callback) {
    JniThreadFun *safeCallback = new JniThreadFun(g_vm, env, bitmap, callback);
    thread thread([](JniThreadFun *fun) {
        fun->run_task([](JNIEnv *env, JniThreadFun* self, jobject &bitmap, jobject callback) -> void {
            MatFun matfun = MatFun();
            jobject newBitmap = matfun.haar(env, bitmap);
            LOGD("haar success");
            executeJavaOnMainWithReleaseAndResult(env, self,newBitmap, callback);

        });

    }, safeCallback);

    thread.detach();
    jthrowable thorwable = env->ExceptionOccurred(); //jni 捕获异常
    if (thorwable) {
        env->ExceptionClear();//jni 清理异常
        jclass no_such_clz = env->FindClass("java/lang/Exception");
        env->ThrowNew(no_such_clz, "Exception occurred");
        env->DeleteLocalRef(no_such_clz);
    }
}