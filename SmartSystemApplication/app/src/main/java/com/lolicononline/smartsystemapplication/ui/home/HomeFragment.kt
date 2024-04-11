package com.lolicononline.smartsystemapplication.ui.home

import android.annotation.SuppressLint
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import android.widget.Toast
import androidx.annotation.RequiresApi
import androidx.fragment.app.Fragment
import androidx.lifecycle.ViewModelProvider
import com.hivemq.client.mqtt.MqttClient
import com.hivemq.client.mqtt.MqttClientState
import com.hivemq.client.mqtt.MqttGlobalPublishFilter
import com.hivemq.client.mqtt.datatypes.MqttQos
import com.hivemq.client.mqtt.mqtt5.message.publish.Mqtt5Publish
import com.lolicononline.smartsystemapplication.R
import com.lolicononline.smartsystemapplication.databinding.FragmentHomeBinding
import org.json.JSONException
import org.json.JSONObject
import java.nio.charset.StandardCharsets

const val mqttHost = "test.mosquitto.org"
const val mqttPort = 1883
const val mqttUsername = ""
const val mqttPassword = ""

const val originStateTopic = "origin/state/topic"
const val clientStateTopic = "client/state/topic"

const val originLwtTopic = "origin/lwt/topic"

const val duration = Toast.LENGTH_SHORT

class HomeFragment : Fragment() {

    private var _binding: FragmentHomeBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    // 定义一个标志，用于指示是软件控制还是触控
    private var isSoftwareControlled = false

    private var ledIntensity = 0u
    private var alarmIntensity = 0u

    private var temperatureUpperBound = 0.0f
    private var temperatureLowerBound = 0.0f
    private var humidityUpperBound = 0.0f
    private var humidityLowerBound = 0.0f
    private var illuminationUpperBound = 0.0f
    private var illuminationLowerBound = 0.0f

    private val mqttClient =
        MqttClient.builder().useMqttVersion5().serverHost(mqttHost).serverPort(mqttPort)/*.sslWithDefaultConfig()*/
            .buildBlocking()

    @RequiresApi(Build.VERSION_CODES.O)
    @SuppressLint("SetTextI18n", "UseCompatLoadingForDrawables")
    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?, savedInstanceState: Bundle?
    ): View {

        val homeViewModel = ViewModelProvider(this)[HomeViewModel::class.java]

        _binding = FragmentHomeBinding.inflate(inflater, container, false)
        val root: View = binding.root

        val originStateTextView: TextView = binding.stateTextView

        val temperatureTextView: TextView = binding.temperatureTextView
        val humidityTextView: TextView = binding.humidityTextView
        val illuminationTextView: TextView = binding.illuminationTextView

        val temperatureRangeSeekBar = binding.temperatureRangeSeekBar
        val humidityRangeSeekBar = binding.humidityRangeSeekBar
        val illuminationRangeSeekBar = binding.illuminationRangeSeekBar

        val setFlagButton = binding.setFlagButton

        val ledToggleButton = binding.ledToggleButton
        val ledUpButton = binding.ledUpButton
        val ledDownButton = binding.ledDownButton

        val alarmToggleButton = binding.alarmToggleButton
        val alarmUpButton = binding.alarmUpButton
        val alarmDownButton = binding.alarmDownButton

        homeViewModel.text.observe(viewLifecycleOwner) {
            originStateTextView.text = it
        }

        temperatureRangeSeekBar.setIndicatorTextDecimalFormat("0.0")
        temperatureRangeSeekBar.setIndicatorTextStringFormat("%s ℃")
        humidityRangeSeekBar.setIndicatorTextDecimalFormat("0.0")
        humidityRangeSeekBar.setIndicatorTextStringFormat("%s %%")
        illuminationRangeSeekBar.setIndicatorTextDecimalFormat("0.0")
        illuminationRangeSeekBar.setIndicatorTextStringFormat("%s lx")

        if (mqttClient.state != MqttClientState.CONNECTED) {
            do {
                connectToMqttServer()
                Thread.sleep(1000)
                if (mqttClient.state == MqttClientState.CONNECTED) {
                    Toast.makeText(context, "连接已建立", duration).show()

                    subscribeToTopic(originStateTopic)
                    subscribeToTopic(originLwtTopic)

                    // set a callback that is called when a message is received (using the async API style)
                    mqttClient.toAsync().publishes(MqttGlobalPublishFilter.SUBSCRIBED) { publish: Mqtt5Publish ->
                        Log.d(
                            "MqttClient",
                            "Received message: " + publish.topic + " -> " + StandardCharsets.UTF_8.decode(publish.payload.get())
                        )

                        val payload = StandardCharsets.UTF_8.decode(publish.payload.get()).toString()

                        activity?.runOnUiThread {
                            if (payload != "Offline") {
                                originStateTextView.text = "Online"
                                originStateTextView.background = requireContext().getDrawable(R.color.colorOnline)
                            } else {
                                originStateTextView.text = payload
                                originStateTextView.background = requireContext().getDrawable(R.color.colorOffline)
                            }

                            if (isJsonValid(payload)) {
                                val jsonObject = JSONObject(payload)

                                if (jsonObject.has("Temperature")) {
                                    temperatureTextView.text = "温度: ${jsonObject.getDouble("Temperature")} ℃"
                                }

                                if (jsonObject.has("Humidity")) {
                                    humidityTextView.text = "湿度: ${jsonObject.getDouble("Humidity")} %"
                                }

                                if (jsonObject.has("Illumination")) {
                                    illuminationTextView.text = "照度: ${jsonObject.getDouble("Illumination")} lx"
                                }

                                if (jsonObject.has("LED_Intensity")) {
                                    isSoftwareControlled = true
                                    ledIntensity = jsonObject.getInt("LED_Intensity").toUInt()
                                    ledToggleButton.isChecked = ledIntensity > 0u
                                    Toast.makeText(context, "客厅灯亮度: ${ledIntensity}%", duration).show()
                                    isSoftwareControlled = false
                                }

                                if (jsonObject.has("Alarm_Intensity")) {
                                    isSoftwareControlled = true
                                    alarmIntensity = jsonObject.getInt("Alarm_Intensity").toUInt()
                                    alarmToggleButton.isChecked = alarmIntensity > 0u
                                    Toast.makeText(context, "报警器强度: ${alarmIntensity}%", duration).show()
                                    isSoftwareControlled = false
                                }

                                if (jsonObject.has("TemperatureH") && jsonObject.has("TemperatureL")) {
                                    temperatureUpperBound = jsonObject.getDouble("TemperatureH").toFloat()
                                    temperatureLowerBound = jsonObject.getDouble("TemperatureL").toFloat()
                                    temperatureRangeSeekBar.setProgress(
                                        temperatureLowerBound, temperatureUpperBound
                                    )
                                }

                                if (jsonObject.has("HumidityH") && jsonObject.has("HumidityL")) {
                                    humidityUpperBound = jsonObject.getDouble("HumidityH").toFloat()
                                    humidityLowerBound = jsonObject.getDouble("HumidityL").toFloat()
                                    humidityRangeSeekBar.setProgress(humidityLowerBound, humidityUpperBound)
                                }

                                if (jsonObject.has("IlluminationH") && jsonObject.has("IlluminationL")) {
                                    illuminationUpperBound = jsonObject.getDouble("IlluminationH").toFloat()
                                    illuminationLowerBound = jsonObject.getDouble("IlluminationL").toFloat()
                                    illuminationRangeSeekBar.setProgress(
                                        illuminationLowerBound, illuminationUpperBound
                                    )
                                }
                            }
                        }
                    }
                } else {
                    Toast.makeText(context, "正在建立连接", duration).show()
                }
            } while (mqttClient.state != MqttClientState.CONNECTED)
        }

        publishToTopic("{\"ClientState\":\"READY\"}")

        Toast.makeText(context, "READY", duration).show()

        setFlagButton.setOnClickListener {
            if (originStateTextView.text == "Online") {
                temperatureUpperBound = String.format("%.1f", temperatureRangeSeekBar.rightSeekBar.progress).toFloat()
                temperatureLowerBound = String.format("%.1f", temperatureRangeSeekBar.leftSeekBar.progress).toFloat()
                humidityUpperBound = String.format("%.1f", humidityRangeSeekBar.rightSeekBar.progress).toFloat()
                humidityLowerBound = String.format("%.1f", humidityRangeSeekBar.leftSeekBar.progress).toFloat()
                illuminationUpperBound = String.format("%.1f", illuminationRangeSeekBar.rightSeekBar.progress).toFloat()
                illuminationLowerBound = String.format("%.1f", illuminationRangeSeekBar.leftSeekBar.progress).toFloat()

                publishToTopic("{\"TemperatureH\":$temperatureUpperBound,\"TemperatureL\":$temperatureLowerBound,\"HumidityH\":$humidityUpperBound,\"HumidityL\":$humidityLowerBound,\"IlluminationH\":$illuminationUpperBound,\"IlluminationL\":$illuminationLowerBound}")
                Toast.makeText(context, "安全位已设置", duration).show()
            } else {
                Toast.makeText(context, "设备未上线", duration).show()
            }
        }

        ledToggleButton.setOnCheckedChangeListener { _, isChecked ->
            if (!isSoftwareControlled) {
                if (originStateTextView.text == "Online") {
                    if (isChecked) {
                        // The toggle is enabled
                        publishToTopic("{\"LED_Intensity\":${99u}}")
                    } else {
                        // The toggle is disabled
                        publishToTopic("{\"LED_Intensity\":${0u}}")
                    }
                } else {
                    ledToggleButton.isChecked = !isChecked
                    Toast.makeText(context, "设备未上线", duration).show()
                }
            }
        }

        ledUpButton.setOnClickListener {
            if (originStateTextView.text == "Online") {
                if (ledIntensity < 99u) {
                    publishToTopic("{\"LED_Intensity\":${ledIntensity + 10u}}")
                } else {
                    Toast.makeText(context, "已达到最大亮度", duration).show()
                }
            } else {
                Toast.makeText(context, "设备未上线", duration).show()
            }
        }

        ledDownButton.setOnClickListener {
            if (originStateTextView.text == "Online") {
                if (ledIntensity > 0u) {
                    publishToTopic("{\"LED_Intensity\":${ledIntensity - 10u}}")
                } else {
                    Toast.makeText(context, "已达到最小亮度", duration).show()
                }
            } else {
                Toast.makeText(context, "设备未上线", duration).show()
            }
        }

        alarmToggleButton.setOnCheckedChangeListener { _, isChecked ->
            if (!isSoftwareControlled) {
                if (originStateTextView.text == "Online") {
                    if (isChecked) {
                        // The toggle is enabled
                        publishToTopic("{\"Alarm_Intensity\":${99u}}")
                    } else {
                        // The toggle is disabled
                        publishToTopic("{\"Alarm_Intensity\":${0u}}")
                    }
                } else {
                    alarmToggleButton.isChecked = !isChecked
                    Toast.makeText(context, "设备未上线", duration).show()
                }
            }
        }

        alarmUpButton.setOnClickListener {
            if (originStateTextView.text == "Online") {
                if (alarmIntensity < 99u) {
                    publishToTopic("{\"Alarm_Intensity\":${alarmIntensity + 10u}}")
                } else {
                    Toast.makeText(context, "已达到最大强度", duration).show()
                }
            } else {
                Toast.makeText(context, "设备未上线", duration).show()
            }
        }

        alarmDownButton.setOnClickListener {
            if (originStateTextView.text == "Online") {
                if (alarmIntensity > 0u) {
                    publishToTopic("{\"Alarm_Intensity\":${alarmIntensity - 10u}}")
                } else {
                    Toast.makeText(context, "已达到最小强度", duration).show()
                }
            } else {
                Toast.makeText(context, "设备未上线", duration).show()
            }
        }

        return root
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    private fun isJsonValid(test: String): Boolean {
        return try {
            JSONObject(test)
            true
        } catch (ex: JSONException) {
            false
        }
    }

    private fun connectToMqttServer(username: String = mqttUsername, password: String = mqttPassword) {
        mqttClient.toAsync().connectWith().simpleAuth().username(username)
            .password(StandardCharsets.UTF_8.encode(password)).applySimpleAuth().send()
    }

    private fun subscribeToTopic(topic: String) {
        if (mqttClient.state == MqttClientState.CONNECTED) {
            mqttClient.toAsync().subscribeWith().topicFilter(topic).qos(MqttQos.EXACTLY_ONCE).send()
        } else {
            connectToMqttServer()
        }
    }

    private fun publishToTopic(payload: String) {
        if (mqttClient.state == MqttClientState.CONNECTED) {
            mqttClient.toAsync().publishWith().topic(clientStateTopic).payload(StandardCharsets.UTF_8.encode(payload))
                .qos(MqttQos.EXACTLY_ONCE).send()
        } else {
            connectToMqttServer()
        }
    }
}
