package com.lolicononline.smartsystemapplication

import com.hivemq.client.mqtt.MqttClient
import com.hivemq.client.mqtt.MqttClient.DEFAULT_SERVER_PORT
import com.hivemq.client.mqtt.MqttClientState
import com.hivemq.client.mqtt.MqttGlobalPublishFilter
import com.hivemq.client.mqtt.datatypes.MqttQos
import com.hivemq.client.mqtt.mqtt5.Mqtt5BlockingClient
import com.hivemq.client.mqtt.mqtt5.message.publish.Mqtt5Publish
import kotlinx.coroutines.*
import java.nio.charset.StandardCharsets

class MyMQTT {
    private val mqttHost = "test.mosquitto.org"
    private val mqttPort = DEFAULT_SERVER_PORT
    private val mqttUsername = ""   // MQTT username
    private val mqttPassword = ""   // MQTT password

    private val originStateTopic = "origin/state/topic" // MQTT topic
    val clientStateTopic = "client/state/topic" // MQTT topic
    private val originLwtTopic = "origin/lwt/topic" // MQTT topic

    private val mqttClient = initialize()

    init {
        CoroutineScope(Dispatchers.IO).launch {
            connect(mqttClient)
            subscribe(topic = originStateTopic)
            subscribe(topic = originLwtTopic)
        }
    }

    private fun initialize(host: String = mqttHost, port: Int = mqttPort): Mqtt5BlockingClient {
        return MqttClient.builder().useMqttVersion5().serverHost(host).serverPort(port)/*.sslWithDefaultConfig()*/
            .buildBlocking()
    }

    private suspend fun connect(
        client: Mqtt5BlockingClient = mqttClient, username: String = mqttUsername, password: String = mqttPassword
    ) {
        do {
            try {
                withContext(Dispatchers.IO) {
                    client.toAsync().connectWith().simpleAuth().username(username)
                        .password(StandardCharsets.UTF_8.encode(password)).applySimpleAuth().send()
                }
                if (client.state != MqttClientState.CONNECTED) {
                    // 如果连接未成功，等待一段时间再尝试
                    delay(5000) // 等待5秒
                }
            } catch (e: Exception) {
                // 如果连接尝试失败，等待一段时间再尝试
                delay(5000) // 等待5秒
            }
        } while (client.state != MqttClientState.CONNECTED)
    }

    private suspend fun subscribe(client: Mqtt5BlockingClient = mqttClient, topic: String) {
        withContext(Dispatchers.IO) {
            client.toAsync().subscribeWith().topicFilter(topic).qos(MqttQos.EXACTLY_ONCE).send()
        }
    }

    fun createReceiveCallback(client: Mqtt5BlockingClient = mqttClient) {
        client.toAsync().publishes(MqttGlobalPublishFilter.SUBSCRIBED) { publish: Mqtt5Publish ->
            val topic = publish.topic.toString()
            val payload = StandardCharsets.UTF_8.decode(publish.payload.get()).toString()

            receiveCallback(topic, payload)
        }
    }

    suspend fun publish(client: Mqtt5BlockingClient = mqttClient, topic: String, payload: String) {
        withContext(Dispatchers.IO) {
            client.toAsync().publishWith().topic(topic).payload(StandardCharsets.UTF_8.encode(payload))
                .qos(MqttQos.EXACTLY_ONCE).send()
        }
    }

    private fun receiveCallback(topic: String, payload: String) {
        when (topic) {
            originStateTopic -> {
                // Do something with the payload
            }

            originLwtTopic -> {
                // Do something with the payload
            }
        }
    }
}
