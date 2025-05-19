package com.yaxiu.opencv.ui.principle

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.fragment.app.Fragment
import com.yaxiu.opencv.CallbackInMainThread
import com.yaxiu.opencv.FaceDetection
import com.yaxiu.opencv.Mat
import com.yaxiu.opencv.Opencv
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

        }else{
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
                calculHist()
            }

            R.id.btn7 -> {
                normalize()
            }

            R.id.btn8 -> {
                equalizeHist()
            }


        }
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
        val des = Mat(src.rows, src.cols, Opencv.CV_8UC4)
        //高亮清晰度处理

    }


    private fun equalizeHist() {


    }

    private fun normalize() {

    }

    private fun calculHist() {

    }


    private fun remap() {

    }

    private fun reSize() {

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
                binding.img1.setImageBitmap(bitmap)
                binding.tvLab1.text = "仿射变换旋转图片";
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
                val newBitmap=result as Bitmap;
                binding.img2.setImageBitmap(newBitmap)
                binding.tvLab2.text = "旋转图片";
            }


        })
    }


}