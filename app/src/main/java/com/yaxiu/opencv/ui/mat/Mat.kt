package com.yaxiu.opencv.ui.mat

import android.graphics.Bitmap
import android.graphics.Canvas
import android.graphics.ColorMatrix
import android.graphics.ColorMatrixColorFilter
import android.graphics.Paint
import android.os.Build
import android.util.Log
import androidx.annotation.RequiresApi
import androidx.core.graphics.createBitmap


class Mat {
    /**
     * 方式1 使用ColorMatrix 实现灰度图
     */
    fun colorMatrix(bitmapSrc: Bitmap): Bitmap {
        val bitmapDes = createBitmap(bitmapSrc.width, bitmapSrc.height, bitmapSrc.config)
        val canvas = Canvas(bitmapDes)
        val paint = Paint().apply {

            isAntiAlias = true
            isDither = true

            // 方式1 使用 setSaturation 设置灰色
            //val matrix = ColorMatrix()
           // matrix.setSaturation(0f)
            //方式2 使用ColorMatrix 自定义矩阵值设置灰色
            /**
             *   final float invSat = 1;
             *         final float R = 0.213f * invSat;
             *         final float G = 0.715f * invSat;
             *         final float B = 0.072f * invSat;
             *
             *         m[0] = R + sat; m[1] = G;       m[2] = B;
             *         m[5] = R;       m[6] = G + sat; m[7] = B;
             *         m[10] = R;      m[11] = G;      m[12] = B + sat;
             */
            val matrixGrey = ColorMatrix(floatArrayOf(
                0.213f, 0.715f,0.072f,0f,0f,
                0.213f, 0.715f,0.072f,0f,0f,
                0.213f, 0.715f,0.072f,0f,0f,
                0f,0f,0f,1f,0f,
            ))
            //原图
            val matrix= ColorMatrix(floatArrayOf(
                1f,0f,0f,0f,0f,
                0f,1f,0f,0f,0f,
                0f,0f,1f,0f,0f,
                0f,0f,0f,1f,0f,
            ))
            //底片
            val matrixNegative = ColorMatrix(floatArrayOf(
                -1f,0f,0f,0f,255f,
                0f,-1f,0f,0f,255f,
                0f,0f,-1f,0f,255f,
                0f,0f,0f,1f,0f,
            ))

            //亮色
            val matrixLight = ColorMatrix(floatArrayOf(
                1.2f,0f,0f,0f,0f,
                0f,1f,0f,0f,0f,
                0f,0f,1f,0f,0f,
                0f,0f,0f,1f,0f,
            ))

            colorFilter = ColorMatrixColorFilter(matrixLight)
        }


        canvas.drawBitmap(bitmapSrc, 0f, 0f, paint)
        return bitmapDes

    }


    /**
     * 方式2 bitmap 获取像素操作
     */
    @RequiresApi(Build.VERSION_CODES.O)
    fun setBitmapPixel(bitmapSrc: Bitmap): Bitmap {


        val width = bitmapSrc.width
        val height = bitmapSrc.height

        val pixels = IntArray(width * height)
        bitmapSrc.getPixels(pixels, 0, width, 0, 0, width, height)
        val channelCount = getBitmapChannel(bitmapSrc)

        if(channelCount==4) {  //当前像素，8888 第一个8 >> 右移24 位， 第二个8 右移16 位 ，第三个8右移8位
            for (i in pixels.indices) {

                val pixel = pixels[i];

                val a = (pixel shr 24) and 0xFF
                val r = (pixel shr 16) and 0xFF
                val g = (pixel shr 8) and 0xFF
                val b = pixel and 0xFF
                //设置灰色
                val grey = (0.213f * r + 0.715f * g + 0.072f * b).toInt()
                pixels[i] = (a shl 24) or (grey shl 16) or (grey shl 8) or grey
                // 设置黑白色
                /* val black_white=if((r+g+b)/3>125){
                     255
                 }else{
                     0
                 }
                 pixels[i] = (a shl 24) or (black_white shl 16) or (black_white shl 8) or black_white*/

            }
        }else if(channelCount==3){//当前像素，R 5 G 6 b5 第一个5 >> 右移11 位， 第二个6 右移5 位 R 8 G 8 B8 A8 565
            for (i in pixels.indices) {
                val pixel = pixels[i]
                val r = ((pixel shr 11) and 0x1F) shl 3
                val g = ((pixel shr 5) and 0x3F) shl 2
                val b = (pixel and 0x1F) shl 3
               // val grey = (0.213f * r + 0.715f * g + 0.072f * b).toInt() 均可使用
                val grey = (0.299f * r + 0.587f * g + 0.114f * b).toInt()//此公式只适用于4通道必须左移补齐8位
                pixels[i] = ((grey shr 3) shl 11) or ((grey shr 2) shl 5) or (grey shr 3)//此公式只适用于4通道必须右移补齐8位*/
            }
        }


        //方式1 重新申请新的bitmap
        val bitmapDes = createBitmap(width, height,bitmapSrc.config) //
        bitmapDes.setPixels(pixels, 0, width, 0, 0, width, height)
        return bitmapDes;
        //方式2 option.inMutable=true 在原有的bitmap 上修改，需要修改
       // bitmapSrc.setPixels(pixels, 0, width, 0, 0, width, height)
       // return bitmapSrc;
    }

    fun convertToGrayscaleFromRGB565(original: Bitmap): Bitmap {
        require(original.config == Bitmap.Config.RGB_565) { "Bitmap must be RGB_565 format" }

        val width = original.width
        val height = original.height
        val grayscaleBitmap = createBitmap(width, height)


        // 使用更高效的方式处理
        val pixels = IntArray(width * height)
        original.getPixels(pixels, 0, width, 0, 0, width, height)

        for (i in pixels.indices) {
            val pixel = pixels[i]


            // 直接处理RGB565格式 (当Bitmap是RGB565时，getPixels()返回的格式)
            // RGB565在内存中的布局: RRRRRGGG GGGBBBBB
            var r = ((pixel shr 8) and 0xF800) shr 11 // 取高5位 (红色)
            var g = ((pixel shr 5) and 0x07E0) shr 5 // 取中间6位 (绿色)
            var b = (pixel and 0x001F) // 取低5位 (蓝色)


            // 扩展到8位
            r = (r shl 3) or (r shr 2)
            g = (g shl 2) or (g shr 4)
            b = (b shl 3) or (b shr 2)


            // 计算灰度值
            val gray = (0.299 * r + 0.587 * g + 0.114 * b).toInt()


            // 设置灰度像素 (不透明)
            pixels[i] = -0x1000000 or (gray shl 16) or (gray shl 8) or gray
        }

        grayscaleBitmap.setPixels(pixels, 0, width, 0, 0, width, height)
        return grayscaleBitmap
    }

   @RequiresApi(Build.VERSION_CODES.O)
   fun getBitmapChannel(bitmap: Bitmap):Int{
        val config=bitmap.getConfig()
        val channelCount = when (config) {
            Bitmap.Config.ALPHA_8 -> 1 // 只有 Alpha 通道
            Bitmap.Config. RGB_565 -> 3 // RGB 三通道
            Bitmap.Config.ARGB_4444,  Bitmap.Config.ARGB_8888,  Bitmap.Config.RGBA_F16 -> 4 // ARGB 四通道
            else -> 4 // 默认按 ARGB_8888 处理
        }
        Log.d("ChannelCount", "通道数: $channelCount")
       return  channelCount
    }
}