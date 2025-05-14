package com.yaxiu.opencv.ui.native

import android.app.ActivityManager
import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.os.Looper
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import android.widget.Toast
import androidx.fragment.app.Fragment
import androidx.lifecycle.ViewModelProvider
import com.yaxiu.opencv.FaceDetection
import com.yaxiu.opencv.R
import com.yaxiu.opencv.databinding.FragmentSlideshowBinding
import java.io.File
import kotlin.random.Random

class NativeFragment : Fragment(), View.OnClickListener {

    private lateinit var img2: ImageView
    private lateinit var img1: ImageView
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
        img1 = binding.img1
        img2 = binding.img2
        binding.btn1.setOnClickListener(this)
        binding.btn2.setOnClickListener(this)
        binding.btn3.setOnClickListener(this)
        binding.btn4.setOnClickListener(this)

        return root
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    private fun createMat(view: View) {
        FaceDetection.instance.matObj()
        try {
            val file=
                File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
            if(!file.exists()){
                Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
                return
            }
            val b =
                FaceDetection.instance.createBitmapByDecodeFile(file.absolutePath)
            println("createBitmapByDecodeFile b= [${b}]")
            img1.setImageBitmap(b)
        } catch (e: Exception) {
            e.printStackTrace();
        }
    }

    private fun opencvEditPiexl(view: View) {
        try {
            val file=
                File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
            if(!file.exists()){
                Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
                return
            }
            val loadPreMemory = getMemoryInfo(requireContext())
            val loadPreJavaMemory = getJavaHeapMemory()
            val random =Random.nextInt(10)
            println("random = [${random}]")
            val bitmap: Bitmap
            if (random % 2 == 0) {
                bitmap =
                    FaceDetection.instance.opencvEditPiexl(file.absolutePath)
                img2.setImageBitmap(bitmap)
            } else {
                //建议使用
                bitmap =
                    BitmapFactory.decodeFile(file.absolutePath)
                FaceDetection.instance.loadBitmapEditPiexl(bitmap,object :Runnable{
                    override fun run() {
                        img2.setImageBitmap(bitmap)
                    }

                })
            }


            val loadLastMemory = getMemoryInfo(requireContext())
            val loadLastJavaMemory = getJavaHeapMemory()
            println("opencvSaturationBrightnessContrast loadPreMemory = [${loadPreMemory}] loadLastMemory = [${loadLastMemory}] loadPreJavaMemory = [${loadPreJavaMemory}] loadLastJavaMemory = [${loadLastJavaMemory}]")
        } catch (e: Exception) {
            e.printStackTrace();
        }
    }



    private fun opencvAddWeight(view: View) {
        try {

            val file=
                File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
            val file1=
                File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250508104604.jpg");
            if(!file.exists()||!file.exists()){
                Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
                return
            }

            val bitmap = FaceDetection.instance.opencvAddWeight(
                file.absolutePath,
                file1.absolutePath,
            )
            img2.setImageBitmap(bitmap);
        } catch (e: Exception) {
            e.printStackTrace();
        }
    }

    private fun opencvSaturationBrightnessContrast(view: View) {
        try {

            val file=
                File("/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250512112613.jpg");
            if(!file.exists()){
                Toast.makeText(requireContext(), "图片资源丢失哦", Toast.LENGTH_SHORT).show()
                return
            }
            val loadPreMemory = getMemoryInfo(requireContext())
            val bitmap =
                FaceDetection.instance.opencvSaturationBrightnessContrast(file.absolutePath)
            img2.setImageBitmap(bitmap);
            val loadLastMemory = getMemoryInfo(requireContext())
            println("opencvSaturationBrightnessContrast loadPreMemory = [${loadPreMemory}] loadLastMemory = [${loadLastMemory}]")
        } catch (e: Exception) {
            e.printStackTrace();
        }
    }

    /*
     * 获取当前应用的内存使用情况
     */
    fun getMemoryInfo(context: Context): String {
        val activityManager = context.getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager
        val memoryInfo = ActivityManager.MemoryInfo()
        activityManager.getMemoryInfo(memoryInfo)

        val runtime = Runtime.getRuntime()
        val usedMemInMB = (runtime.totalMemory() - runtime.freeMemory()) / (1024 * 1024)
        val maxHeapSizeInMB = runtime.maxMemory() / (1024 * 1024)
        val availHeapSizeInMB = memoryInfo.availMem / (1024 * 1024)
        val totalMemInMB = memoryInfo.totalMem / (1024 * 1024)
        val isLowMemory = memoryInfo.lowMemory

        return """
        应用内存使用情况:
        已使用内存: $usedMemInMB MB
        最大堆内存: $maxHeapSizeInMB MB
        可用内存: $availHeapSizeInMB MB
        系统总内存: $totalMemInMB MB
        是否低内存状态: $isLowMemory""".trimIndent()
    }

    /**
     *  查看 Java 堆内存
     */
    fun getJavaHeapMemory(): String {
        val runtime = Runtime.getRuntime()
        val maxMemory = runtime.maxMemory() / (1024 * 1024)
        val allocatedMemory = runtime.totalMemory() / (1024 * 1024)
        val freeMemory = runtime.freeMemory() / (1024 * 1024)

        return """
        Java堆内存信息:
        最大可用: $maxMemory MB
        已分配: $allocatedMemory MB
        空闲: $freeMemory MB
        实际使用: ${allocatedMemory - freeMemory} MB
    """.trimIndent()
    }

    override fun onClick(v: View) {
        when (v.id) {
            R.id.btn1 -> {
                createMat(v)
            }

            R.id.btn2 -> {
                opencvEditPiexl(v)
            }

            R.id.btn3 -> {
                opencvAddWeight(v)
            }

            R.id.btn4 -> {
                opencvSaturationBrightnessContrast(v)
            }

        }
    }


}