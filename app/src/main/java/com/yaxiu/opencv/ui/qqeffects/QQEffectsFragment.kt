package com.yaxiu.opencv.ui.qqeffects

import android.annotation.SuppressLint
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.view.LayoutInflater
import android.view.MotionEvent
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.fragment.app.Fragment
import com.yaxiu.opencv.CallbackInMainThread
import com.yaxiu.opencv.FaceDetection
import com.yaxiu.opencv.R
import com.yaxiu.opencv.databinding.FragmentQQEffectsBinding
import java.io.File
import java.util.Random
import java.util.Timer
import java.util.TimerTask

class QQEffectsFragment : Fragment(), View.OnClickListener {

    private var timer: Timer? = null
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
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()

        } else {
            val bitmap =
                BitmapFactory.decodeFile(file.absolutePath)
            binding.img1.setImageBitmap(bitmap)
        }

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
        binding.btn11.setOnClickListener(this)
        binding.btn12.setOnClickListener(this)
        binding.btn13.setOnClickListener(this)
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

            R.id.btn9 -> {
                faceMosaicSpecialEffects()
            }

            R.id.btn10 -> {
                clipSpecialEffects()
            }
            R.id.btn11->{
                codeSquareVerification()
            }
            R.id.btn12->{
                codeTiltVerification()
            }
            R.id.btn13->{
                codeRoundVerification()
            }
        }
    }

    private fun codeRoundVerification() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/code3.jpg")
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        binding.img1.setImageBitmap(bitmap)
        binding.tvLab1.text = "圆形二维码";
        println("clipSpecialEffects pre bitmap width=${bitmap.width} height=${bitmap.height}")
        FaceDetection.instance.codeRoundVerification(bitmap, object : CallbackInMainThread {
            override fun resultCallback(result: Any) {
                val newBitmap = result as Bitmap
                println("codeVerification last bitmap width=${newBitmap.width} height=${newBitmap.height}")
                binding.img1.setImageBitmap(newBitmap)
                binding.tvLab1.text = "圆形二维码验证裁剪"
            }


        })
    }

    private fun codeTiltVerification() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/code.png")
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        binding.img1.setImageBitmap(bitmap)
        binding.tvLab1.text = "倾斜二维码";
        println("clipSpecialEffects pre bitmap width=${bitmap.width} height=${bitmap.height}")
        FaceDetection.instance.codeTiltVerification(bitmap, object : CallbackInMainThread {
            override fun resultCallback(result: Any) {
                val newBitmap = result as Bitmap
                println("codeVerification last bitmap width=${newBitmap.width} height=${newBitmap.height}")
                binding.img1.setImageBitmap(newBitmap)
                binding.tvLab1.text = "倾斜二维码验证裁剪"
            }


        })
    }

    private fun codeSquareVerification() {

        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/code1.jpg")
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        binding.img1.setImageBitmap(bitmap)
        binding.tvLab1.text = "方形二维码";
        println("clipSpecialEffects pre bitmap width=${bitmap.width} height=${bitmap.height}")
        FaceDetection.instance.codeVerification(bitmap, object : CallbackInMainThread {
            override fun resultCallback(result: Any) {
                val newBitmap = result as Bitmap
                println("codeVerification last bitmap width=${newBitmap.width} height=${newBitmap.height}")
                binding.img1.setImageBitmap(newBitmap)
                binding.tvLab1.text = "方形二维码验证裁剪"
            }


        })
    }

    private fun clipSpecialEffects() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }

        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        println("clipSpecialEffects pre bitmap width=${bitmap.width} height=${bitmap.height}")
        FaceDetection.instance.clipSpecialEffects(bitmap, object : CallbackInMainThread {
            override fun resultCallback(result: Any) {
                val newBitmap = result as Bitmap
                println("clipSpecialEffects pre bitmap width=${newBitmap.width} height=${newBitmap.height}")
                binding.img2.setImageBitmap(newBitmap)
                binding.tvLab2.text = "裁剪特效";
            }


        })
    }

    private fun faceMosaicSpecialEffects() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/lbpcascade_frontalface.xml");
        val file1 =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/face.jpg");
        if (!file.exists() || !file1.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file1.absolutePath)
        FaceDetection.instance.faceMosaicSpecialEffects(
            file.absolutePath,
            bitmap,
            object : Runnable {
                override fun run() {
                    binding.img4.setImageBitmap(bitmap)
                    binding.tvLab4.text = "面部马赛克特效";
                }

            })
    }

    @SuppressLint("ClickableViewAccessibility")
    private fun magnifierSpecialEffects() {

        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        binding.img2.setOnTouchListener { v, event ->
            when (event.action) {
                MotionEvent.ACTION_DOWN -> {
                    releaseTimer()
                    FaceDetection.instance.moveMagnifierSpecialEffects(
                        event.x,
                        event.y,
                        bitmap
                    ) {
                        println("moveMagnifierSpecialEffects finish")
                        binding.img2.setImageBitmap(bitmap)
                        binding.tvLab2.text = "放大镜特效"
                    }
                }

            }
            true
        }

        timer = Timer()
        timer?.schedule(object : TimerTask() {
            override fun run() {
                val random = Random().nextInt(100)
                println("moveMagnifierSpecialEffects Timer random:$random")
                FaceDetection.instance.moveMagnifierSpecialEffects(
                    10 * random.toFloat(),
                    10 * random.toFloat(),
                    bitmap
                ) {
                    println("moveMagnifierSpecialEffects Timer finish")
                    binding.img2.setImageBitmap(bitmap)
                    binding.tvLab2.text = "放大镜特效";
                }
            }
        }, 1_000, 10_000)


    }

    private fun releaseTimer() {
        timer?.cancel()
        timer=null;
    }

    private fun fishEyeSpecialEffects() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.fishEyeSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img3.setImageBitmap(bitmap)
                binding.tvLab3.text = "鱼眼特效";
            }

        })
    }

    private fun oilPaintingSpecialEffects() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/2188631868,32000639.webp");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.oilPaintingSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img4.setImageBitmap(bitmap)
                binding.tvLab4.text = "油画特效";
            }

        })
    }

    private fun glassSpecialEffects() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.glassSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img2.setImageBitmap(bitmap)
                binding.tvLab2.text = "毛玻璃特效";
            }

        })
    }

    private fun inverseWorldSpecialEffects() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.inverseWorldSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img3.setImageBitmap(bitmap)
                binding.tvLab3.text = "逆世界特效";
            }

        })
    }

    private fun mirrorSpecialEffects() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.mirrorSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img4.setImageBitmap(bitmap)
                binding.tvLab4.text = "镜像特效";
            }

        })
    }

    private fun mosaicSpecialEffects() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.mosaicSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img2.setImageBitmap(bitmap)
                binding.tvLab2.text = "马赛克特效";
            }

        })
    }

    private fun reliefSpecialEffects() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.reliefSpecialEffects(bitmap, object : Runnable {
            override fun run() {
                binding.img3.setImageBitmap(bitmap)
                binding.tvLab3.text = "浮雕特效";
            }

        })
    }

    override fun onDestroy() {
        super.onDestroy()
        releaseTimer()
    }
}