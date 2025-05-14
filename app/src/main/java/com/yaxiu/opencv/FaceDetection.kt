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
    external fun opencvEditPiexl(path: String):Bitmap
    external fun loadBitmapEditPiexl(bitmap: Bitmap, callback: Runnable)
    @Throws(FileNotFoundException::class)
    external fun opencvAddWeight(s: String, s1: String): Bitmap
    @Throws(FileNotFoundException::class)
    external fun opencvSaturationBrightnessContrast(s: String): Bitmap
    external fun magnifierSpecialEffects(bitmap: Bitmap)
    external fun fishEyeSpecialEffects(bitmap: Bitmap)
    external fun oilPaintingSpecialEffects(bitmap: Bitmap)
    external fun glassSpecialEffects(bitmap: Bitmap)
    external fun inverseWorldSpecialEffects(bitmap: Bitmap)
    external fun mirrorSpecialEffects(bitmap: Bitmap)
    external fun mosaicSpecialEffects(bitmap: Bitmap)
    external fun reliefSpecialEffects(bitmap: Bitmap)
}