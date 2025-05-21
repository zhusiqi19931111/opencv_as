package com.yaxiu.opencv.ui.principle

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.fragment.app.Fragment
import com.yaxiu.opencv.CVType
import com.yaxiu.opencv.CallbackInMainThread
import com.yaxiu.opencv.FaceDetection
import com.yaxiu.opencv.Mat
import com.yaxiu.opencv.R
import com.yaxiu.opencv.databinding.FragmentPrincipleBinding
import java.io.File


class PrincipleFragment : Fragment(), View.OnClickListener {

    private var _binding: FragmentPrincipleBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {

        _binding = FragmentPrincipleBinding.inflate(inflater, container, false)

        val root: View = binding.root
        binding.btn1.setOnClickListener(this)
        binding.btn2.setOnClickListener(this)
        binding.btn3.setOnClickListener(this)
        binding.btn4.setOnClickListener(this)
        binding.btn5.setOnClickListener(this)
        binding.btn6.setOnClickListener(this)
        binding.btn7.setOnClickListener(this)
        binding.btn8.setOnClickListener(this)

        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()

        } else {
            val bitmap =
                BitmapFactory.decodeFile(file.absolutePath)
            binding.img1.setImageBitmap(bitmap)
            binding.tvLab1.text = "原图";
        }
        return root
    }

    override fun onClick(v: View) {

        when (v.id) {
            R.id.btn1 -> {
                rotateImage()
            }

            R.id.btn2 -> {
                matrixTransform()
            }

            R.id.btn3 -> {
                reSize()
            }

            R.id.btn4 -> {
                remap()
            }

            R.id.btn5 -> {
                opencvMap()
            }

            R.id.btn6 -> {
                calcuHist()
            }

            R.id.btn7 -> {
                equalizeHist()
            }

            R.id.btn8 -> {
                matLight()
            }

        }
    }

    private fun matLight() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.matLight(bitmap, object : Runnable {
            override fun run() {
                binding.img2.setImageBitmap(bitmap)
                binding.tvLab2.text = "图片高亮"
            }


        })
    }

    private fun opencvMap() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        val src = Mat()
        src.bitmap2mat(bitmap)

        val dst = Mat(src.rows, src.cols, src.type)

        val M = Mat(2, 3, CVType.CV_32FC1) //2*3 矩阵
        //缩小0.5
        M.at(0, 0, 0.5f) //a0
        M.at(0, 1, 0f) //a1
        M.at(0, 2, 0f) //a2

        M.at(1, 0, 0f) //b0
        M.at(1, 1, 0.5f) //b1
        M.at(1, 2, 0f) //b2
        src.warpAffine(dst.jniMatAds, M.jniMatAds,bitmap)
        binding.img3.setImageBitmap(bitmap)
        binding.tvLab3.text = "图片自定义仿射变换"
    }


    private fun equalizeHist() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.equalizeHist(bitmap,object : Runnable {
            override fun run() {
                binding.img2.setImageBitmap(bitmap)
                binding.tvLab2.text = "手写实现直方图均衡";
            }

        })

    }

    private fun calcuHist() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.calcuHist(bitmap, object : CallbackInMainThread {

            override fun resultCallback(result: Any) {
                val newBitmap = result as Bitmap
                binding.img4.setImageBitmap(newBitmap)
                binding.tvLab4.text = "手写实现直方图计算";
            }

        })
    }


    private fun remap() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.remap(bitmap, object : Runnable {
            override fun run() {
                binding.img2.setImageBitmap(bitmap)
                binding.tvLab2.text = "图片重映射";
            }

        })
    }

    private fun reSize() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.reSize(bitmap, object : CallbackInMainThread {
            override fun resultCallback(result: Any) {
                val newBitmap = result as Bitmap
                binding.img3.setImageBitmap(newBitmap)
                binding.tvLab3.text = "仿上采样，降采样实现图片的放大和缩小";
            }

        })
    }

    private fun matrixTransform() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        FaceDetection.instance.matrixTransform(bitmap, object : Runnable {
            override fun run() {
                binding.img2.setImageBitmap(bitmap)
                binding.tvLab2.text = "仿射变换旋转图片";
            }

        })

    }

    private fun rotateImage() {
        val file =
            File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
        if (!file.exists()) {
            Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
            return
        }
        val bitmap =
            BitmapFactory.decodeFile(file.absolutePath)
        println("rotateImage bitmap width=${bitmap.width} height=${bitmap.height}")

        FaceDetection.instance.rotateImage(bitmap, object : CallbackInMainThread {
            override fun resultCallback(result: Any) {
                val newBitmap = result as Bitmap;
                binding.img2.setImageBitmap(newBitmap)
                binding.tvLab2.text = "旋转图片";
            }


        })
    }


}