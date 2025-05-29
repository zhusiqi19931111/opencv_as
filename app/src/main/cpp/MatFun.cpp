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

void MatFun::matObj(JNIEnv *env, jstring &path) {

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

void MatFun::loadBitmapPixelEdit(JNIEnv *env, jobject &bitmap) {
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
                uchar gray = (b + 4 * g + 3 * r) / 10;
                mat.at<Vec4b>(i, j)[0] = gray;
                mat.at<Vec4b>(i, j)[1] = gray;
                mat.at<Vec4b>(i, j)[2] = gray;
                mat.at<Vec4b>(i, j)[3] = a;
            }
        }
    } else if (mat.channels() == 3) {
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

jobject MatFun::pixelEdit(JNIEnv *env, jstring &path) {

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
                // 好几种算法求灰度
                // uchar gery = (b + g + r) / 3;
                // 位移 > +- >  */  int > float
                uchar gray = (r + (g << 1) + b) >> 2; // 0.4  sobel  |x|+|y|
                // uchar gray = 0.11 * b + 0.59 * g + 0.3 * r;
                mat.at<Vec4b>(i, j)[0] = gray;
                mat.at<Vec4b>(i, j)[1] = gray;
                mat.at<Vec4b>(i, j)[2] = gray;
                mat.at<Vec4b>(i, j)[3] = a;
            }
        }
    } else if (mat.channels() == 3) {
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

jobject MatFun::imaAdd(JNIEnv *env, jstring &path1, jstring &path2) {
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
                mat3.at<Vec4b>(i, j)[0] = saturate_cast<uchar>(b);
                mat3.at<Vec4b>(i, j)[1] = saturate_cast<uchar>(g);
                mat3.at<Vec4b>(i, j)[2] = saturate_cast<uchar>(r);
                mat3.at<Vec4b>(i, j)[3] = saturate_cast<uchar>(a);
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
            } else {
                LOGE("imaAdd mat2Bitmap error ,chanel 对应的单个像素的类型未实现，请自行补充");
            }

        }
    }
    jobject resultBitmap = createBitmap(env, minCols, minRows, 5);
    mat2bitmap(env, mat3, resultBitmap);
    LOGI("imaAdd mat2Bitmap sucess");
    return resultBitmap;

}

jobject MatFun::saturationBrightnessContrast(JNIEnv *env, jstring &path) {
    jobject bitmap = createBitmapByDecodeFile(env, path);
    Mat mat;
    int result = bitmap2mat(env, mat, bitmap);
    if (result < 0) {
        LOGE("saturationBrightnessContrast bitmap2Mat error");
        return bitmap;
    }
    LOGI("saturationBrightnessContrast Mat type:%d channels:%d", mat.type(), mat.channels());
    float alpha = 1.2;
    int beta = 80;
    if (mat.channels() == 4) {
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                Vec4b pixel = mat.at<Vec4b>(i, j);
                uchar b = pixel[0];
                uchar g = pixel[1];
                uchar r = pixel[2];
                uchar a = pixel[3];
                mat.at<Vec4b>(i, j)[0] = saturate_cast<uchar>(alpha * b + beta);
                mat.at<Vec4b>(i, j)[1] = saturate_cast<uchar>(alpha * g + beta);
                mat.at<Vec4b>(i, j)[2] = saturate_cast<uchar>(alpha * r + beta);
                mat.at<Vec4b>(i, j)[3] = saturate_cast<uchar>(alpha * a + beta);
            }
        }
    } else if (mat.channels() == 3) {
        for (int i = 0; i < mat.rows; ++i) {
            for (int j = 0; j < mat.cols; ++j) {
                Vec3b pixel = mat.at<Vec3b>(i, j);
                uchar b = pixel[0];
                uchar g = pixel[1];
                uchar r = pixel[2];
                mat.at<Vec3b>(i, j)[0] = saturate_cast<uchar>(alpha * b + beta);
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
    // Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);   ALPHA_8, RGB_565, ARGB_4444, ARGB_8888, RGBA_F16, HARDWARE, RGBA_1010102
    int wid = width;
    int hei = height;
    char *name = "ARGB_8888";
    if (type == 1) {
        name = "ALPHA_8";
    } else if (type == 3) {
        name = "RGB_565";
    } else if (type == 4) {
        name = "ARGB_4444";
    } else if (type == 5) {
        name = "ARGB_8888";
    } else if (type == 6) {
        name = "RGBA_F16";
    } else if (type == 7) {
        name = "HARDWARE";
    } else if (type == 8) {
        name = "RGBA_1010102";
    } else if (type == CV_8UC1) {
        name = "ALPHA_8";
    } else if (type == CV_8UC2) {
        name = "ALPHA_8";
    } else if (type == CV_8UC3) {
        name = "RGB_565";
    } else if (type == CV_8UC4) {
        name = "ARGB_8888";
    }
    LOGE("createBitmap  width=%d height=%d type=%d name=%s", width, height, type, name);
    jclass configClass = env->FindClass("android/graphics/Bitmap$Config");

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
              bitmap_info.format == ANDROID_BITMAP_FORMAT_RGB_565 ||
              bitmap_info.format == ANDROID_BITMAP_FORMAT_A_8);
    int lockPixelsRes = AndroidBitmap_lockPixels(env, bitmap, &pixelPtr);
    if (lockPixelsRes < 0) {
        return lockPixelsRes;
    }
    LOGI("mat2Bitmap current format:%d type:%d height：%d  width：%d", bitmap_info.format, mat.type(),
         bitmap_info.height, bitmap_info.width);

    if (bitmap_info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {//对应CV_8UC4
        Mat temp = Mat(bitmap_info.height, bitmap_info.width, CV_8UC4, pixelPtr);
        if (mat.type() == CV_8UC1) {
            cvtColor(mat, temp, cv::COLOR_GRAY2RGBA);
        } else if (mat.type() == CV_8UC3) {
            cvtColor(mat, temp, cv::COLOR_RGB2RGBA);
        } else if (mat.type() == CV_8UC4) {
            mat.copyTo(temp);
        } else {
            LOGE("mat2Bitmap current type:%d unknown don't handler", mat.type());
        }

    } else if (bitmap_info.format == ANDROID_BITMAP_FORMAT_RGB_565) {//对应CV_8UC2
        Mat temp = Mat(bitmap_info.height, bitmap_info.width, CV_8UC2, pixelPtr);
        if (mat.type() == CV_8UC1) {
            cvtColor(mat, temp, cv::COLOR_GRAY2BGR565);
        } else if (mat.type() == CV_8UC3) {
            cvtColor(mat, temp, cv::COLOR_RGB2BGR565);
        } else if (mat.type() == CV_8UC4) {
            cvtColor(mat, temp, cv::COLOR_RGBA2BGR565);
        } else {
            LOGE("mat2Bitmap current type:%d unknown don't handler", mat.type());
        }

    } else {
        LOGE("mat2Bitmap current format:%d unknown don't handler", bitmap_info.format);
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    return 0;
}

/**
 * 浮雕特效
 */
void MatFun::reliefSpecialEffects(JNIEnv *env, jobject &bitmap) {
    Mat mat;
    int status = bitmap2mat(env, mat, bitmap);
    if (status < 0) {
        LOGE("mirrorSpecialEffects bitmap2mat error");
        return;
    }
    LOGE("mirrorSpecialEffects channels = %d", mat.channels());
    Mat reliefMat = Mat::zeros(mat.size(), mat.type());
    // 当前像素-下一个像素+ 128 会呈现浮雕特效有立体感，突出了轮廓信息，opencv  卷积
    uchar v = 128;
    if (mat.channels() == 4) {

        for (int rows = 0; rows < mat.rows - 1; ++rows) {
            for (int cols = 0; cols < mat.cols - 1; ++cols) {
                Vec4b pixels_p = mat.at<Vec4b>(rows, cols);
                Vec4b pixels_n = mat.at<Vec4b>(rows + 1, cols + 1);
                // bgra
                reliefMat.at<Vec4b>(rows, cols)[0] = saturate_cast<uchar>(
                        pixels_p[0] - pixels_n[0] + 128);
                reliefMat.at<Vec4b>(rows, cols)[1] = saturate_cast<uchar>(
                        pixels_p[1] - pixels_n[1] + 128);
                reliefMat.at<Vec4b>(rows, cols)[2] = saturate_cast<uchar>(
                        pixels_p[2] - pixels_n[2] + 128);
                reliefMat.at<Vec4b>(rows, cols)[3] = pixels_p[0];
            }
        }
    } else if (mat.channels() == 3) {

        for (int row = 0; row < mat.rows - 1; ++row) {
            for (int col = 0; col < mat.cols - 1; ++col) {
                Vec3b pixel_c = mat.at<Vec3b>(row, col);
                Vec3b pixel_n = mat.at<Vec3b>(row + 1, col + 1);
                reliefMat.at<Vec3b>(row, col)[0] = saturate_cast<uchar>(
                        pixel_c[0] + pixel_n[0] + v);
                reliefMat.at<Vec3b>(row, col)[1] = saturate_cast<uchar>(
                        pixel_c[1] + pixel_n[1] + v);
                reliefMat.at<Vec3b>(row, col)[2] = saturate_cast<uchar>(
                        pixel_c[2] + pixel_n[2] + v);
            }
        }
    } else if (mat.channels() == 1) {
        for (int row = 0; row < mat.rows - 1; ++row) {
            for (int col = 0; col < mat.cols - 1; ++col) {
                uchar pixel_c = mat.at<uchar>(row, col);
                uchar pixel_n = mat.at<uchar>(row + 1, col + 1);
                reliefMat.at<uchar>(row, col) = saturate_cast<uchar>(pixel_c + pixel_n + v);
            }
        }
    }

    mat2bitmap(env, reliefMat, bitmap);

}

/**
 * 马赛克特效
 */
void MatFun::mosaicSpecialEffects(JNIEnv *env, jobject &bitmap) {

    Mat mat;
    int status = bitmap2mat(env, mat, bitmap);
    if (status < 0) {
        LOGE("mirrorSpecialEffects bitmap2mat error");
        return;
    }
    LOGE("mirrorSpecialEffects channels = %d", mat.channels());
    //认定区域 进行马赛克处理
    // 获取图片宽高
    //1.指定马赛克区域
    //2.指定马赛克步长
    //3.取步长的像素值
    //4.循环遍历步长
    //5.将步骤3取得像数值赋值给步长+1 的每个像素
    //原理，相当区域内区使用外区的像素值
    int src_w = mat.cols;
    int src_h = mat.rows;
    int rows_start = src_h >> 2;
    int rows_end = src_h * 3 / 4;
    int cols_start = src_w >> 2;
    int cols_end = src_w * 3 / 4;
    int size = 10;

    for (int row = rows_start; row < rows_end; row += size) {
        for (int col = cols_start; col < cols_end; col += size) {
            int pixel_c = mat.at<int>(row, col);
            for (int m_rows = 1; m_rows < size; ++m_rows) {
                for (int m_cols = 1; m_cols < size; ++m_cols) {
                    mat.at<int>(row + m_rows, col + m_cols) = pixel_c;
                }

            }
        }
    }
    mat2bitmap(env, mat, bitmap);

}

/**
 *面部马赛克
 */
void MatFun::faceMosaicSpecialEffects(JNIEnv *env, jstring &filName, jobject &bitmap) {
    const char *path = env->GetStringUTFChars(filName, JNI_FALSE);
    bool result = cascadeClassifier.load(path);
    LOGE("加载分类器path%s", path);
    if (result) {
        LOGE("加载分类器文件成功");
        //1.bitmap 转 mat
        Mat src;
        bitmap2mat(env, src, bitmap);
        //2.mat 灰度化
        Mat gray;
        cvtColor(src, gray, COLOR_BGRA2GRAY);
        //直方图均衡化
        Mat hist;
        cv::equalizeHist(gray, hist);
        /****
         *InputArray image,
          CV_OUT std::vector<Rect>& objects,
          double scaleFactor = 1.1,
          int minNeighbors = 3, int flags = 0,
          Size minSize = Size(),
          Size maxSize = Size()
         */
        vector<Rect> faces;
        cascadeClassifier.detectMultiScale(hist, faces, 1.1, 5);
        if (faces.size() > 0) {
            //x, y,width,height
            Rect faceRect = faces[0];//面部所在的位置
            // 在人脸部分花个图
            // rectangle(src, faceRect, Scalar(255, 0, 0), 8);
            int x = faceRect.x;
            int y = faceRect.y;

            int src_w = faceRect.width;
            int src_h = faceRect.height;

            int rows_start = y;
            int rows_end = y + src_h;
            int cols_start = x;
            int cols_end = x + src_w;
            int blockSize = 10;
            LOGE("faceMosaicSpecialEffects x = %d y = %d width = %d height = %d",
                 x, y, src_w, src_h);
            LOGE("faceMosaicSpecialEffects rows_start = %d rows_end = %d cols_start = %d cols_end = %d",
                 rows_start, rows_end, cols_start, cols_end);
            //方式1
            for (int row = rows_start; row < rows_end; row += blockSize) {
                for (int col = cols_start; col < cols_end; col += blockSize) {
                    // 计算当前 block 实际宽高，防止越界
                    //方式1
                    /* int blockWidth = min(blockSize, cols_end- col);
                      int blockHeight = min(blockSize, rows_end - row);
                      int pixel_c = src.at<int>(row, col);//todo 特别注意马赛克必须在原图上取值绘制，不能在直方图上
                      for (int m_rows = 1; m_rows < blockWidth; ++m_rows) {
                          for (int m_cols = 1; m_cols < blockHeight; ++m_cols) {
                              src.at<int>(row + m_rows, col + m_cols) = pixel_c;
                          }

                      }*/
                    // 优化方式2：获取块内的平均颜色
                    Rect blockRect(col, row, blockSize, blockSize);
                    Mat block = src(blockRect);
                    Scalar avgColor = mean(block);

                    // 将整个块区域设置为平均颜色
                    rectangle(src, blockRect, avgColor, FILLED);
                }
            }
            //方式2
            /* RNG rng(time(NULL));
             for (int row = rows_start; row < rows_end - blockSize; ++row) {
                 for (int col = cols_start; col < cols_end - blockSize; ++col) {
                     int random = rng.uniform(0, blockSize);
                     src.at<int>(row, col) = src.at<int>(row + random, col + random);

                 }

             }*/
            mat2bitmap(env, src, bitmap);
        }


    } else {
        LOGE("加载分类器文件失败");
    }

    env->ReleaseStringUTFChars(filName, path);

}

/**
 * 镜像特效
 */
void MatFun::mirrorSpecialEffects(JNIEnv *env, jobject &bitmap) {

    Mat mat;
    int status = bitmap2mat(env, mat, bitmap);
    if (status < 0) {
        LOGE("mirrorSpecialEffects bitmap2mat error");
        return;
    }

    int random_num = Random().random_start_end(0, 2);
    LOGE("mirrorSpecialEffects channels = %d random_num = %d", mat.channels(), random_num);
    Mat mirrorMat = Mat::zeros(mat.size(), mat.type());

    if (random_num == 0) {
        if (mat.channels() == 4) {
            for (int row = 0; row < mat.rows; ++row) {//rows 代表高度
                for (int col = 0; col < mat.cols; ++col) {//cols 代表宽度
                    //1.左右镜像
                    Vec4b v4 = mat.at<Vec4b>(row, col);
                    mirrorMat.at<Vec4b>(row, mat.cols - col - 1) = v4;

                }
            }
        } else if (mat.channels() == 3) {
            //1.左右镜像
            for (int row = 0; row < mat.rows; ++row) {//rows 代表高度
                for (int col = 0; col < mat.cols; ++col) {//cols 代表宽度
                    //1.左右镜像
                    Vec3b v3 = mat.at<Vec3b>(row, col);
                    mirrorMat.at<Vec3b>(row, mat.cols - col - 1) = v3;

                }
            }
        } else if (mat.channels() == 1) {
            //1.左右镜像
            for (int row = 0; row < mat.rows; ++row) {//rows 代表高度
                for (int col = 0; col < mat.cols; ++col) {//cols 代表宽度
                    //1.左右镜像
                    uchar u = mat.at<uchar>(row, col);
                    mirrorMat.at<uchar>(row, mat.cols - col - 1) = u;
                }
            }
        } else {
            LOGE("mirrorSpecialEffects channels not support");
        }

    } else if (random_num == 1) {
        if (mat.channels() == 4) {
            for (int row = 0; row < mat.rows; ++row) {
                for (int col = 0; col < mat.cols; ++col) {
                    //2.上下镜像
                    Vec4b v4 = mat.at<Vec4b>(row, col);
                    mirrorMat.at<Vec4b>(mat.rows - row - 1, col) = v4;
                }
            }
        } else if (mat.channels() == 2) {
            for (int row = 0; row < mat.rows; ++row) {
                for (int col = 0; col < mat.cols; ++col) {
                    //2.上下镜像
                    Vec3b v3 = mat.at<Vec3b>(row, col);
                    mirrorMat.at<Vec3b>(mat.rows - row - 1, col) = v3;
                }
            }
        } else if (mat.channels() == 1) {
            for (int row = 0; row < mat.rows; ++row) {
                for (int col = 0; col < mat.cols; ++col) {
                    //2.上下镜像
                    uchar u = mat.at<uchar>(row, col);
                    mirrorMat.at<uchar>(mat.rows - row - 1, col) = u;
                }
            }
        } else {
            LOGE("mirrorSpecialEffects channels not support");
        }

    } else {
        if (mat.channels() == 4) {
            for (int row = 0; row < mat.rows; ++row) {
                for (int col = 0; col < mat.cols; ++col) {
                    //3.左右上下镜像
                    Vec4b v4 = mat.at<Vec4b>(row, col);
                    mirrorMat.at<Vec4b>(mat.rows - row - 1, mat.cols - col - 1) = v4;
                }
            }
        } else if (mat.channels() == 2) {
            for (int row = 0; row < mat.rows; ++row) {
                for (int col = 0; col < mat.cols; ++col) {
                    //3.左右上下镜像
                    Vec3b v3 = mat.at<Vec3b>(row, col);
                    mirrorMat.at<Vec3b>(mat.rows - row - 1, mat.cols - col - 1) = v3;

                }
            }
        } else if (mat.channels() == 1) {
            for (int row = 0; row < mat.rows; ++row) {
                for (int col = 0; col < mat.cols; ++col) {
                    //3.左右上下镜像
                    uchar u = mat.at<uchar>(row, col);
                    mirrorMat.at<uchar>(mat.rows - row - 1, mat.cols - col - 1) = u;
                }
            }
        } else {
            LOGE("mirrorSpecialEffects channels not support");
        }

    }
    mat2bitmap(env, mirrorMat, bitmap);

}

/**
 * 逆世界特效
 */
void MatFun::inverseWorldSpecialEffects(JNIEnv *env, jobject &bitmap) {
    Mat mat;
    int status = bitmap2mat(env, mat, bitmap);
    if (status < 0) {
        LOGE("mirrinverseWorldSpecialEffectsorSpecialEffects bitmap2mat error");
        return;
    }
    //1.将图片高度平均分成4等分
    //2.取中间2/4 的像素
    //3.将所取的像素放置在新Mat 中间-底部 区域
    //4.将所取像素翻转之后，放置在新Mat 的顶部-中间 区域
    Mat matInverse = Mat::zeros(mat.size(), mat.type());
    int src_w = mat.cols;
    int src_h = mat.rows;

    int mid_h = src_h >> 1;
    int start_h = mid_h >> 1;

    if (mat.channels() == 4) {
        for (int row = 0; row < mid_h; ++row) {
            for (int col = 0; col < mat.cols; ++col) {
                // 4 rgba  , 判断 type()
                matInverse.at<Vec4b>(row + mid_h, col) = mat.at<Vec4b>(row + start_h, col);

            }

        }
        for (int row = 0; row < mid_h; ++row) {
            for (int col = 0; col < mat.cols; ++col) {
                matInverse.at<Vec4b>(row, col) = mat.at<Vec4b>(mat.rows - start_h - row, col);

            }

        }
    } else if (mat.channels() == 3) {
        for (int row = 0; row < mid_h; ++row) {
            for (int col = 0; col < mat.cols; ++col) {
                // 4 rgba  , 判断 type()
                matInverse.at<Vec3b>(row + mid_h, col) = mat.at<Vec3b>(row + start_h, col);

            }

        }
        for (int row = 0; row < mid_h; ++row) {
            for (int col = 0; col < mat.cols; ++col) {
                matInverse.at<Vec3b>(row, col) = mat.at<Vec3b>(mat.rows - start_h - row, col);

            }

        }
    } else if (mat.channels() == 1) {
        for (int row = 0; row < mid_h; ++row) {
            for (int col = 0; col < mat.cols; ++col) {
                // 4 rgba  , 判断 type()
                matInverse.at<uchar>(row + mid_h, col) = mat.at<uchar>(row + start_h, col);

            }

        }
        for (int row = 0; row < mid_h; ++row) {
            for (int col = 0; col < mat.cols; ++col) {
                matInverse.at<uchar>(row, col) = mat.at<uchar>(mat.rows - start_h - row, col);

            }

        }
    }


    mat2bitmap(env, matInverse, bitmap);
}

/**
 * 毛玻璃特效
 */
void MatFun::glassSpecialEffects(JNIEnv *env, jobject &bitmap) {
    Mat mat;
    bitmap2mat(env, mat, bitmap);
    // 高斯模糊，毛玻璃 （对某个区域取随机像素）
    int src_w = mat.cols;
    int src_h = mat.rows;
    int size = 25;
    RNG rng(time(NULL));
    for (int row = 0; row < src_h - size; ++row) {
        for (int col = 0; col < src_w - size; ++col) {
            int random = rng.uniform(0, size);
            mat.at<int>(row, col) = mat.at<int>(row + random, col + random);
        }

    }
    mat2bitmap(env, mat, bitmap);
}

void MatFun::oilPaintingSpecialEffects2(JNIEnv *env, jobject &bitmap) {
    // 创建输出图像
    Mat mat;
    bitmap2mat(env, mat, bitmap);

    // 不进行图像缩放，保持原始图像尺寸
    // resize(mat, mat, Size(), 0.5, 0.5);

    int brushSize = 15;  // 增加笔触大小
    int coarseness = 1;  // 减少颜色量化次数

    // 创建临时图像
    Mat temp;
    mat.copyTo(temp);

    // 1. 颜色量化
    for (int i = 0; i < coarseness; i++) {
        pyrDown(temp, temp);
        pyrUp(temp, temp);
    }

    // 2. 添加笔触效果
    RNG rng(getTickCount());
    for (int y = 0; y < temp.rows; y += brushSize) {
        for (int x = 0; x < temp.cols; x += brushSize) {
            int radius = brushSize / 2 + rng.uniform(-brushSize / 4, brushSize / 4);
            Point center(x + rng.uniform(-brushSize / 4, brushSize / 4),
                         y + rng.uniform(-brushSize / 4, brushSize / 4));

            // 获取区域颜色
            Rect roi(max(0, center.x - radius), max(0, center.y - radius),
                     min(2 * radius, temp.cols - center.x + radius),
                     min(2 * radius, temp.rows - center.y + radius));
            if (roi.width <= 0 || roi.height <= 0) continue;

            Mat region = temp(roi);
            Scalar meanColor = mean(region);

            // 绘制椭圆笔触
            ellipse(temp, center, Size(radius, radius / 2 + rng.uniform(0, radius / 2)),
                    rng.uniform(0, 360), 0, 360,
                    meanColor, FILLED, LINE_AA);
        }
    }

    // 3. 边缘增强
    Mat edges;
    Canny(mat, edges, 50, 150);
    // 方式1：确保 edges 是单通道图像
//    if (edges.channels() != 1) {
//        cvtColor(edges, edges, COLOR_BGR2GRAY);  // 如果不是单通道，转换为灰度图
//    }
//    //这个错误提示表明在调用 subtract(temp, edges * 0.3, temp) 时，temp 和 edges 图像的尺寸或通道数不匹配，导致无法进行逐像素的减法运算。为了避免这个错误，我们需要确保两个输入图像 temp 和 edges 的大小和通道数相同。
//    // 必须将将 edges 转换为三通道图像，否则subtract 会报以上错误
//    Mat edgesBGR;
//
//    cvtColor(edges, edgesBGR, COLOR_GRAY2BGR);  // 将单通道转换为三通道
    //方式二： 将 edges 扩展为三通道
    Mat edgesBGR;
    Mat temp_edges = edges.clone();  // 复制一份边缘图
    cv::merge(std::vector<Mat>{temp_edges, temp_edges, temp_edges}, edgesBGR);  // 将单通道扩展为三通道

    // 边缘增强：减去边缘图像的影响
    //subtract(temp, edgesBGR * 0.3, temp);  // 使用三通道的 edgesBGR 进行减法


    // 4. 纹理增强
    Mat texture;
    Mat kernel = (Mat_<float>(3, 3) << 1, 1, 1, 1, -8, 1, 1, 1, 1);
    filter2D(temp, texture, CV_32F, kernel);
    texture.convertTo(texture, CV_8U);
    add(temp, texture * 0.5, temp); // 使用三通道的 edgesBGR 进行减法 增加纹理增强的强度

    mat2bitmap(env, temp, bitmap);
}

/**
 * 油画特效
 *  // 油画基于直方统计
    // 1. 每个点需要分成 n*n 小块
    // 2. 统计灰度等级
    // 3. 选择灰度等级中最多的值
    // 4. 找到所有的像素取平均值
     Mat src;
     bitmap2mat(env,src, bitmap);
     Mat gary;
     cvtColor(src, gary, COLOR_BGRA2GRAY);
     Mat res(src.size(), src.type());

     int src_w = src.cols;
     int src_h = src.rows;
     int size = 8;
     // 知识不是用来背的 20% ，用来唤醒大家的
     for (int rows = 0; rows < src_h - size; ++rows) {
         for (int cols = 0; cols < src_w - size; ++cols) {
             int g[8] = {0}, b_g[8] = {0}, g_g[8] = {0}, r_g[8] = {0};// 255/8
             // 这个位置  64 循环 -> 1 个像素点 ， 高斯模糊 ，想想怎么优化
             for (int o_rows = 0; o_rows < size; ++o_rows) {
                 for (int o_cols = 0; o_cols < size; ++o_cols) {
                     uchar gery = gary.at<uchar>(rows + o_rows, cols + o_cols);
                     uchar index = gery / (254 / 7); // 254*8/254
                     g[index] += 1;
                     // 等级的像素值之和
                     b_g[index] += src.at<Vec4b>(rows + o_rows, cols + o_cols)[0];
                     g_g[index] += src.at<Vec4b>(rows + o_rows, cols + o_cols)[1];
                     r_g[index] += src.at<Vec4b>(rows + o_rows, cols + o_cols)[2];
                 }
             }
             // 最大的角标找出来
             int max_index = 0;
             int max = g[0];
             for (int i = 1; i < size; ++i) {
                 if (g[max_index] < g[i]) {
                     max_index = i;
                     max = g[i];
                 }
             }
             // 会超过 255 ， 会不会超过，超过了会怎样 （头条面试）  截取掉最高位
             res.at<Vec4b>(rows, cols)[0] = b_g[max_index] / max;
             res.at<Vec4b>(rows, cols)[1] = g_g[max_index] / max;
             res.at<Vec4b>(rows, cols)[2] = r_g[max_index] / max;
         }
     }
    mat2bitmap(env, res, bitmap);
 */
void MatFun::oilPaintingSpecialEffects(JNIEnv *env, jobject &bitmap) {
    oilPaintingSpecialEffects2(env, bitmap);
}

/**
 * bitmap 裁剪
 */
jobject MatFun::croppingBitmap(JNIEnv *env, jobject &bitmap) {
    Mat mat;
    bitmap2mat(env, mat, bitmap);
    Mat resizeMat;
    resize(mat, resizeMat, Size(mat.cols >> 1, mat.rows >> 1));


    jobject bitmap_clip = createBitmap(env, mat.cols >> 1, mat.rows >> 1, mat.type());

    LOGE("croppingBitmap mat cols:%d rows:%d resizeMat cols:%d rows:%d bitmap_clip", mat.cols,
         mat.rows,
         resizeMat.cols, resizeMat.rows);
    mat2bitmap(env, resizeMat, bitmap_clip);
    return bitmap_clip;
}

/**
 * 鱼眼特效
 * @param env
 * @param bitmap
 */
void MatFun::fishEyeSpecialEffects(JNIEnv *env, jobject &bitmap) {
    Mat mat;
    bitmap2mat(env, mat, bitmap);

    // 不进行缩放，保持原始图像尺寸
    // resize(mat, mat, Size(mat.cols >> 1, mat.rows >> 1));

    // 应用鱼眼效果
    float intensity = 0.7f; // 变形强度 (0.0-1.0)

    Mat mapx, mapy;
    Size size = mat.size();
    mapx.create(size, CV_32F);
    mapy.create(size, CV_32F);

    const float centerX = size.width / 2.0f;
    const float centerY = size.height / 2.0f;
    const float maxRadius = sqrt(centerX * centerX + centerY * centerY);

    // 预计算角度和半径的查找表
    Mat dx = Mat::zeros(size, CV_32F);
    Mat dy = Mat::zeros(size, CV_32F);
    Mat r = Mat::zeros(size, CV_32F);
    Mat theta = Mat::zeros(size, CV_32F);

    // 第一阶段：并行计算dx, dy, r, theta
    parallel_for_(Range(0, size.height), [&](const Range &range) {
        for (int y = range.start; y < range.end; y++) {
            for (int x = 0; x < size.width; x++) {
                dx.at<float>(y, x) = x - centerX;
                dy.at<float>(y, x) = y - centerY;
                float _dx = dx.at<float>(y, x);
                float _dy = dy.at<float>(y, x);
                r.at<float>(y, x) = sqrt(_dx * _dx + _dy * _dy);
                theta.at<float>(y, x) = atan2(_dy, _dx);
            }
        }
    });

    // 第二阶段：并行计算映射
    parallel_for_(Range(0, size.height), [&](const Range &range) {
        for (int y = range.start; y < range.end; y++) {
            for (int x = 0; x < size.width; x++) {
                float newR = pow(r.at<float>(y, x) / maxRadius, 1.0f + intensity) * maxRadius;
                mapx.at<float>(y, x) = newR * cos(theta.at<float>(y, x)) + centerX;
                mapy.at<float>(y, x) = newR * sin(theta.at<float>(y, x)) + centerY;
            }
        }
    });

    Mat outMat;
    cv::remap(mat, outMat, mapx, mapy, INTER_LINEAR, BORDER_CONSTANT, Scalar(0, 0, 0));

    // 添加渐晕效果
    float vignetteStrength = 0.0f;  // 暂时禁用渐晕
    Mat vignette = Mat::zeros(outMat.size(), CV_32F);
    Point center(mat.cols / 2, outMat.rows / 2);
    float radius = min(center.x, center.y) * 0.9f;

    for (int y = 0; y < outMat.rows; y++) {
        for (int x = 0; x < outMat.cols; x++) {
            float distance = norm(Point(x, y) - center);
            float factor = 1.0f - vignetteStrength * pow(distance / radius, 2);
            factor = max(0.0f, min(1.0f, factor));

            if (outMat.channels() == 3) {
                outMat.at<Vec3b>(y, x) = outMat.at<Vec3b>(y, x) * factor;
            } else {
                outMat.at<uchar>(y, x) = outMat.at<uchar>(y, x) * factor;
            }
        }
    }

    mat2bitmap(env, outMat, bitmap);  // 更新 bitmap
}

void *getMouseCallbackUserData() {
    // 假设你有一个全局变量存储了用户数据
    static void *userData = nullptr;
    return userData;
}

void advancedMagnifierEffectMy(JNIEnv *env, jobject bitmap, Mat &src, Mat &dst, Point center,
                               int radius, float magnification,
                               bool lensEffect = true, bool fishEye = true) {
    src.copyTo(dst);

    // 参数检查
    if (radius <= 0 || magnification <= 1.0f) return;
    LOGE("advancedMagnifierEffect start lensEffect:%b fishEye:%b", lensEffect, fishEye);
    // 确保中心在图像范围内
    center.x = max(radius, min(src.cols - radius, center.x));
    center.y = max(radius, min(src.rows - radius, center.y));
    LOGE("advancedMagnifierEffect x%d y%d", center.x, center.y);
    // 创建放大区域蒙版
    Mat mask = Mat::zeros(src.size(), CV_8UC1);
    circle(mask, center, radius, Scalar(255), -1);

    // 创建映射矩阵
    Mat map_x(src.size(), CV_32FC1);
    Mat map_y(src.size(), CV_32FC1);

    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            // 计算到中心的距离
            float dx = x - center.x;
            float dy = y - center.y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance <= radius) {
                if (fishEye) {
                    // 鱼眼变形效果
                    float r = distance / radius;
                    float theta = atan2(dy, dx);
                    float newR = pow(r, 0.8f) * radius;

                    map_x.at<float>(y, x) = center.x + newR * cos(theta);
                    map_y.at<float>(y, x) = center.y + newR * sin(theta);
                } else {
                    // 简单放大
                    map_x.at<float>(y, x) = center.x + (x - center.x) / magnification;
                    map_y.at<float>(y, x) = center.y + (y - center.y) / magnification;
                }
            } else {
                map_x.at<float>(y, x) = x;
                map_y.at<float>(y, x) = y;
            }
        }
    }

    // 应用重映射
    cv::remap(src, dst, map_x, map_y, INTER_LINEAR, BORDER_REPLICATE);
    LOGE("advancedMagnifierEffect remap");
    if (lensEffect) {
        // 添加透镜效果
        Mat lensEffect = Mat::zeros(src.size(), src.type());
        circle(lensEffect, center, radius, Scalar(255, 255, 255), -1);
        GaussianBlur(lensEffect, lensEffect, Size(25, 25), 0);
        addWeighted(dst, 1.0, lensEffect, 0.1, 0, dst);

        // 添加边框
        circle(dst, center, radius, Scalar(255, 255, 255), 3);
        circle(dst, center, radius - 2, Scalar(0, 0, 0), 1);
        LOGE("advancedMagnifierEffect circle");

    }
}

/**
 * 放大镜特效
 * @param env
 * @param bitmap
 */
void MatFun::magnifierSpecialEffects(JNIEnv *env, jobject &bitmap) {
    Mat mat;
    bitmap2mat(env, mat, bitmap);


    // 参数
    int radius = 100;
    float magnification = 2.0f;
    Mat dst;
    // 初始显示
    advancedMagnifierEffect(env, bitmap, mat, dst, Point(0, 0), radius,
                            magnification,
                            true, false);
    mat2bitmap(env, dst, bitmap);


}


void MatFun::magnifierSpecialEffects(JNIEnv *env, jfloat x, jfloat y, jobject &bitmap) {

    Mat mat;
    bitmap2mat(env, mat, bitmap);

    // 参数
    int radius = 100;
    float magnification = 2.0f;
    Mat dst;

    int pointx = x;
    int pointy = y;

    // 初始显示
    advancedMagnifierEffect(env, bitmap, mat, dst, Point(pointx, pointy), radius,
                            magnification,
                            true, false);
    mat2bitmap(env, dst, bitmap);
}

void MatFun::advancedMagnifierEffect(JNIEnv *env, jobject bitmap, Mat &src, Mat &dst, Point center,
                                     int radius, float magnification, bool lensEffect = true,
                                     bool fishEye = true) {

    // 参数检查
    if (radius <= 0 || magnification <= 1.0f) return;

    LOGE("advancedMagnifierEffect start lensEffect:%b fishEye:%b", lensEffect, fishEye);

    // 确保中心在图像范围内
    center.x = max(radius, min(src.cols - radius, center.x));
    center.y = max(radius, min(src.rows - radius, center.y));
    LOGE("Updated center: x=%d, y=%d", center.x, center.y);

    // 创建放大区域蒙版
    Mat mask = Mat::zeros(src.size(), CV_8UC1);
    circle(mask, center, radius, Scalar(255), -1);

    // 创建映射矩阵
    Mat map_x(src.size(), CV_32FC1);
    Mat map_y(src.size(), CV_32FC1);

    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            // 计算到中心的距离
            float dx = x - center.x;
            float dy = y - center.y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance <= radius) {
                if (fishEye) {
                    // 鱼眼变形效果
                    float r = distance / radius;
                    float theta = atan2(dy, dx);
                    float newR = pow(r, 0.8f) * radius;

                    map_x.at<float>(y, x) = center.x + newR * cos(theta);
                    map_y.at<float>(y, x) = center.y + newR * sin(theta);
                } else {
                    // 简单放大
                    map_x.at<float>(y, x) = center.x + (x - center.x) / magnification;
                    map_y.at<float>(y, x) = center.y + (y - center.y) / magnification;
                }
            } else {
                map_x.at<float>(y, x) = x;
                map_y.at<float>(y, x) = y;
            }
        }
    }

    // 应用重映射
    cv::remap(src, dst, map_x, map_y, INTER_LINEAR, BORDER_REPLICATE);
    LOGE("advancedMagnifierEffect remap");

    if (lensEffect) {
        // 添加透镜效果
        Mat lensEffect = Mat::zeros(src.size(), src.type());
        circle(lensEffect, center, radius, Scalar(255, 255, 255), -1);
        GaussianBlur(lensEffect, lensEffect, Size(25, 25), 0);
        addWeighted(dst, 1.0, lensEffect, 0.1, 0, dst);

        // 添加边框
        circle(dst, center, radius, Scalar(255, 255, 255), 3);
        circle(dst, center, radius - 2, Scalar(0, 0, 0), 1);
        LOGE("advancedMagnifierEffect circle");
    }


}


jobject MatFun::rotateImage(JNIEnv *env, jobject &bitmap) {
    Mat mat;
    bitmap2mat(env, mat, bitmap);
    int chanel = mat.channels();
    LOGE("rotateImage chanel=%d type=%d CV_8UC0=%d CV_8UC1=%d CV_8UC2=%d CV_8UC3=%d CV_8UC4=%d width=%d height=%d",
         chanel, mat.type(), CV_8U, CV_8UC1, CV_8UC2, CV_8UC3, CV_8UC4, mat.cols, mat.rows);
    int random = Random().random_start_end(0, 4);

    if (random == 0) {//顺时针旋转90 度
        int res_w = mat.rows;
        int res_h = mat.cols;
        Mat des = Mat::zeros(res_h, res_w, mat.type());
        for (int row = 0; row < res_h; ++row) {//rows高度 相当于 Y 轴
            for (int col = 0; col < res_w; ++col) {//cols 宽度 相当于 x 轴
                int src_row = res_w - col - 1;
                int src_col = row;
                des.at<int>(row, col) = mat.at<int>(src_row, src_col);

            }
        }
        //bitmap 宽高被调换
        jobject rotateBitmap = createBitmap(env, res_w, res_h, des.type());
        mat2bitmap(env, des, rotateBitmap);

        LOGE("顺时针旋转90 度 width=%d height=%d", des.cols, des.rows);
        return rotateBitmap;

    } else if (random == 1) {//逆时针旋转90 度
        int res_w = mat.rows;
        int res_h = mat.cols;
        Mat des = Mat::zeros(res_h, res_w, mat.type());
        for (int row = 0; row < res_h; ++row) {//rows高度 相当于 Y 轴
            for (int col = 0; col < res_w; ++col) {//cols 宽度 相当于 x 轴
                int src_row = col;
                int src_col = res_h - row - 1;
                des.at<int>(row, col) = mat.at<int>(src_row, src_col);

            }
        }
        //bitmap 宽高被调换
        jobject rotateBitmap = createBitmap(env, res_w, res_h, des.type());
        mat2bitmap(env, des, rotateBitmap);
        LOGE("逆时针旋转90 度 width=%d height=%d", des.cols, des.rows);
        return rotateBitmap;
    } else if (random == 2) {//上下翻转180 度 ,x(cols) 上下对调，y(rows) 不变
        Mat des = Mat::zeros(mat.size(), mat.type());
        for (int row = 0; row < mat.rows; ++row) {//rows高度 相当于 Y 轴
            for (int col = 0; col < mat.cols; ++col) {//cols 宽度 相当于 x 轴
                des.at<int>(row, col) = mat.at<int>(mat.rows - row - 1, col);

            }
        }
        mat2bitmap(env, des, bitmap);
        LOGE("上下翻转180 度 , y(rows)上下对调，x(cols) 左右不变");
        return bitmap;
    } else if (random == 3) {//左右翻转180 度 ，x(cols) 不变，y(rows) 左右对调
        Mat des = Mat::zeros(mat.size(), mat.type());
        for (int row = 0; row < mat.rows; ++row) {//rows高度 相当于 Y 轴
            for (int col = 0; col < mat.cols; ++col) {//cols 宽度 相当于 x 轴
                des.at<int>(row, col) = mat.at<int>(row, mat.cols - col - 1);

            }
        }
        mat2bitmap(env, des, bitmap);
        LOGE("左右翻转180 度 ，y(rows) 上下不变，x(cols)左右对调");
        return bitmap;
    } else if (random == 4) {//上下左右翻转
        Mat des = Mat::zeros(mat.size(), mat.type());
        for (int row = 0; row < mat.rows; ++row) {//rows高度 相当于 Y 轴
            for (int col = 0; col < mat.cols; ++col) {//cols 宽度 相当于 x 轴
                des.at<int>(row, col) = mat.at<int>(mat.rows - row - 1, mat.cols - col - 1);

            }
        }
        mat2bitmap(env, des, bitmap);
        LOGE("上下左右翻转");
        return bitmap;
    }

    return bitmap;
}

/**
 *  Point2f 和 Mat 在 OpenCV 中各有用途：
    Point2f 适合表示和操作单个点
    Mat 适合存储和处理点集或进行矩阵运算
    getAffineTransform 等函数通过接受 Point2f 数组并返回 Mat 矩阵，实现了两者之间的桥梁作用
 * @param env
 * @param bitmap
 */
void MatFun::matrixTransform(JNIEnv *env, jobject &bitmap) {
    Mat mat;
    bitmap2mat(env, mat, bitmap);

    Mat M = Mat(2, 3, CV_32FC1); //2*3 矩阵

    // 这几个值应该怎么确定？
    // [a0,a1,a2]     两个矩阵      [a0,a1]    [a2]       =     [ x ]     *     [a0,a1]    +   [a2]    =   a0*x+a1*y + a2
    // [b0,b1,b2]                  [b0,b1]    [b2]             [ y ]           [b0,b1]        [b2]    =   b0*x+b1*y + b2
    //原图
    M.at<float>(0, 0) = 1; //a0
    M.at<float>(0, 1) = 0; //a1
    M.at<float>(0, 2) = 0; //a2

    M.at<float>(1, 0) = 0; //b0
    M.at<float>(1, 1) = 1; //b1
    M.at<float>(1, 2) = 0; //b2
    Mat M1 = Mat(2, 3, CV_32FC1); //2*3 矩阵
    //位移10个像素
    M1.at<float>(0, 0) = 1; //a0
    M1.at<float>(0, 1) = 0; //a1
    M1.at<float>(0, 2) = 10; //a2

    M1.at<float>(1, 0) = 0; //b0
    M1.at<float>(1, 1) = 1; //b1
    M1.at<float>(1, 2) = 10; //b2

    Mat M2 = Mat(2, 3, CV_32FC1); //2*3 矩阵
    //缩小0.5
    M2.at<float>(0, 0) = 0.5; //a0
    M2.at<float>(0, 1) = 0; //a1
    M2.at<float>(0, 2) = 0; //a2

    M2.at<float>(1, 0) = 0; //b0
    M2.at<float>(1, 1) = 0.5; //b1
    M2.at<float>(1, 2) = 0; //b2

    Mat M3 = Mat(2, 3, CV_32FC1); //2*3 矩阵
    //放大1.5
    M3.at<float>(0, 0) = 1.5; //a0
    M3.at<float>(0, 1) = 0; //a1
    M3.at<float>(0, 2) = 0; //a2

    M3.at<float>(1, 0) = 0; //b0
    M3.at<float>(1, 1) = 1.5; //b1
    M3.at<float>(1, 2) = 0; //b2

    Mat M4 = Mat(2, 3, CV_32FC1); //2*3 矩阵
    //y轴旋转0.8，x轴旋转0.5
    M4.at<float>(0, 0) = 1; //a0
    M4.at<float>(0, 1) = 0.8; //a1
    M4.at<float>(0, 2) = 0; //a2

    M4.at<float>(1, 0) = 0.5; //b0
    M4.at<float>(1, 1) = 1; //b1
    M4.at<float>(1, 2) = 0; //b2
    Mat dst;
    //方式一 仿射变换
    //warpAffine(mat, dst,  M4, mat.size(), INTER_LINEAR);
    //系统 仿射变换
    //原理如下：
    /*旋转 (线性变换)
    平移 (向量加)
    缩放操作 (线性变换)*/
    Point2f srcTri[3];
    Point2f dstTri[3];
    /**
     * x' = a11 * x + a12 * y + b1
       y' = a21 * x + a22 * y + b2
     */
    // 设置源点
    srcTri[0] = Point2f(0, 0);
    srcTri[1] = Point2f(100, 0);
    srcTri[2] = Point2f(0, 100);

    int random = Random().random_start_end(0, 3);
    LOGE("matrixTransform random=%d", random);
    if (random == 0) {
        //平移向右 100 像素，向下 50 像素 .每个点都加上 (100, 50)，表示向右下平移。
        dstTri[0] = Point2f(100, 50);
        dstTri[1] = Point2f(200, 50);
        dstTri[2] = Point2f(100, 150);
        LOGE("平移向右 100 像素，向下 50 像素d");
    } else if (random == 1) {

        //以某点为中心，进行逆时针旋转 θ 度（OpenCV 默认角度是逆时针）。绕原点旋转 90 度（逆时针）
        dstTri[0] = Point2f(50, 50);
        dstTri[1] = Point2f(0, 100);
        dstTri[2] = Point2f(-100, 0);
        LOGE("绕原点旋转 90 度（逆时针");
    } else if (random == 2) {
        //以原点为中心，放大 2 倍，就是每个点乘了 2
        dstTri[0] = Point2f(0, 0);
        dstTri[1] = Point2f(200, 0);
        dstTri[2] = Point2f(0, 200);
        LOGE("以原点为中心，放大 2 倍");
    } else if (random == 3) {
        //剪切是在保持某一方向不变的情况下，斜着拉伸图像。：X 方向剪切，shear_x = 0.5
        dstTri[0] = Point2f(0, 0);
        dstTri[1] = Point2f(100, 0);
        dstTri[2] = Point2f(50, 100);
        LOGE("X 方向剪切，shear_x = 0.5");
    }

    //方式二 仿射变换
    //Mat warpMat = getAffineTransform(srcTri, dstTri);
    //方式三
    Point2f center = Point2f(mat.cols >> 1, mat.rows >> 1);
    double angle = 90;
    double scale = 1;
    Mat warpMat = getRotationMatrix2D(center, angle, scale);
    warpAffine(mat, dst, M2, mat.size(), INTER_LINEAR);
    mat2bitmap(env, dst, bitmap);
}

jobject MatFun::reSize(JNIEnv *env, jobject &bitmap) {
    Mat mat;
    bitmap2mat(env, mat, bitmap);
    int random = Random().random_start_end(0, 1);
    Mat dst;
    Size dstsize;
    if (random == 0) {
        LOGE("scale = 2f");
        //方式1放大
        dstsize = Size(mat.cols * 2, mat.rows * 2);
        // pyrUp(mat, dst, dstsize);
    } else {
        LOGE("scale = 0.5f");
        //方式1缩小
        dstsize = Size(mat.cols / 2, mat.rows / 2);
        // pyrDown(mat, dst, dstsize);
    }
    //方式2 仿拉普拉斯金字塔 实现上采样降采样
    ImageProc().pyrUpOrDown(mat, dst, dstsize);
    jobject newBitmap = createBitmap(env, dst.cols, dst.rows, dst.type());
    LOGE("reSize src cols=%d rows=%d dst cols=%d rows=%d", mat.cols, mat.rows, dst.cols, dst.rows);
    mat2bitmap(env, dst, newBitmap);
    return newBitmap;
}


void MatFun::remap(JNIEnv *env, jobject &bitmap) {
    Mat mat;
    bitmap2mat(env, mat, bitmap);
    Mat dst;
    //方式1
    //注意CV_32FC1 与 at<float> 对应 ,系统cv::remap只能使用 CV_32FC1
/*    Mat mat_x(mat.size(), CV_32FC1);
    Mat mat_y(mat.size(), CV_32FC1);
    for (int row = 0; row < mat.rows; ++row) {
        for (int col = 0; col < mat.cols; ++col) {
            mat_x.at<float>(row, col) = mat.cols - col;
            mat_y.at<float>(row, col) = mat.rows - row;
        }
    }


    cv::remap(mat, dst, mat_x, mat_y, InterpolationFlags::INTER_LINEAR);*/
    //方式2 自定义remap
    Mat mat_x(mat.size(), mat.type());
    Mat mat_y(mat.size(), mat.type());
    for (int row = 0; row < mat.rows; ++row) {
        for (int col = 0; col < mat.cols; ++col) {
            mat_x.at<int>(row, col) = mat.cols - col;
            mat_y.at<int>(row, col) = mat.rows - row;
        }
    }
    ImageProc().dRemap(mat, dst, mat_x, mat_y);
    mat2bitmap(env, dst, bitmap);
}

void MatFun::equalizeHist(JNIEnv *env, jobject &bitmap) {
    Mat mat;
    bitmap2mat(env, mat, bitmap);
    //1.灰度化
    cvtColor(mat, mat, COLOR_BGR2GRAY);
    Mat hist;
    //方式1 系统的
    //cv::equalizeHist(mat,hist);
    //方式2 自定义的
    ImageProc().equalizeHist(mat, hist);
    mat2bitmap(env, hist, bitmap);

}


jobject MatFun::calcuHist(JNIEnv *env, jobject &bitmap) {
    ImageProc imageProc = ImageProc();
    Mat mat;
    bitmap2mat(env, mat, bitmap);
    //1.灰度化，直方图计算只能处理单通道图片
    Mat gray;
    cvtColor(mat, gray, COLOR_BGRA2GRAY);
    Mat hist;
    //计算直方图
    imageProc.calcHist(gray, hist);
    //归一化
    Mat normalize;
    int max_v = 255;
    imageProc.normalize(hist, normalize, max_v);
    //绘制计算之后的直方图
//  画直方图
    int bin_w = 5;
    int grad = 256;
    Mat hist_result = Mat(grad, grad * bin_w, CV_8UC3);
    for (int i = 0; i < grad; ++i) {
        //InputOutputArray img, Point pt1, Point pt2, const Scalar& color
        Point pointStart(i * bin_w, hist_result.rows);//rows 代表y
        Point pointEnd(i * bin_w, hist_result.rows - normalize.at<int>(0, i));
        line(hist_result, pointStart, pointEnd, Scalar(0, 255, 255), 1, LineTypes::LINE_AA);
    }

    jobject newBitmap = createBitmap(env, hist_result.cols, hist_result.rows, hist_result.type());
    mat2bitmap(env, hist_result, newBitmap);
    return newBitmap;

}

void MatFun::matLight(JNIEnv *env, jobject &bitmap) {
    Mat mat;
    bitmap2mat(env, mat, bitmap);
    Mat dst(mat.size(), mat.type());
    //方式1高亮
    int chananls = mat.channels();
    if (chananls == 4) {
        for (int row = 0; row < mat.rows; ++row) {
            for (int col = 0; col < mat.cols; ++col) {
                dst.at<Vec4b>(row, col)[0] = saturate_cast<uchar>(mat.at<Vec4b>(row, col)[0] + 20);
                dst.at<Vec4b>(row, col)[1] = saturate_cast<uchar>(mat.at<Vec4b>(row, col)[1] + 20);
                dst.at<Vec4b>(row, col)[2] = saturate_cast<uchar>(mat.at<Vec4b>(row, col)[2] + 20);
                dst.at<Vec4b>(row, col)[3] = saturate_cast<uchar>(mat.at<Vec4b>(row, col)[3] + 20);


            }
        }
    } else if (chananls == 3) {
        for (int row = 0; row < mat.rows; ++row) {
            for (int col = 0; col < mat.cols; ++col) {
                dst.at<Vec3b>(row, col)[0] = saturate_cast<uchar>(mat.at<Vec3b>(row, col)[0] + 20);
                dst.at<Vec3b>(row, col)[1] = saturate_cast<uchar>(mat.at<Vec3b>(row, col)[1] + 20);
                dst.at<Vec3b>(row, col)[2] = saturate_cast<uchar>(mat.at<Vec3b>(row, col)[2] + 20);


            }
        }
    } else {
        for (int row = 0; row < mat.rows; ++row) {
            for (int col = 0; col < mat.cols; ++col) {
                dst.at<uchar>(row, col) = saturate_cast<uchar>(mat.at<uchar>(row, col) + 20);

            }
        }
    }
    // mat2bitmap(env, dst, bitmap);
    //方式2高亮
    Mat hsv;
    cvtColor(mat, hsv, COLOR_BGR2HSV);
    vector<Mat> hsvMats;
    split(hsv, hsvMats);
    cv::equalizeHist(hsvMats[2], hsvMats[2]); //直方图均衡
    Mat mergeMat;
    cv::merge(hsvMats, hsv);
    cvtColor(hsv, mat, COLOR_HSV2BGR);//hsv 图必须转成原图BGR ,否则显示异常
    mat2bitmap(env, mat, bitmap);
}


/**
 * @param env
 * @param bitmap
 * @return
 */
jobject MatFun::codeVerification(JNIEnv *env, jobject &bitmap) {
    Mat src;
    bitmap2mat(env, src, bitmap);
    //1.图片转灰度化
    cvtColor(src, src, COLOR_BGR2GRAY);
    //2.图片二值化，自动阈值
    threshold(src, src, 0, 255, ThresholdTypes::THRESH_BINARY | ThresholdTypes::THRESH_OTSU);
    //3.查找轮廓
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(src, contours, hierarchy, RetrievalModes::RETR_LIST,
                 ContourApproximationModes::CHAIN_APPROX_SIMPLE);
    vector<Rect> codeMats;

    ImageProc imageProc = ImageProc();

    for (int i = 0; i < contours.size(); ++i) {
        //4.轮廓的筛选，传统二维码轮廓的大小>49 ,宽高比>0.8
        // 过滤面积，
        vector<Point> points = contours[i];
        double area = contourArea(points);

        if (area < 49) {
            continue;
        }
        RotatedRect rotateRect = minAreaRect(points);
        //高度
        float height = rotateRect.size.height;
        //宽度
        float width = rotateRect.size.width;

        float radio = min(height, width) / max(height, width);

        if (radio > 0.8 && width < src.cols / 2 && height < src.rows / 2) {
            // 去分析，找到满足宽高比的，满足宽高大小的
            Mat qrROI = imageProc.warpTransfrom(src, rotateRect);
            LOGE("codeVerification qrROI  width=%d height=%d ", qrROI.cols, qrROI.rows);
            //5.对比二维码特征黑白比例7:3:1
            if (imageProc.isYCorner(qrROI) && imageProc.isXCorner(qrROI)) {
                drawContours(src, contours, i, Scalar(0, 0, 255), 4);
                Rect bounding = boundingRect(points);
                codeMats.push_back(bounding);

            }
            // char* name;
            // sprintf(name,"rectangle_%d.jpg",i);
            //  LOGE("codeVerification name=%s", name);

        }
    }

    LOGE("codeVerification final  codeMats size=%d ", codeMats.size());
    if (codeMats.size() >= 3) {
        Rect roi_rect(imageProc.getX(codeMats), imageProc.getY(codeMats),
                      imageProc.getWidth(codeMats), imageProc.getHeight(codeMats));
        // 校正 Rect，确保不会越界
        roi_rect.x = max(0, roi_rect.x);
        roi_rect.y = max(0, roi_rect.y);
        roi_rect.width = min(roi_rect.width, src.cols - roi_rect.x);
        roi_rect.height = min(roi_rect.height, src.rows - roi_rect.y);

        // 再进行裁剪
        Mat mergeCode = src(roi_rect);
        cvtColor(mergeCode, mergeCode, COLOR_GRAY2BGR);
        //6.二维码剪切
        jobject newBitmap = createBitmap(env, mergeCode.cols, mergeCode.rows, mergeCode.type());
        mat2bitmap(env, mergeCode, newBitmap);
        LOGE("codeVerification Rect x=%d y=%d width=%d height=%d ", roi_rect.x, roi_rect.y,
             roi_rect.width, roi_rect.height);
        //7.二维码显示
        return newBitmap;
    }
    mat2bitmap(env, src, bitmap);
    return bitmap;
}


jobject MatFun::codeTiltVerification(JNIEnv *env, jobject &bitmap) {
    Mat src;
    bitmap2mat(env, src, bitmap);

    ImageProc imageProc = ImageProc();

    //1.图片转灰度化
    cvtColor(src, src, COLOR_BGR2GRAY);
    cv::equalizeHist(src, src);
    // 降噪
    GaussianBlur(src, src, Size(3, 3), 0);
    //2.图片二值化，自动阈值
    threshold(src, src, 0, 255, ThresholdTypes::THRESH_BINARY | ThresholdTypes::THRESH_OTSU);
    Mat dst(src);
    float angle = 90.0f;
    bool isCorrection = false;
    LOGE("codeTiltVerification src  type=%d channels=%d cols=%d rows=%d dst type=%d channels=%d cols=%d rows=%d ",
         src.type(), src.channels(), src.cols, src.rows, dst.type(), dst.channels(), dst.cols,
         dst.rows);
    //3.查找二维码轮廓
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours(dst, contours, hierarchy, RetrievalModes::RETR_LIST,
                 ContourApproximationModes::CHAIN_APPROX_SIMPLE);
    vector<Rect> codeMats;

    for (int i = 0; i < contours.size(); ++i) {
        //4.轮廓的筛选，传统二维码轮廓的大小>49 ,宽高比>0.8
        // 过滤面积，
        vector<Point> points = contours[i];
        double area = contourArea(points);

        if (area < 49) {
            continue;
        }
        RotatedRect rotateRect = minAreaRect(points);
        //高度
        float height = rotateRect.size.height;
        //宽度
        float width = rotateRect.size.width;

        float radio = min(height, width) / max(height, width);

        if (radio > 0.8 && width < dst.cols / 2 && height < dst.rows / 2) {
            // 去分析，找到满足宽高比的，满足宽高大小的
            Mat qrROI = imageProc.warpTransfrom(dst, rotateRect);
            LOGE("codeTiltVerification qrROI  width=%d height=%d ", qrROI.cols, qrROI.rows);
            //5.对比二维码特征黑白比例7:3:1
            if (imageProc.isYCorner(qrROI) && imageProc.isXCorner(qrROI)) {
                angle = rotateRect.angle;
                if (angle != 90.0f && !isCorrection) {
                    isCorrection = true;
                    i = 0;
                    imageProc.correctionMat(dst, dst, angle);
                    contours.clear();
                    findContours(dst, contours, hierarchy, RetrievalModes::RETR_LIST,
                                 ContourApproximationModes::CHAIN_APPROX_SIMPLE);
                    LOGE("codeTiltVerification reset continue angle=%f", angle);
                    continue;
                }
                drawContours(dst, contours, i, Scalar(0, 0, 255), 4);
                Rect bounding = boundingRect(points);
                codeMats.push_back(bounding);
                LOGE("codeTiltVerification push_back i=%d bounding x=%d y=%d width=%d height=%d ",
                     i, bounding.x, bounding.y, bounding.width, bounding.height);

            }
            // char* name;
            // sprintf(name,"rectangle_%d.jpg",i);
            //  LOGE("codeVerification name=%s", name);

        }
    }

    LOGE("codeTiltVerification final  codeMats size=%d angle=%f", codeMats.size(), angle);
    if (codeMats.size() >= 3) {
        Rect roi_rect(imageProc.getX(codeMats), imageProc.getY(codeMats),
                      imageProc.getWidth(codeMats), imageProc.getHeight(codeMats));
        // 校正 Rect，确保不会越界
        roi_rect.x = max(0, roi_rect.x);
        roi_rect.y = max(0, roi_rect.y);
        roi_rect.width = min(roi_rect.width, dst.cols - roi_rect.x);
        roi_rect.height = min(roi_rect.height, dst.rows - roi_rect.y);

        // 再进行裁剪
        Mat mergeCode = dst(roi_rect);
        cvtColor(mergeCode, mergeCode, COLOR_GRAY2BGR);
        //6.二维码剪切
        jobject newBitmap = createBitmap(env, mergeCode.cols, mergeCode.rows, mergeCode.type());
        mat2bitmap(env, mergeCode, newBitmap);
        // imageProc.restoreMat(mergeCode,mergeCode,angle);
        LOGE("codeTiltVerification Rect x=%d y=%d width=%d height=%d ", roi_rect.x, roi_rect.y,
             roi_rect.width, roi_rect.height);
        //7.二维码显示
        return newBitmap;
    }

    mat2bitmap(env, src, bitmap);
    return bitmap;
}

/**
 * 三种解决方案：
    1. 再写一套识别圆形特征的代码
    2. 借鉴人脸识别的方案，采用训练样本的方式识别
    3. 换一种检查方案，只写一套代码
 * @param env
 * @param bitmap
 * @return
 *  1. 对其进行轮廓查找
    2. 对查找的到的轮廓进行初步过滤
    3. 判断是否是一个大轮廓套两个小轮廓且符合特征规则(面积比例判断)
    4. 截取二维码区域
    5. 识别二维码
 */
jobject MatFun::codeRoundVerification(JNIEnv *env, jobject &bitmap) {
    ImageProc imageProc = ImageProc();

    Mat src;
    bitmap2mat(env, src, bitmap);
    vector<Rect> codeMats;
    // 对图像进行灰度转换
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    //方式一 二值化
    threshold(gray, gray, 0, 255, THRESH_BINARY | THRESH_OTSU);

    /**
      * 方式二
      * todo 特别注意二维码图像是彩色的，背景和前景对比不强，并且颜色变化细腻，直接灰度 + Otsu 会导致重要的颜色细节丢失，二维码圆点被清除或连接不清晰。使用 adaptiveThreshold + 色彩通道分离（保留色彩特征）
      * todo我们可以将图像转换为 HSV 或 LAB 色彩空间，并选择其中的亮度/色度通道进行自适应二值化。
      *  Mat hsv, vChannel;
         cvtColor(src, hsv, COLOR_BGR2HSV);
         vector<Mat> hsv_channels;
         split(hsv, hsv_channels); // HSV 通道分离
         vChannel = hsv_channels[2]; // 亮度通道
         // 使用自适应阈值，适应局部颜色变化
         adaptiveThreshold(vChannel, vChannel, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 31, 5);
         // 可选：形态学操作增强圆形结构（闭运算填补间隙）
         Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
         morphologyEx(vChannel, gray, MORPH_CLOSE, kernel);
    */

    // 1. 对其进行轮廓查找
    vector<Vec4i> hierarchy;
    vector<vector<Point> > contours;
    vector<vector<Point> > contoursRes;
    /**
     参数说明：https://blog.csdn.net/guduruyu/article/details/69220296
        输入图像image必须为一个2值单通道图像
        contours参数为检测的轮廓数组，每一个轮廓用一个point类型的vector表示
        hiararchy参数和轮廓个数相同，每个轮廓contours[ i ]对应4个hierarchy元素hierarchy[ i ][ 0 ] ~hierarchy[ i ][ 3 ]，
            分别表示后一个轮廓、前一个轮廓、父轮廓、内嵌轮廓的索引编号，如果没有对应项，该值设置为负数。
        mode表示轮廓的检索模式
            CV_RETR_EXTERNAL 表示只检测外轮廓
            CV_RETR_LIST 检测的轮廓不建立等级关系
            CV_RETR_CCOMP 建立两个等级的轮廓，上面的一层为外边界，里面的一层为内孔的边界信息。如果内孔内还有一个连通物体，这个物体的边界也在顶层。
            CV_RETR_TREE 建立一个等级树结构的轮廓。具体参考contours.c这个demo
        method为轮廓的近似办法
            CV_CHAIN_APPROX_NONE 存储所有的轮廓点，相邻的两个点的像素位置差不超过1，即max（abs（x1-x2），abs（y2-y1））==1
            CV_CHAIN_APPROX_SIMPLE 压缩水平方向，垂直方向，对角线方向的元素，只保留该方向的终点坐标，例如一个矩形轮廓只需4个点来保存轮廓信息
            CV_CHAIN_APPROX_TC89_L1，CV_CHAIN_APPROX_TC89_KCOS 使用teh-Chinl chain 近似算法
        offset表示代表轮廓点的偏移量，可以设置为任意值。对ROI图像中找出的轮廓，并要在整个图像中进行分析时，这个参数还是很有用的。
     */
    findContours(gray, contours, hierarchy, RetrievalModes::RETR_TREE, CHAIN_APPROX_NONE,
                 Point(0, 0));
    int tCC = 0; // 临时用来累加的子轮廓计数器
    int pId = -1;// 父轮廓的 index
    for (int i = 0; i < contours.size(); i++) {
        if (hierarchy[i][2] != -1 && tCC == 0) {
            pId = i;
            tCC++;
        } else if (hierarchy[i][2] != -1) {// 有父轮廓
            tCC++;
        } else if (hierarchy[i][2] == -1) {// 没有父轮廓
            tCC = 0;
            pId = -1;
        }
        // 找到了两个子轮廓
        if (tCC >= 2) {
            Rect bounding = boundingRect(contours[i]);
            codeMats.push_back(bounding);
            contoursRes.push_back(contours[pId]);
            tCC = 0;
            pId = -1;
        }
    }


    for (int i = 0; i < contoursRes.size(); ++i) {
        drawContours(src, contoursRes, i, Scalar(255, 0, 0), 2);
    }
    LOGE("codeRoundVerification contoursRes.size=%d codeMats.size=%d", contoursRes.size(),
         codeMats.size());
    // 裁剪二维码，交给 zxing 或者 zbar 处理即可
    if (codeMats.size() >= 3) {
        Rect roi_rect(imageProc.getX(codeMats), imageProc.getY(codeMats),
                      imageProc.getWidth(codeMats), imageProc.getHeight(codeMats));
        // 校正 Rect，确保不会越界
        roi_rect.x = max(0, roi_rect.x);
        roi_rect.y = max(0, roi_rect.y);
        roi_rect.width = min(roi_rect.width, src.cols - roi_rect.x);
        roi_rect.height = min(roi_rect.height, src.rows - roi_rect.y);

        // 再进行裁剪
        Mat mergeCode = src(roi_rect);
        cvtColor(mergeCode, mergeCode, COLOR_GRAY2BGR);
        //6.二维码剪切
        jobject newBitmap = createBitmap(env, mergeCode.cols, mergeCode.rows, mergeCode.type());
        mat2bitmap(env, mergeCode, newBitmap);
        LOGE("codeRoundVerification Rect x=%d y=%d width=%d height=%d ", roi_rect.x, roi_rect.y,
             roi_rect.width, roi_rect.height);
        //7.二维码显示
        return newBitmap;
    }


    mat2bitmap(env, src, bitmap);

    return bitmap;
}

/**
 * HOGDescriptor 原理详解
HOG（Histogram of Oriented Gradients，方向梯度直方图）是一种在计算机视觉和图像处理中用于物体检测的特征描述方法。HOGDescriptor 是 OpenCV 中实现这一算法的类。

一、HOG 基本概念
HOG 特征的核心思想是：局部物体的外观和形状可以被梯度或边缘方向的分布很好地描述。它通过计算和统计图像局部区域的梯度方向直方图来构成特征。

二、HOG 算法步骤
1. 图像预处理
    灰度化：将彩色图像转换为灰度图像
    Gamma 校正：对图像进行归一化处理，减少光照影响
    math
    I(x,y) = I(x,y)^{gamma}
    通常 gamma 取 1/2

2. 计算梯度
    对于每个像素点，计算其在 x 和 y 方向的梯度：
    math
    G_x(x,y) = I(x+1,y) - I(x-1,y)
    G_y(x,y) = I(x,y+1) - I(x,y-1)
    然后计算梯度的幅值和方向：
    math
    G(x,y) = \sqrt{G_x(x,y)^2 + G_y(x,y)^2}
    θ(x,y) = \arctan\left(\frac{G_y(x,y)}{G_x(x,y)}\right)
3. 划分细胞单元(Cell)
   将图像划分为小的空间区域，称为细胞单元（Cell）。通常每个 Cell 大小为 8×8 像素。

4. 计算细胞单元的梯度方向直方图
   将 0-180°（无符号梯度）或 0-360°（有符号梯度）划分为若干个 bins（通常 9 个 bins，每 20° 一个 bin）
   根据像素的梯度方向，将其幅值加权投影到对应的方向 bin 中

5. 块(Block)归一化
    将多个细胞单元组合成块（Block），通常为 2×2 个 Cell（16×16 像素）
    对 Block 内的所有 Cell 的直方图进行串联，然后进行归一化处理
    常用的归一化方法有：L2-norm、L1-norm、L1-sqrt 等

6. 生成 HOG 特征向量
   将所有 Block 的特征向量串联起来，形成最终的 HOG 特征描述符。

三、OpenCV 中的 HOGDescriptor
    OpenCV 中的 HOGDescriptor 类实现了上述算法，主要参数包括：
    cv::HOGDescriptor hog(
        Size winSize,           // 检测窗口大小
        Size blockSize,         // 块大小
        Size blockStride,       // 块滑动步长
        Size cellSize,         // 细胞单元大小
        int nbins,             // 直方图的bin数量
        int derivAperture = 1, // 梯度计算的sobel算子孔径
        double winSigma = -1,  // 高斯平滑窗口参数
        int histogramNormType = HOGDescriptor::L2Hys, // 归一化类型
        double L2HysThreshold = 0.2, // L2-norm归一化后截断阈值
        bool gammaCorrection = true, // 是否进行gamma校正
        int nlevels = 64       // 检测窗口放大倍数
    );
四、HOG 特征的应用
    行人检测：HOG + SVM 是经典的 pedestrian detection 方法
    物体识别：可作为通用特征用于各种物体识别任务
    图像分类：作为图像的一种特征表示

五、HOG 的优缺点
优点：
    对几何和光学变化有很好的不变性
    局部单元格操作使其对部分遮挡不敏感
    特征维度相对较低，计算效率较高

缺点：
    对方向变化敏感
    需要固定大小的输入窗口
    在复杂背景下检测效果可能下降
 * @param env
 * @param bitmap
 */
jobject MatFun::hog(JNIEnv *env, jobject &bitmap) {
    Mat src;
    bitmap2mat(env, src, bitmap);
//    resize(src, src, Size(64, 128));
//    Mat gray;
//    cvtColor(src, gray, COLOR_BGR2GRAY);
//    HOGDescriptor hog;
    /**
     *  特征向量维度计算
        HOG 特征向量的维度可以通过以下方式计算：
        对于检测窗口大小为 winSize，块大小为 blockSize，块步长为 blockStride，细胞大小为 cellSize，bin 数为 nbins：
        每个 block 包含 (blockSize.width/cellSize.width) × (blockSize.height/cellSize.height) 个 cell
        每个 block 的特征维度为 (blockSize.width/cellSize.width) × (blockSize.height/cellSize.height) × nbins
        窗口中的 block 数量为 ((winSize.width-blockSize.width)/blockStride.width + 1) × ((winSize.height-blockSize.height)/blockStride.height + 1)
        总维度 = 每个 block 的维度 × block 数量
     */
//    vector<float> descriptors;
//    vector<Point> locations;
    /**
     *  img：输入图像（可以是单通道或多通道，但通常使用灰度图像）
        descriptors：输出的 HOG 特征向量
        winStride：窗口移动步长（默认等于 blockStride）
        padding：填充大小（用于处理边界）
        locations：指定要计算 HOG 特征的位置集合（如果为空，则计算整个图像的密集特征）
     */
//    hog.compute(gray, descriptors, Size(), Size(), locations);
//    LOGE("HOG descriptor size:%d", descriptors.size());//3780
    /**
     * detectMultiScale 是 OpenCV 中 HOGDescriptor 类用于多尺度物体检测的核心方法，特别在行人检测中广泛应用。下面我将全面解析这个方法的工作原理、参数和使用技巧。

        一、方法原型
        void HOGDescriptor::detectMultiScale(
            InputArray img,
            std::vector<Rect>& foundLocations,
            double hitThreshold = 0,
            Size winStride = Size(),
            Size padding = Size(),
            double scale = 1.05,
            double finalThreshold = 2.0,
            bool useMeanshiftGrouping = false
        ) const;
        二、参数详解
            参数	类型	说明
            img	InputArray	输入图像（通常为灰度图像）
            foundLocations	vector<Rect>&	输出检测到的目标边界框
            hitThreshold	double	分类器判定阈值（通常设为0，使用默认SVM的rho值）
            winStride	Size	检测窗口移动步长（默认等于blockStride）
            padding	Size	图像边缘填充（处理边界效应）
            scale	double	图像金字塔缩放因子（>1.0）
            finalThreshold	double	非极大值抑制阈值
            useMeanshiftGrouping	bool	是否使用meanshift方法合并检测结果
        三、工作原理
            图像金字塔构建：
            通过不断缩放原始图像（缩放因子为scale）构建图像金字塔
            在每个尺度上使用固定大小的窗口进行检测
            滑动窗口检测：
            在每个金字塔层级上，窗口以winStride为步长滑动
            对每个窗口位置提取HOG特征
            使用预训练的SVM分类器判断是否包含目标
            非极大值抑制：
            合并重叠的检测窗口
            使用finalThreshold控制合并的严格程度
            结果优化：
            可选使用meanshift方法进一步优化检测框位置
    */
    Mat bgr;
    cvtColor(src, bgr, COLOR_BGRA2BGR);
    HOGDescriptor descriptor;
    descriptor.setSVMDetector(descriptor.getDefaultPeopleDetector());
    vector<Rect> foundLocations;
    descriptor.detectMultiScale(bgr, foundLocations, 0, Size(14, 14));
    for (int i = 0; i < foundLocations.size(); ++i) {
        rectangle(src, foundLocations[i], Scalar(255, 0, 0), 2, LINE_AA);
    }
    jobject newBitmap = createBitmap(env, src.rows, src.cols, src.type());
    mat2bitmap(env, src, newBitmap);

    return newBitmap;

}

jobject MatFun::lbp(JNIEnv *env, jobject &bitmap) {
    //3*3 矩阵
    Mat src;
    bitmap2mat(env, src, bitmap);
    Mat gray;
    cvtColor(src, gray, COLOR_BGR2GRAY);
    LOGE("lbp gray cols:%d rows:%d", gray.cols, gray.rows);


    // 方式1：3*3 lbp 计算的特征数据
    /* Mat result = Mat::zeros(Size(gray.cols - 2, gray.rows - 2), CV_8UC1);
     //计算lbp ,强化轮廓
     for (int row = 1; row < gray.rows - 1; ++row) {
         for (int col = 1; col < gray.cols - 1; ++col) {
             uchar pixels =  gray.at<uchar>(row, col);
             int rPixels = 0;
             rPixels |= (pixels >= gray.at<uchar>(row - 1, col - 1)) << 0;
             rPixels |= (pixels >= gray.at<uchar>(row - 1, col)) << 1;
             rPixels |= (pixels >= gray.at<uchar>(row - 1, col + 1)) << 2;
             rPixels |= (pixels >= gray.at<uchar>(row, col - 1)) << 7;
             rPixels |= (pixels >= gray.at<uchar>(row, col + 1)) << 3;
             rPixels |= (pixels >= gray.at<uchar>(row + 1, col - 1)) << 6;
             rPixels |= (pixels >= gray.at<uchar>(row + 1, col)) << 5;
             rPixels |= (pixels >= gray.at<uchar>(row + 1, col + 1)) << 4;
             result.at<uchar>(row - 1, col - 1) = rPixels;
         }
     }*/
    //方式2：

    /* Mat expansion;
     copyMakeBorder(gray, expansion, 1, 1 ,1, 1,
                    BorderTypes::BORDER_CONSTANT, Scalar().all(0));
     LOGE("lbp expansion cols:%d rows:%d", expansion.cols, expansion.rows);
     Mat result = Mat::zeros(Size(gray.cols, gray.rows), CV_8UC1);
     for (int row = 1; row < expansion.rows-1 ; ++row) {
         for (int col = 1; col < expansion.cols-1; ++col) {
             uchar pixels =  expansion.at<uchar>(row, col);
             int rPixels = 0;
             rPixels |= (pixels >= expansion.at<uchar>(row - 1, col - 1)) << 0;
             rPixels |= (pixels >= expansion.at<uchar>(row - 1, col)) << 1;
             rPixels |= (pixels >= expansion.at<uchar>(row - 1, col + 1)) << 2;
             rPixels |= (pixels >= expansion.at<uchar>(row, col - 1)) << 7;
             rPixels |= (pixels >= expansion.at<uchar>(row, col + 1)) << 3;
             rPixels |= (pixels >= expansion.at<uchar>(row + 1, col - 1)) << 6;
             rPixels |= (pixels >= expansion.at<uchar>(row + 1, col)) << 5;
             rPixels |= (pixels >= expansion.at<uchar>(row + 1, col + 1)) << 4;
             result.at<uchar>(row-1, col-1) = rPixels;
         }
     }*/

    //方式3：使用指针效果高
    Mat expansion;
    copyMakeBorder(gray, expansion, 1, 1, 1, 1,
                   BorderTypes::BORDER_CONSTANT, Scalar().all(0));
    LOGE("lbp expansion cols:%d rows:%d", expansion.cols, expansion.rows);
    Mat result = Mat::zeros(Size(gray.cols, gray.rows), CV_8UC1);
    for (int row = 1; row < expansion.rows - 1; ++row) {

        uchar *pixelsPre = expansion.ptr(row - 1);
        uchar *pixelsCur = expansion.ptr(row);
        uchar *pixelsLast = expansion.ptr(row + 1);
        //输出
        uchar *piexlsOut = result.ptr(row - 1);
        for (int col = 1; col < expansion.cols - 1; col += expansion.channels()) {
            int rPixels = 0;
            uchar pixels = pixelsCur[col];
            rPixels |= (pixels >= pixelsPre[col - 1]) << 0;
            rPixels |= (pixels >= pixelsPre[col]) << 1;
            rPixels |= (pixels >= pixelsPre[col + 1]) << 2;

            rPixels |= (pixels >= pixelsCur[col - 1]) << 7;
            rPixels |= (pixels >= pixelsCur[col + 1]) << 3;

            rPixels |= (pixels >= pixelsLast[col - 1]) << 6;
            rPixels |= (pixels >= pixelsLast[col]) << 5;
            rPixels |= (pixels >= pixelsLast[col + 1]) << 4;
            piexlsOut[col - 1] = rPixels;
        }

    }
    jobject newBitmap = createBitmap(env, result.rows, result.cols, src.type());
    mat2bitmap(env, result, newBitmap);

    return newBitmap;
}

jobject MatFun::haar(JNIEnv *env, jobject &bitmap) {
    Mat src;
    bitmap2mat(env, src, bitmap);

    // 拿 hog 的特征
    // 1. 分成 8* 8 的网格 ceil 求取直方图
    // 2. 对 ceil 的 4*4 的块去合并直方图，每次必须要有 1/2 的重叠区域
    // 3. 然后获取直方图的数据

    /*
     CV_WRAP HOGDescriptor() : winSize(64,128), blockSize(16,16), blockStride(8,8),
        cellSize(8,8), nbins(9), derivAperture(1), winSigma(-1),
        histogramNormType(HOGDescriptor::L2Hys), L2HysThreshold(0.2), gammaCorrection(true),
        free_coef(-1.f), nlevels(HOGDescriptor::DEFAULT_NLEVELS), signedGradient(false)
    {}


    Mat dst, dst_gary;
    resize(src, dst, Size(64, 128));
    cvtColor(dst, dst_gary, COLOR_BGRA2GRAY);

    HOGDescriptor hogDescriptor;
    vector<float> descriptors;
    vector<Point> locations;

     CV_WRAP virtual void compute(InputArray img,
                 CV_OUT std::vector<float>& descriptors,
                 Size winStride = Size(), Size padding = Size(),
                 const std::vector<Point>& locations = std::vector<Point>()) const;

    hogDescriptor.compute(dst_gary,descriptors,Size(),Size(),locations);

    __android_log_print(ANDROID_LOG_ERROR,"TAG","descriptors size : %d", descriptors.size());
    */

    /*
    CV_WRAP virtual void detectMultiScale(InputArray img, CV_OUT std::vector<Rect>& foundLocations,
                                          CV_OUT std::vector<double>& foundWeights, double hitThreshold = 0,
                                          Size winStride = Size(), Size padding = Size(), double scale = 1.05,
                                          double finalThreshold = 2.0,bool useMeanshiftGrouping = false) const;
                                          */
    // 训练样本，直接拿过来用
    Mat bgr;
    cvtColor(src, bgr, COLOR_BGRA2BGR);
    HOGDescriptor descriptor;
    descriptor.setSVMDetector(descriptor.getDefaultPeopleDetector());
    vector<Rect> foundLocations;
    descriptor.detectMultiScale(bgr, foundLocations, 0, Size(14, 14));
    for (int i = 0; i < foundLocations.size(); ++i) {
        rectangle(src, foundLocations[i], Scalar(255, 0, 0), 2, LINE_AA);
    }
    return bitmap;
}