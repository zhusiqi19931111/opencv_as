package com.yaxiu.opencv.ui.face

import android.Manifest
import android.content.ContentValues
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.os.Environment
import android.provider.MediaStore
import android.provider.Settings
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.fragment.app.Fragment
import com.yaxiu.opencv.FaceDetection
import com.yaxiu.opencv.R
import com.yaxiu.opencv.databinding.FragmentFaceBinding
import kotlinx.coroutines.delay
import java.io.File
import java.io.FileOutputStream
import java.io.IOException
import java.io.InputStream
import java.io.OutputStream
import java.util.concurrent.Executors

class FaceRecognitionFragment : Fragment(), View.OnClickListener {

    private lateinit var mCascadeFile: File
    private lateinit var faceBitmap: Bitmap
    private val REQUEST_CODE = 200
    private val TAG = "FaceRecognitionFragment"

    private var _binding: FragmentFaceBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    val threadPool = Executors.newSingleThreadExecutor()

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {


        _binding = FragmentFaceBinding.inflate(inflater, container, false)
        val root: View = binding.root


        faceBitmap = BitmapFactory.decodeResource(resources, R.drawable.face);
        binding.faceImage.setImageBitmap(faceBitmap)
        binding.btnRecognition.setOnClickListener(this)
        binding.btnCheckPermission.setOnClickListener(this)
        return root
    }

    /**
     * api 34 访问应用外部目录
     */
    private fun getFilePath() {

        // 访问 Downloads 目录
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            val resolver = requireContext().contentResolver
            val contentValues = ContentValues().apply {
                put(MediaStore.Downloads.DISPLAY_NAME, "lbpcascade_frontalface.xml")
                put(MediaStore.Downloads.MIME_TYPE, "application/octet-stream")
            }
            val uri = resolver.insert(MediaStore.Downloads.EXTERNAL_CONTENT_URI, contentValues)
            uri?.let {
                resolver.openOutputStream(it).use { outputStream ->
                    try {
                        // load cascade file from application resources
                        val inputStream = resources.openRawResource(R.raw.lbpcascade_frontalface)
                        val buffer = ByteArray(4096)
                        var bytesRead: Int
                        while ((inputStream.read(buffer).also { bytesRead = it }) != -1) {
                            outputStream?.write(buffer, 0, bytesRead)
                        }
                        inputStream.close()
                        outputStream?.close()
                        println("$TAG uri.path:${uri.path} uri：$uri")
                    } catch (e: IOException) {
                        e.printStackTrace()
                    }

                }
            }
        }
    }

    private fun copyCascadeFile() {
        /**
         *
         * 从Android 10开始，由于Scoped Storage限制，直接访问外部存储路径变得更加受限
         *
         * 对于MediaStore中的文件，建议使用ContentResolver直接操作而不是获取路径
         *
         * 对于应用私有文件，使用getFilesDir()或getCacheDir()获取内部存储路径
         *
         * 对于需要共享的文件，考虑使用FileProvider
         */
        // 获取应用专属外部存储目录
        val externalFilesDir = requireContext().getExternalFilesDir(null)
        val externalCacheDir = requireContext().externalCacheDir
        // 获取特定类型的子目录（如 Pictures）
        val download = requireContext().getExternalFilesDir(Environment.DIRECTORY_DOWNLOADS)

        mCascadeFile = File(download, "lbpcascade_frontalface.xml")
        println("$TAG mCascadeFile.exists:${mCascadeFile.exists()} mCascadeFile.absolutePath:${mCascadeFile.absolutePath}")
        if (mCascadeFile.exists()) {
            opencvUpdateMat()
            runSubThread {

                FaceDetection.instance.loadCascade(mCascadeFile.absolutePath)
            }
            return
        }
        var inputStream: InputStream? = null
        var outputStream: OutputStream? = null
        println("$TAG externalFilesDir:${externalFilesDir} externalCacheDir:${externalCacheDir} \n download:$download  getFilesDir()：${requireContext().getFilesDir()}  getCacheDir()：${requireContext().cacheDir}")
        try {
            outputStream = FileOutputStream(mCascadeFile)
            // load cascade file from application resources
            inputStream = resources.openRawResource(R.raw.lbpcascade_frontalface)
            val buffer = ByteArray(4096)
            var bytesRead: Int
            while ((inputStream.read(buffer).also { bytesRead = it }) != -1) {
                outputStream.write(buffer, 0, bytesRead)
            }
            runSubThread {
                FaceDetection.instance.loadCascade(mCascadeFile.absolutePath)
            }
            opencvUpdateMat()

        } catch (e: IOException) {
            e.printStackTrace()
        } finally {
            inputStream?.close()
            outputStream?.close()
        }

    }

    private fun opencvUpdateMat() {
        runSubThread {
            FaceDetection.instance.opencvUpdateMat(
                "/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/20250425172853.jpg",
                "/storage/emulated/0/Android/data/com.yaxiu.opencv/files/Download/out_20250425172853.jpg"
            )
        }
    }


    // 检查权限
    fun checkStoragePermission(): Boolean {
        return if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            Environment.isExternalStorageManager()
        } else {
            ContextCompat.checkSelfPermission(
                requireContext(),
                Manifest.permission.WRITE_EXTERNAL_STORAGE
            ) == PackageManager.PERMISSION_GRANTED
        }
    }

    // 请求权限
    fun requestStoragePermission() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            try {
                val intent = Intent(Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION)
                intent.addCategory("android.intent.category.DEFAULT")
                intent.data = Uri.parse("package:${requireContext().packageName}")
                startActivityForResult(intent, REQUEST_CODE)
            } catch (e: Exception) {
                val intent = Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION)
                startActivityForResult(intent, REQUEST_CODE)
            }
        } else {
            ActivityCompat.requestPermissions(
                requireActivity(),
                arrayOf(
                    Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.READ_EXTERNAL_STORAGE
                ),
                REQUEST_CODE
            )
        }
    }

    override fun onClick(v: View) {
        // 识别人脸，保存人脸特征信息
        when (v.id) {
            R.id.btn_recognition -> {
                runSubThread {
                    FaceDetection.instance.faceDetectionSaveInfo(faceBitmap)
                    Thread.sleep(3_000)//临时用途，建议采用回调
                    requireActivity().runOnUiThread({
                        binding.faceImage.setImageBitmap(faceBitmap)
                    })

                }

            }

            R.id.btn_check_permission -> {

                if (checkStoragePermission()) {
                    requestStoragePermission()
                } else {
                    copyCascadeFile()

                }
            }

        }

    }


    @Deprecated("Deprecated in Java")
    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        when (requestCode) {
            REQUEST_CODE -> {
                if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // 权限已授予
                    println("$TAG 获取权限成功")
                    copyCascadeFile()
                } else {
                    // 权限被拒绝
                    println("$TAG 权限被拒绝")
                }
            }
        }
    }

    fun runSubThread(callback: () -> Unit) {
        threadPool.execute {
            callback.invoke()
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }


}