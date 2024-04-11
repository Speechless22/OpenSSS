package com.lolicononline.smartsystemapplication.ui.config

import android.Manifest
import android.app.Activity
import android.content.Context
import android.content.pm.PackageManager
import android.net.wifi.WifiInfo
import android.net.wifi.WifiManager
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.fragment.app.Fragment
import androidx.lifecycle.ViewModelProvider
import com.espressif.iot.esptouch.EsptouchTask
import com.lolicononline.smartsystemapplication.databinding.FragmentConfigBinding
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch

const val LOCATION_PERMISSION_REQUEST_CODE = 1

class ConfigFragment : Fragment() {
    private var _binding: FragmentConfigBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    private var task: EsptouchTask? = null

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?
    ): View {
        val configViewModel = ViewModelProvider(this)[ConfigViewModel::class.java]

        _binding = FragmentConfigBinding.inflate(inflater, container, false)
        val root: View = binding.root

//        val textView: TextView = binding.textConfig
//        configViewModel.text.observe(viewLifecycleOwner) {
//            textView.text = it
//        }

        val wifiManager: WifiManager? = context?.getSystemService(Context.WIFI_SERVICE) as? WifiManager
        val wifiState: Int = wifiManager?.wifiState ?: WifiManager.WIFI_STATE_UNKNOWN
        val wifiInfo: WifiInfo? = wifiManager?.connectionInfo

        val apSsidTextView = binding.apSsidText
        val apBssidTextView = binding.apBssidText

        val apPasswordTextView = binding.apPasswordEdit
        val deviceCountEditText = binding.deviceCountEdit
        val packageBroadcastRadioButton = binding.packageBroadcast
        val packageMulticastRadioButton = binding.packageMulticast
        val confirmButton = binding.confirmBtn

        val cancelButton = binding.cancelButton

        if (wifiState == WifiManager.WIFI_STATE_ENABLED && wifiInfo != null) {
            if (ContextCompat.checkSelfPermission(
                    requireContext(), Manifest.permission.ACCESS_FINE_LOCATION
                ) == PackageManager.PERMISSION_GRANTED
            ) {
                // 已经有权限
                apSsidTextView.text = wifiInfo.ssid.replace("\"", "")
                apBssidTextView.text = wifiInfo.bssid
            } else {
                // 请求权限
                ActivityCompat.requestPermissions(
                    requireContext() as Activity,
                    arrayOf(Manifest.permission.ACCESS_FINE_LOCATION),
                    LOCATION_PERMISSION_REQUEST_CODE
                )
            }
        } else {
            Toast.makeText(context, "WIFI未连接", Toast.LENGTH_LONG).show()
        }

        confirmButton.setOnClickListener {
            task = EsptouchTask(
                apSsidTextView.text.toString(),
                apBssidTextView.text.toString(),
                apPasswordTextView.text.toString(),
                context
            )
            task!!.setPackageBroadcast(packageBroadcastRadioButton.isChecked)

            binding.progressView.visibility = View.VISIBLE

            Toast.makeText(context, "开始配网", Toast.LENGTH_LONG).show()

            CoroutineScope(Dispatchers.IO).launch {
                val expectResultCount = 1
                val results = task?.executeForResults(expectResultCount)
                val first = results?.get(0)

                if (first != null) {
                    if (first.isCancelled) {
                        // User cancel the task
                        activity?.runOnUiThread {
                            Toast.makeText(context, "取消配网", Toast.LENGTH_LONG).show()
                        }
                    }
                    if (first.isSuc) {
                        // EspTouch successfully
                        activity?.runOnUiThread {
                            binding.progressView.visibility = View.GONE

                            Toast.makeText(context, "配网成功", Toast.LENGTH_LONG).show()
                        }
                    }
                }
            }
        }

        cancelButton.setOnClickListener {
            task?.interrupt()

            binding.progressView.visibility = View.GONE
        }

//        task?.setEsptouchListener {
//            // Result callback
//        }

        return root
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}
