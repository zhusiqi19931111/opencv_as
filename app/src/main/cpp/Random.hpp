//
// Created by cherish on 2025/5/18.
//

#ifndef OPENCV_RANDOM_HPP
#define OPENCV_RANDOM_HPP

#include <random> // C++11 随机数库
using namespace std;

class Random{
public :
    int random_start_end(int start ,int end){
        random_device rd;//用于获取随机种子
        mt19937 gen(rd()); // 使用Mersenne Twister引擎
        uniform_int_distribution dis(start, end);
        int random = dis(gen);
        return random;
    };
};

#endif //OPENCV_RANDOM_HPP
