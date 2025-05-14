//
// Created by cherish on 2025/5/13.
//

#ifndef OPENCV_JNITHREADFUN_H
#define OPENCV_JNITHREADFUN_H

#include <jni.h>
#include <mutex>
#include <thread>
#include <queue>
#include "log.h"

using namespace std;

class JniThreadFun {
private:
    JavaVM *m_vm;
    mutex m_mutex;
    jobject params;
    jobject callback;

public:
    JniThreadFun(JavaVM *vm, JNIEnv *env, jobject& params, jobject& callback);

    ~JniThreadFun();

    void run_task(void(*execute)(JNIEnv *env, jobject& params, jobject& callback));

    JNIEnv *getEnv();

private:
    void maybeDetach();

};

JniThreadFun::JniThreadFun(JavaVM *vm, JNIEnv *env, jobject& params, jobject& callback) : m_vm(vm),
                                                                                        params(env->NewGlobalRef(
                                                                                                params)),
                                                                                        callback(env->NewGlobalRef(
                                                                                                callback)){
    LOGE("JniThreadFun create");
}

JniThreadFun::~JniThreadFun() {
    LOGE("JniThreadFun destroy");
    JNIEnv *env = getEnv();
    if (env) {
        env->DeleteGlobalRef(params);
        env->DeleteGlobalRef(callback);
        maybeDetach();
    }

}

JNIEnv *JniThreadFun::getEnv() {
    LOGE("JniThreadFun getEnv");
    JNIEnv *env = nullptr;
    int status = m_vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (status == JNI_EDETACHED) {
        if (m_vm->AttachCurrentThread(&env, nullptr) != 0) {
            return nullptr;
        }
    }
    return env;
}

void JniThreadFun::run_task(void(*execute)(JNIEnv *env, jobject& params, jobject& callback)) {

    lock_guard<mutex> lock(m_mutex);  // 自动加锁解锁
    LOGE("JniThreadFun run_task");
    // 临界区代码
    JNIEnv *env = getEnv();
    if (!env) return;
    execute(env, params,callback);
    maybeDetach();
}

void JniThreadFun::maybeDetach() {
    LOGE("JniThreadFun maybeDetach");
    JNIEnv *env = nullptr;
    if (m_vm->GetEnv((void **) &env, JNI_VERSION_1_6) == JNI_EDETACHED) {
        m_vm->DetachCurrentThread();
    }
}

#endif //OPENCV_JNITHREADFUN_H
