package com.yaxiu.opencv.ui.qqeffects

import android.annotation.SuppressLint
import android.graphics.BitmapFactory
import android.os.Bundle
import android.view.LayoutInflater
import android.view.MotionEvent
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.fragment.app.Fragment
import com.yaxiu.opencv.FaceDetection
import com.yaxiu.opencv.R
import com.yaxiu.opencv.databinding.FragmentQQEffectsBinding
import java.io.File

class QQEffectsFragment : Fragment(), View.OnClickListener {

    private var _binding: FragmentQQEffectsBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {

        _binding = FragmentQQEffectsBinding.inflate(inflater, container, false)
        val bitmap =
            BitmapFactory.decodeFile("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg")
        binding.img1.setImageBitmap(bitmap)
        val root: View = binding.root
        binding.btn1.setOnClickListener(this)
        binding.btn2.setOnClickListener(this)
        binding.btn3.setOnClickListener(this)
        binding.btn4.setOnClickListener(this)
        binding.btn5.setOnClickListener(this)
        binding.btn6.setOnClickListener(this)
        binding.btn7.setOnClickListener(this)
        binding.btn8.setOnClickListener(this)
        binding.btn9.setOnClickListener(this)
        binding.btn10.setOnClickListener(this)

        return root
    }

    override fun onClick(v: View) {
        binding.img2.setOnTouchListener(null)
        when (v.id) {
            R.id.btn1 -> {
                reliefSpecialEffects()
            }

            R.id.btn2 -> {
                mosaicSpecialEffects()
            }

            R.id.btn3 -> {
                mirrorSpecialEffects()
            }

            R.id.btn4 -> {
                inverseWorldSpecialEffects()
            }

            R.id.btn5 -> {
                glassSpecialEffects()
            }

            R.id.btn6 -> {
                oilPaintingSpecialEffects()
            }

            R.id.btn7 -> {
                fishEyeSpecialEffects()
            }

            R.id.btn8 -> {
                magnifierSpecialEffects()
            }
            R.id.btn9->{
                faceMosaicSpecialEffects()
            }
            R.id.btn10->{
                clipSpecialEffects()
            }

        }
    }

    private fun clipSpecialEffects() {
        val file=
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if(!file.exists()){
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.clipSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img2.setImageBitmap(bitmap)
                binding.tvLab2.text="裁剪特效";
            }

        })
    }

    private fun faceMosaicSpecialEffects() {
        val file=
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/lbpcascade_frontalface.xml");
        val file1=
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/face.jpg");
        if(!file.exists()||!file1.exists()){
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file1.absolutePath)
        FaceDetection.instance.faceMosaicSpecialEffects(file.absolutePath,bitmap, object : Runnable {
            override fun run() {
                binding.img4.setImageBitmap(bitmap)
                binding.tvLab4.text="面部马赛克特效";
            }

        })
    }

    @SuppressLint("ClickableViewAccessibility")
    private fun magnifierSpecialEffects() {

        val file=
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if(!file.exists()){
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        binding.img2.setOnTouchListener { v, event ->
            FaceDetection.instance.moveMagnifierSpecialEffects(
                event.x,
                event.y,
                bitmap
            ) {
                binding.img2.setImageBitmap(bitmap)
                binding.tvLab2.text = "放大镜特效";
            }
            true
        }


        FaceDetection.instance.magnifierSpecialEffects(bitmap, object : Runnable {
              override fun run() {
                  binding.img2.setImageBitmap(bitmap)
                  binding.tvLab2.text="放大镜特效";
              }

          })
    }

    private fun fishEyeSpecialEffects() {
        val file=
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if(!file.exists()){
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.fishEyeSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img3.setImageBitmap(bitmap)
                binding.tvLab3.text="鱼眼特效";
            }

        })
    }

    private fun oilPaintingSpecialEffects() {
        val file=
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if(!file.exists()){
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.oilPaintingSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img4.setImageBitmap(bitmap)
                binding.tvLab4.text="油画特效";
            }

        })
    }

    private fun glassSpecialEffects() {
        val file=
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if(!file.exists()){
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.glassSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img2.setImageBitmap(bitmap)
                binding.tvLab2.text="毛玻璃特效";
            }

        })
    }

    private fun inverseWorldSpecialEffects() {
        val file=
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if(!file.exists()){
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.inverseWorldSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img3.setImageBitmap(bitmap)
                binding.tvLab3.text="逆世界特效";
            }

        })
    }

    private fun mirrorSpecialEffects() {
        val file=
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if(!file.exists()){
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.mirrorSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img4.setImageBitmap(bitmap)
                binding.tvLab4.text="镜像特效";
            }

        })
    }

    private fun mosaicSpecialEffects() {
        val file=
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if(!file.exists()){
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.mosaicSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img2.setImageBitmap(bitmap)
                binding.tvLab2.text="马赛克特效";
            }

        })
    }

    private fun reliefSpecialEffects() {
        val file=
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if(!file.exists()){
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.reliefSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img3.setImageBitmap(bitmap)
                binding.tvLab3.text="浮雕特效";
            }

        })
    }
}