//
// Created by cherish on 2025/5/20.
//

#include "ImageProc.h"

void ImageProc::pyrUpOrDown(const Mat &src, Mat &dst, const Size &dstsize) {
    LOGE("ImageProc pyrUpOrDown ");
    dst.create(dstsize.height, dstsize.width, src.type());

    float src_w = src.cols;
    float src_h = src.rows;

    for (int row = 0; row < dst.rows; ++row) {
        for (int col = 0; col < dst.cols; ++col) {
            //  height 100 src_h 200
            int src_rows = row * (src_h / dstsize.height);// src 的 高
            int src_cols = col * (src_w / dstsize.width);
            int pixels = src.at<int>(src_rows, src_cols);
            dst.at<int>(row, col) = pixels;
        }
    }

}

void ImageProc::dRemap(const Mat &src, Mat &dst, Mat &mat_x, Mat &mat_y) {
    LOGE("ImageProc dRemap");
    dst.create(src.size(), src.type());
    int res_w = dst.cols;
    int res_h = dst.rows;
    for (int rows = 0; rows < res_h; ++rows) {
        for (int cols = 0; cols < res_w; ++cols) {
            int x = mat_x.at<int>(rows, cols);
            int y = mat_y.at<int>(rows, cols);
            dst.at<int>(rows, cols) = src.at<int>(x, y);
        }
    }
}

/**
 * 计算单通道图片像素值的个数
 * @param src
 * @param dst
 */
void ImageProc::calcHist(const Mat &src, Mat &dst) {
    dst.create(1, 256, CV_32S); //int 单通道 最大值256,
    cout << "cal" << dst << endl;
    for (int col = 0; col < dst.cols; ++col) {
        dst.at<int>(0, col) = 0;
    }
    for (int row = 0; row < src.rows; ++row) {
        for (int col = 0; col < src.cols; ++col) {
            int index = src.at<uchar>(row, col);//uchar 单通道图片
            dst.at<int>(0, index) += 1;
        }
    }

}

/**
 * 归一化
 * @param src
 * @param dst
 */
void ImageProc::normalize(const Mat &src, Mat &dst, int max_v) {
    int search_max = 0;
    dst.create(src.size(), src.type()); //int 单通道
    for (int row = 0; row < src.rows; ++row) {
        for (int col = 0; col < src.cols; ++col) {
            int value = src.at<int>(row, col);
            search_max = max(value, search_max);

        }
    }
    LOGE("ImageProc search_max=%d", search_max);
    for (int row = 0; row < src.rows; ++row) {
        for (int col = 0; col < src.cols; ++col) {
            //int 需要判断图片类型
            int value = src.at<int>(row, col);
            dst.at<int>(row, col) = (1.0 / search_max) * value * max_v;

        }
    }


}

// 1. 直方图的统计
// 2. 计算直方图中像素的概率  计算规则：1.像素/width*height 2. 概率累加
// 3. 生成一张映射表
// 4. 从映射表中查找赋值
void ImageProc::equalizeHist(const Mat &src, Mat &dst) {
    Mat hist;
    // 1. 直方图的统计
    calcHist(src, hist);
    Mat probMat;
    //概率是浮点类型
    probMat.create(hist.size(), CV_32FC1);
    // 2. 计算直方图中像素的概率  计算规则：1.像素/width*height 2. 概率累加
    float size = src.rows * src.cols;
    float prob_sum = 0.0;
    for (int col = 0; col < hist.cols; ++col) {
        int times = hist.at<int>(0, col);
        prob_sum += times / size;
        probMat.at<float>(0, col) = prob_sum;

    }
    // 3. 生成一张映射表
    Mat map;
    map.create(probMat.size(), CV_32FC1);
    for (int col = 0; col < probMat.cols; ++col) {
        float prob = probMat.at<float>(0, col);
        map.at<float>(0, col) = 255 * prob; //特别注意255 不能使用col

    }
    // 4. 从映射表中查找赋值
    dst.create(src.size(), src.type());
    for (int row = 0; row < src.rows; ++row) {
        for (int col = 0; col < src.cols; ++col) {
            uchar pixels = src.at<uchar>(row, col);
            dst.at<uchar>(row, col) = map.at<float>(0, pixels);
        }
    }

}
