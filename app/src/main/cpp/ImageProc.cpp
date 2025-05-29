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




/**
 * warpAffine适用场景：

图像旋转（配合getRotationMatrix2D）
    图像平移
    均匀缩放
    错切变换
    简单的形状对齐

warpPerspective适用场景：
    文档扫描矫正（如手机拍文档）
    车牌识别中的倾斜矫正
    AR/VR中的视角变换
    全景图像拼接
    实现三维投影效果
 getPerspectiveTransform

    精确解算：基于4组精确的点对应关系
    数学基础：直接解线性方程组
    前提假设：点对应完全准确，无噪声
    典型应用：已知精确对应点的场景（如文档扫描）

findHomography
    鲁棒估计：可处理多组（≥4）含噪声的点对应
    数学基础：使用RANSAC等鲁棒估计算法
    附加功能：可识别并剔除异常点（outliers）
    典型应用：特征匹配、图像拼接、真实场景中的透视估计
 */

/***
 * 放射频变换，将矩形调整为平面矩形
 * @param gary
 * @param rect
 * @return
 */
Mat ImageProc::warpTransfrom(const Mat &gary, RotatedRect &rect){
    int width = rect.size.width;
    int height = rect.size.height;
    Mat result(Size(width, height), gary.type());

    // 矩阵怎么获取
    vector<Point> srcPoints;
    Point2f pts[4];
    rect.points(pts);
    for (int i = 0; i < 4; i++)
    {
        srcPoints.push_back(pts[i]);
    }
    vector<Point> dstPoints;
    dstPoints.push_back(Point(0, 0));
    dstPoints.push_back(Point(width, 0));
    dstPoints.push_back(Point(width, height));
    dstPoints.push_back(Point(0, height));

    Mat M = findHomography(srcPoints, dstPoints);
    warpPerspective(gary, result, M, result.size());

    return result;
}


bool ImageProc::isYCorner(const Mat &qrROI){
    // 统计白色像素点和黑色像素点
    int bp = 0, wp = 0;
    int width = qrROI.cols;
    int height = qrROI.rows;
    int cx = width / 2;

    // 中心点是黑的
    int pv = 0;

    for (int row = 0; row < height; row++)
    {
        pv = qrROI.at<uchar>(row, cx);
        if (pv == 0){
            bp++;
        }
        else if (pv == 255){
            wp++;
        }
    }

    if (bp == 0 || wp == 0){
        return false;
    }

    if (wp * 2 > bp || bp > 4 * wp){
        return false;
    }

    LOGE("isYCorner widht=%d height=%d", qrROI.cols,qrROI.rows);
    return true;
}
bool ImageProc::isXCorner(const Mat &qrROI){
// 四个值
    int cb = 0, lw = 0, rw = 0, lb = 0, rb = 0;

    int width = qrROI.cols;
    int height = qrROI.rows;
    int cx = width / 2;
    int cy = height / 2;
    uchar pixels = qrROI.at<uchar>(cy, cx);
    LOGE("isXCorner widht=%d height=%d cx=%d cy=%d pixels=%d", qrROI.cols,qrROI.rows,cx,cy,pixels);
    if (pixels == 255){
        return false;
    }

    // 求中心黑色
    int start = 0, end = 0, offset = 0;
    bool findleft = false, findright = false;
    while (true){
        offset++;

        if ((cx - offset) <= 0 || (cx+offset) >= width - 1){
            break;
        }

        // 左边扫
        pixels = qrROI.at<uchar>(cy, cx - offset);
        if (!findleft && pixels == 255){
            start = cx - offset;
            findleft = true;
        }

        // 右边扫
        pixels = qrROI.at<uchar>(cy, cx + offset);
        if (!findright && pixels == 255){
            end = cx + offset;
            findright = true;
        }

        if (findleft && findright){
            break;
        }
    }
    if (start == 0 || end == 0){
        return false;
    }
    cb = end - start;
    // 相间的白色
    for (int col = end; col < width - 1; col++)
    {
        pixels = qrROI.at<uchar>(cy, col);
        if (pixels == 0){
            break;
        }
        rw++;
    }
    for (int col = start; col > 0; col--)
    {
        pixels = qrROI.at<uchar>(cy, col);
        if (pixels == 0){
            break;
        }
        lw++;
    }
    if (rw == 0 || lw == 0){
        return false;
    }

    // 两边的黑色
    for (int col = end + rw; col < width - 1; col++)
    {
        pixels = qrROI.at<uchar>(cy, col);
        if (pixels == 255){
            break;
        }
        rb++;
    }
    for (int col = start - lw; col > 0; col--)
    {
        pixels = qrROI.at<uchar>(cy, col);
        if (pixels == 255){
            break;
        }
        lb++;
    }
    if (rb == 0 || lb == 0){
        return false;
    }

    float sum = cb + lb + rb + lw + rw;
    // 求比例 3:1:1:1:1
    cb = (cb / sum)*7.0 + 0.5;
    lb = (lb / sum)*7.0 + 0.5;
    rb = (rb / sum)*7.0 + 0.5;
    lw = (lw / sum)*7.0 + 0.5;
    rw = (rw / sum)*7.0 + 0.5;
    LOGE("isXCorner sum=%f cb=%f lb=%f rb=%f lw=%f rw=%f", sum,cb,lb,rb,lw,rw);
    if ((cb == 3 || cb == 4) && (lw == rw) && (lb == rb) && (lw == rb) && (lw == 1)){
        // 3:1:1:1:1 or 4:1:1:1:1

        return true;
    }

    return false;
}

int ImageProc::getX(const vector<Rect> &rects,float angle){
    //查找x值最小的矩形
    auto minRectByx = min_element(rects.begin(), rects.end(),
                                  [](const Rect& a, const Rect& b) {
                                      return a.x < b.x;
                                  });
    return minRectByx->x*sin(angle * M_PI / 180.0);

}
int ImageProc::getY(const vector<Rect> &rects,float angle){
    //查找Y值最小的矩阵
    auto minRectByy= min_element(rects.begin(),rects.end(),
                                 [](const Rect& a, const Rect& b)->bool{
                                     return  a.y<b.y;

                                 });
    return minRectByy->y*sin(angle * M_PI / 180.0);
}
int ImageProc::getWidth(const vector<Rect> &rects,float angle){
    //查找最大值x+width
    auto maxRectByx= min_element(rects.begin(),rects.end(),
                                 [](const Rect& a, const Rect& b)->bool{
                                     return  a.x>b.x;

                                 });
    return (maxRectByx->x +maxRectByx->width)*sin(angle * M_PI / 180.0);
}
int ImageProc::getHeight(const vector<Rect> &rects,float angle){
    //查找最大值y+height
    auto maxRectByy= min_element(rects.begin(),rects.end(),
                                 [](const Rect& a, const Rect& b)->bool{
                                     return  a.y>b.y;

                                 });
    return (maxRectByy->y+maxRectByy->height)*sin(angle * M_PI / 180.0);
}
/**
 * 判断矩形区域是否倾斜
 * @param gray
 * @param rect
 * @return
 */
bool ImageProc::isRegionTiltedByHough(const Mat& gray, const Rect& rect) {
    Mat roi = gray(rect).clone();
    Mat edges;
    Canny(roi, edges, 50, 150);

    // 标准Hough变换检测直线
    vector<Vec2f> lines;
    HoughLines(edges, lines, 1, CV_PI/180, 100);

    // 统计角度直方图
    const int bins = 18; // 每10度一个bin
    float hist[bins] = {0};

    for (const auto& line : lines) {
        float theta = line[1] * 180 / CV_PI;
        if (theta >= 180) theta -= 180;
        int bin = int(theta / 10);
        hist[bin]++;
    }

    // 寻找主方向
    int maxBin = 0;
    for (int i = 1; i < bins; i++) {
        if (hist[i] > hist[maxBin]) maxBin = i;
    }

    float mainAngle = maxBin * 10 + 5; // 取bin中间值

    // 检查是否倾斜
    const double tiltThreshold = 10.0;
    if (abs(mainAngle) > tiltThreshold && abs(90 - mainAngle) > tiltThreshold) {
        return true;
    }

    return false;
}

void ImageProc::restoreMat(const Mat& src, Mat& dst, float angle ){
    Mat rotMat = getRotationMatrix2D(Point2f(src.cols>>2,src.rows>>2), angle, 1.0);

    warpAffine(src, dst, rotMat, src.size(), INTER_LINEAR, BORDER_CONSTANT, Scalar(255,255,255));

}
void ImageProc::correctionMat(const Mat& binary,Mat& dst,float angle) {

    Point2f center(binary.cols / 2.0, binary.rows / 2.0);
    Mat rotMat = getRotationMatrix2D(center, angle, 1.0);

    // 计算新图像边界以防裁切
    Rect2f bbox = RotatedRect(center, binary.size(), angle).boundingRect2f();
    rotMat.at<double>(0, 2) += bbox.width / 2.0 - center.x;
    rotMat.at<double>(1, 2) += bbox.height / 2.0 - center.y;

    warpAffine(binary, dst, rotMat, bbox.size());
    LOGE("correctionMat binary cols=%d rows=%d dst cols=%d rows=%d angle=%f", binary.cols,binary.rows,dst.cols,dst.rows,angle);
}