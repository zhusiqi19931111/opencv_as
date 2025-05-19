package com.yaxiu.opencv

import android.os.Handler
import android.os.Looper

interface CallbackInMainThread {
    val handler: Handler
        get() = Handler(Looper.getMainLooper())

    fun executeOnMain(result: Any){
        handler.post {
            resultCallback(result)
        }
    }

    fun resultCallback(result: Any)
}