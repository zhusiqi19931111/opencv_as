package com.yaxiu.opencv

import android.graphics.Bitmap

class Mat : AutoCloseable {

    var cols: Int = 0
    var rows: Int = 0
    var type = CVType.CV_32FC1
    var jniMatAds: Long = 0;


    constructor() {
        jniMatAds = FaceDetection.instance.nCrateMat()

    }

    constructor(rows: Int, cols: Int, type: CVType) {
        this.rows = rows
        this.cols = cols
        this.type = type
        jniMatAds = FaceDetection.instance.nCrateMatIII(rows, cols, type.value)
        println("Mat rows:$rows cols:$cols type:$type")
    }

    fun bitmap2mat(bitmap: Bitmap) {
        if (jniMatAds != 0L) {
            FaceDetection.instance.nBitmap2mat(jniMatAds, bitmap)
        } else {
            println("Mat 未初始化")
        }

    }

    fun  at(i0: Int, i1: Int, value: Float) {
        if(type != CVType.CV_32FC1){
            throw  UnsupportedOperationException("Provider value nonsupport, please check CVType.");
        }
        if (jniMatAds != 0L) {
            FaceDetection.instance.nat(jniMatAds, i0, i1, value)
        } else {
            println("Mat 未初始化")
        }

    }

    fun warpAffine(dst: Long, m: Long, bitmap: Bitmap) {
        if (jniMatAds != 0L) {
            FaceDetection.instance.nWarpAffine(jniMatAds, dst, m,bitmap)
        } else {
            println("Mat 未初始化")
        }
    }

    override fun close() {
        if (jniMatAds != 0L) {
            FaceDetection.instance.nReleaseMat(jniMatAds)
            jniMatAds = 0L
            println("Mat 释放指针")
        }
    }



}