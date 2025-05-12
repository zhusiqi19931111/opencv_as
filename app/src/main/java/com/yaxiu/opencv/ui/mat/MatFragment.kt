package com.yaxiu.opencv.ui.mat

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Build
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.annotation.RequiresApi
import androidx.fragment.app.Fragment
import androidx.lifecycle.ViewModelProvider
import com.yaxiu.opencv.FaceDetection
import com.yaxiu.opencv.R
import com.yaxiu.opencv.databinding.FragmentGalleryBinding


class MatFragment : Fragment() {

    private var _binding: FragmentGalleryBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    private val mat = Mat()

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        val galleryViewModel =
            ViewModelProvider(this).get(GalleryViewModel::class.java)

        _binding = FragmentGalleryBinding.inflate(inflater, container, false)
        val root: View = binding.root

        initView(binding);


        return root
    }

    @RequiresApi(Build.VERSION_CODES.O)
    private fun initView(binding: FragmentGalleryBinding) {
        val option=BitmapFactory.Options()
        option.inMutable = true;// 代表bitmap 可以复用，图片操作时无需新建一个bitmap 对象，在原有的基础上修改像素颜色值即可
        option.inPreferredConfig = Bitmap.Config.RGB_565 //修改图片的颜色通道数
        val bitmap = BitmapFactory.decodeResource(resources, R.drawable.girl,option)
        binding.img1.setImageBitmap(bitmap)
        colorMatrix(bitmap,binding)
        setBitmapPixel(bitmap,binding)
        nativeBitmapPixel(bitmap,binding)


    }

    private fun nativeBitmapPixel(bitmap: Bitmap, binding: FragmentGalleryBinding) {
        FaceDetection.instance.nativeBitmapPixel(bitmap)
        binding.img4.setImageBitmap(bitmap)

    }

    @RequiresApi(Build.VERSION_CODES.O)
    private fun setBitmapPixel(bitmap: Bitmap, binding: FragmentGalleryBinding) {
        //方式1 算法不一致
        val  original = bitmap.copy(Bitmap.Config.RGB_565, false)
        //binding.img3.setImageBitmap( mat.setBitmapPixel(original))

        // 方式2加载原始图片 (确保是RGB565格式)
       // 转换为灰度图
        val grayscale: Bitmap = mat.convertToGrayscaleFromRGB565(original)
       // 显示结果
        binding.img3.setImageBitmap(grayscale)
    }

    private fun colorMatrix(bitmap: Bitmap, binding: FragmentGalleryBinding) {
        val bitmapConvert = mat.colorMatrix(bitmap)
        binding.img2.setImageBitmap(bitmapConvert)
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}