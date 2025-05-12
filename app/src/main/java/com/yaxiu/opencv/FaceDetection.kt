package com.yaxiu.opencv

import android.graphics.Bitmap

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
   external  fun nativeBitmapPixel(bitmap: Bitmap)
   external  fun matObj()
}