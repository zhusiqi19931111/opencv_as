//
// Created by cherish on 2025/4/10.
//

#ifndef DATASTRUCTCLASS_LOG_H
#define DATASTRUCTCLASS_LOG_H

#include <android/log.h> //添加头文件

#define LOG_TAG "native-lib.cpp" //定义TAG
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#endif //DATASTRUCTCLASS_LOG_H
