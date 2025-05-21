//
// Created by cherish on 2025/5/13.
//

#ifndef OPENCV_JNITHREADFUN_HPP
#define OPENCV_JNITHREADFUN_HPP

#include <jni.h>
#include <mutex>
#include <thread>
#include <queue>
#include "log.h"

using namespace std;

class JniThreadFun {
private:
    JavaVM *m_vm;
    shared_ptr<mutex> m_mutex = make_shared<mutex>();//用智能指针共享,即使对象销毁，mutex 也不会立即释放，避免线程崩溃。
    jobject params;
    jobject callback;

public:
    JniThreadFun(JavaVM *vm, JNIEnv *env, jobject &params, jobject &callback);

    //禁止拷贝
    JniThreadFun(const JniThreadFun &fun) = delete;

    JniThreadFun &operator=(const JniThreadFun &) = delete;

    ~JniThreadFun();

    void
    run_task(void(*execute)(JNIEnv *env, JniThreadFun *self, jobject &params, jobject callback));

    void run_task_globle(void(*execute)(JNIEnv *env, jobject &params, jobject callback));

    JNIEnv *getEnv();

    friend void
    executeJavaOnMainWithRelease(JNIEnv *env, JniThreadFun *self, jobject callback);

    friend void
    executeJavaOnMain(JNIEnv *env, jobject callback);

    friend void
    executeJavaOnMainWithReleaseAndResult(JNIEnv *env, JniThreadFun *self, jobject &bitmap,
                                          jobject callback);

    friend void
    executeJavaOnMainWithResult(JNIEnv *env, jobject &bitmap, jobject callback);

private:
    void maybeDetach(JNIEnv *env);

};

JniThreadFun::JniThreadFun(JavaVM *vm, JNIEnv *env, jobject &params, jobject &callback) : m_vm(vm),
                                                                                          params(env->NewGlobalRef(
                                                                                                  params)),
                                                                                          callback(
                                                                                                  env->NewGlobalRef(
                                                                                                          callback)) {
    LOGE("JniThreadFun create");
}


JniThreadFun::~JniThreadFun() {
    JNIEnv *env = getEnv();
    if (env) {
        LOGE("~JniThreadFun");
        env->DeleteGlobalRef(params);
        env->DeleteGlobalRef(callback);
        maybeDetach(env);
    } else {
        LOGE("~JniThreadFun env = null ");
    }

}

JNIEnv *JniThreadFun::getEnv() {
    JNIEnv *env = nullptr;
    int status = m_vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (status == JNI_EDETACHED) {
        LOGE("JniThreadFun AttachCurrentThread");
        if (m_vm->AttachCurrentThread(&env, nullptr) != 0) {
            return nullptr;
        }
    }

    return env;
}

void JniThreadFun::run_task(
        void(*execute)(JNIEnv *env, JniThreadFun *self, jobject &params, jobject callback)) {

    lock_guard<mutex> lock(*m_mutex);  // 自动加锁解锁
    LOGE("JniThreadFun run_task");
    // 临界区代码
    JNIEnv *env = getEnv();
    if (!env) return;
    execute(env, this, params, callback);
}

void executeJavaOnMainWithRelease(JNIEnv *env, JniThreadFun *self, jobject callback) {
    const char *className = "android/os/Handler";
    jclass handlerClass = env->FindClass(className);
    if (handlerClass == nullptr) {
        return;
    }
    jmethodID constructor = env->GetMethodID(handlerClass, "<init>", "(Landroid/os/Looper;)V");
    if (constructor == nullptr) {
        env->DeleteLocalRef(handlerClass);
        return; // 方法找不到
    }
    jclass lopperClass = env->FindClass("android/os/Looper");
    if (lopperClass == nullptr) {
        return;
    }
    jmethodID getMainLooper = env->GetStaticMethodID(lopperClass, "getMainLooper",
                                                     "()Landroid/os/Looper;");
    if (getMainLooper == nullptr) {
        env->DeleteLocalRef(lopperClass);
        return; // 方法找不到
    }
    jobject lopper = env->CallStaticObjectMethod(lopperClass, getMainLooper);

    jobject handler = env->NewObject(handlerClass, constructor, lopper);

    jmethodID post = env->GetMethodID(handlerClass, "post", "(Ljava/lang/Runnable;)Z");

    // 5. 提交到主线程
    env->CallBooleanMethod(handler, post, callback);

    env->DeleteLocalRef(lopper);
    env->DeleteLocalRef(handler);
    env->DeleteLocalRef(lopperClass);
    env->DeleteLocalRef(handlerClass);
    if (self) {
        delete self;
        self = nullptr;
        LOGE("delete jniThreadFun");
    }

}

void executeJavaOnMainWithReleaseAndResult(JNIEnv *env, JniThreadFun *self, jobject &bitmap,
                                           jobject callback) {
    jclass callbackInMainThreadClazz = env->GetObjectClass(callback);
    jmethodID methodId = env->GetMethodID(callbackInMainThreadClazz, "executeOnMain",
                                          "(Ljava/lang/Object;)V");
    env->CallVoidMethod(callback,methodId,bitmap);
    env->DeleteLocalRef(callbackInMainThreadClazz);
    if (self) {
        delete self;
        self = nullptr;
        LOGE("delete jniThreadFun");
    }

}

void executeJavaOnMain(JNIEnv *env, jobject callback) {
    const char *className = "android/os/Handler";
    jclass handlerClass = env->FindClass(className);
    if (handlerClass == nullptr) {
        return;
    }
    jmethodID constructor = env->GetMethodID(handlerClass, "<init>", "(Landroid/os/Looper;)V");
    if (constructor == nullptr) {
        env->DeleteLocalRef(handlerClass);
        return; // 方法找不到
    }
    jclass lopperClass = env->FindClass("android/os/Looper");
    if (lopperClass == nullptr) {
        return;
    }
    jmethodID getMainLooper = env->GetStaticMethodID(lopperClass, "getMainLooper",
                                                     "()Landroid/os/Looper;");
    if (getMainLooper == nullptr) {
        env->DeleteLocalRef(lopperClass);
        return; // 方法找不到
    }
    jobject lopper = env->CallStaticObjectMethod(lopperClass, getMainLooper);

    jobject handler = env->NewObject(handlerClass, constructor, lopper);

    jmethodID post = env->GetMethodID(handlerClass, "post", "(Ljava/lang/Runnable;)Z");

    // 5. 提交到主线程
    env->CallBooleanMethod(handler, post, callback);

    env->DeleteLocalRef(lopper);
    env->DeleteLocalRef(handler);
    env->DeleteLocalRef(lopperClass);
    env->DeleteLocalRef(handlerClass);

}

void executeJavaOnMainWithResult(JNIEnv *env, jobject &bitmap, jobject callback) {
    jclass callbackInMainThreadClazz = env->GetObjectClass(callback);
    jmethodID methodId = env->GetMethodID(callbackInMainThreadClazz, "executeOnMain",
                                          "(Ljava/lang/Object;)V");
    env->CallVoidMethod(callback,methodId,bitmap);
    env->DeleteLocalRef(callbackInMainThreadClazz);

}


void JniThreadFun::run_task_globle(
        void(*execute)(JNIEnv *env, jobject &params, jobject callback)) {

    lock_guard<mutex> lock(*m_mutex);  // 自动加锁解锁
    LOGE("JniThreadFun run_task");
    // 临界区代码
    JNIEnv *env = getEnv();
    if (!env) return;
    execute(env, params, callback);
    maybeDetach(env);
}


void JniThreadFun::maybeDetach(JNIEnv *env) {
    if (env) {
        m_vm->DetachCurrentThread();
        env = nullptr;
        LOGE("JniThreadFun DetachCurrentThread");
    }
}

#endif //OPENCV_JNITHREADFUN_HPP
