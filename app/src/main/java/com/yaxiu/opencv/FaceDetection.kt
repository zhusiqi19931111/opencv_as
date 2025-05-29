package com.yaxiu.opencv

import android.graphics.Bitmap
import java.io.FileNotFoundException

class FaceDetection {


    companion object {
        // Used to load the 'opencv' library on application startup.
        init {
            System.loadLibrary("face_detection")
        }

        val instance: FaceDetection by lazy {
            FaceDetection();
        }
    }

    external fun loadCascade(absolutePath: String)
    external fun faceDetectionSaveInfo(faceBitmap: Bitmap)
    external fun opencvUpdateMat(absolutePath: String, outpath: String)
    external fun nativeBitmapPixel(bitmap: Bitmap)

    @Throws(FileNotFoundException::class)
    external fun createBitmapByDecodeFile(path: String): Bitmap

    @Throws(FileNotFoundException::class)
    external fun matObj()

    @Throws(FileNotFoundException::class)
    external fun opencvEditPiexl(path: String): Bitmap
    external fun loadBitmapEditPiexl(bitmap: Bitmap, callback: Runnable)

    @Throws(FileNotFoundException::class)
    external fun opencvAddWeight(s: String, s1: String): Bitmap

    @Throws(FileNotFoundException::class)
    external fun opencvSaturationBrightnessContrast(s: String): Bitmap
    external fun magnifierSpecialEffects(bitmap: Bitmap, param: Runnable)
    external fun fishEyeSpecialEffects(bitmap: Bitmap, param: Runnable)
    external fun oilPaintingSpecialEffects(bitmap: Bitmap, param: Runnable)
    external fun glassSpecialEffects(bitmap: Bitmap, param: Runnable)
    external fun inverseWorldSpecialEffects(bitmap: Bitmap, param: Runnable)
    external fun mirrorSpecialEffects(bitmap: Bitmap, param: Runnable)
    external fun mosaicSpecialEffects(bitmap: Bitmap, param: Runnable)
    external fun reliefSpecialEffects(bitmap: Bitmap, param: Runnable)
    external fun faceMosaicSpecialEffects(fileName: String, bitmap: Bitmap, param: Runnable)
    external fun clipSpecialEffects(bitmap: Bitmap, param: CallbackInMainThread)
    external fun moveMagnifierSpecialEffects(x: Float, y: Float, bitmap: Bitmap, param: Runnable)
    external fun rotateImage(bitmap: Bitmap, runnable: CallbackInMainThread)
    external fun matrixTransform(bitmap: Bitmap, runnable: Runnable)
    external fun reSize(bitmap: Bitmap, runnable: CallbackInMainThread)
    external fun remap(bitmap: Bitmap, runnable: Runnable)


    external fun nReleaseMat(jniMatAds: Long)
    external fun nCrateMatIII(rows: Int, cols: Int, type: Int): Long
    external fun nCrateMat(): Long
    external fun nBitmap2mat(jniMatAds: Long, bitmap: Bitmap)
    external fun nat(jniMatAds: Long, i0: Int, i1: Int, value: Float)
    external fun nWarpAffine(jniMatAds: Long, dstMatAds: Long, mMatAds: Long, bitmap: Bitmap)


    external fun equalizeHist(bitmap: Bitmap, runnable: Runnable)
    external fun calcuHist(bitmap: Bitmap, runnable: CallbackInMainThread)
    external fun matLight(bitmap: Bitmap, runnable: Runnable)
    external fun codeVerification(bitmap: Bitmap,callbackInMainThread: CallbackInMainThread)
    external fun codeTiltVerification(bitmap: Bitmap, callbackInMainThread: CallbackInMainThread)
    external fun codeRoundVerification(bitmap: Bitmap?, callbackInMainThread: CallbackInMainThread)
    external fun lbp(bitmap: Bitmap, callbackInMainThread: CallbackInMainThread)
    external fun hog(bitmap: Bitmap, runnable: Runnable)
    external fun haar(bitmap: Bitmap, callbackInMainThread: CallbackInMainThread)
}