package com.yaxiu.opencv.ui.native

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.fragment.app.Fragment
import androidx.lifecycle.ViewModelProvider
import com.yaxiu.opencv.FaceDetection
import com.yaxiu.opencv.databinding.FragmentSlideshowBinding

class NativeFragment : Fragment() {

    private var _binding: FragmentSlideshowBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        val slideshowViewModel =
            ViewModelProvider(this).get(SlideshowViewModel::class.java)

        _binding = FragmentSlideshowBinding.inflate(inflater, container, false)
        val root: View = binding.root

        return root
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    fun createMat(view: View) {
        FaceDetection.instance.matObj()

    }

    fun opencvEditPiexl(view: View) {

    }
    fun opencvAddWeight(view: View) {

    }
    fun opencvSaturationBrightnessContrast(view: View) {

    }

}