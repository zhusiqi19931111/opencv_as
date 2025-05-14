package com.yaxiu.opencv.ui.qqeffects

import android.graphics.BitmapFactory
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import com.yaxiu.opencv.FaceDetection
import com.yaxiu.opencv.R
import com.yaxiu.opencv.databinding.FragmentQQEffectsBinding

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

        return root
    }

    override fun onClick(v: View) {
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

        }
    }

    private fun magnifierSpecialEffects() {
        val bitmap =
            BitmapFactory.decodeFile("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg")
        FaceDetection.instance.magnifierSpecialEffects(bitmap)
        binding.img2.setImageBitmap(bitmap)
    }

    private fun fishEyeSpecialEffects() {
        val bitmap =
            BitmapFactory.decodeFile("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg")
        FaceDetection.instance.fishEyeSpecialEffects(bitmap)
        binding.img3.setImageBitmap(bitmap)
    }

    private fun oilPaintingSpecialEffects() {
        val bitmap =
            BitmapFactory.decodeFile("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg")
        FaceDetection.instance.oilPaintingSpecialEffects(bitmap)
        binding.img4.setImageBitmap(bitmap)
    }

    private fun glassSpecialEffects() {
        val bitmap =
            BitmapFactory.decodeFile("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg")
        FaceDetection.instance.glassSpecialEffects(bitmap)
        binding.img2.setImageBitmap(bitmap)
    }

    private fun inverseWorldSpecialEffects() {
        val bitmap =
            BitmapFactory.decodeFile("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg")
        FaceDetection.instance.inverseWorldSpecialEffects(bitmap)
        binding.img3.setImageBitmap(bitmap)
    }

    private fun mirrorSpecialEffects() {
        val bitmap =
            BitmapFactory.decodeFile("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg")
        FaceDetection.instance.mirrorSpecialEffects(bitmap)
        binding.img4.setImageBitmap(bitmap)
    }

    private fun mosaicSpecialEffects() {
        val bitmap =
            BitmapFactory.decodeFile("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg")
        FaceDetection.instance.mosaicSpecialEffects(bitmap)
        binding.img2.setImageBitmap(bitmap)
    }

    private fun reliefSpecialEffects() {
        val bitmap =
            BitmapFactory.decodeFile("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg")
        FaceDetection.instance.reliefSpecialEffects(bitmap)
        binding.img3.setImageBitmap(bitmap)
    }
}