package com.yaxiu.opencv

import android.graphics.Bitmap

class Mat : AutoCloseable {

    var cols: Int = 0
    var rows: Int = 0
    private var jniMatAds: Long = 0;

    constructor() {
        jniMatAds = nCrateMat()

    }

    constructor(rows: Int, cols: Int, type: Int) {
        this.rows = rows
        this.cols = cols
        jniMatAds = nCrateMatIII(rows, cols, type)

    }

    fun bitmap2mat(bitmap: Bitmap) {
        if (jniMatAds != 0L) {
            nBitmap2mat(jniMatAds, bitmap)
        }else{
            println("jniMatAds 未初始化")
        }

    }


    override fun close() {
        if (jniMatAds != 0L) {
            nReleaseMat(jniMatAds)
            jniMatAds = 0L
        }
    }

    private external fun nReleaseMat(jniMatAds: Long)

    private external fun nCrateMatIII(rows: Int, cols: Int, type: Int): Long

    private external fun nCrateMat(): Long

    private external fun nBitmap2mat(jniMatAds: Long, bitmap: Bitmap)

}