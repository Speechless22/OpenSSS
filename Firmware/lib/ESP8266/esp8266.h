#ifndef ESP8266_H
#define ESP8266_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define ESP8266_USART USART1 // ESP8266串口

#define ESP_TIMEOUT_COUNT 0xFFF                                     // ESP初始化超时计数
#define ESPTOUCH_TIMEOUT_COUNT ESP_TIMEOUT_COUNT - 120              // ESPTOUCH超时计数
#define WIFI_CONNECT_TIMEOUT_COUNT ESPTOUCH_TIMEOUT_COUNT - 300     // WIFI连接超时计数
#define MQTT_CONNECT_TIMEOUT_COUNT WIFI_CONNECT_TIMEOUT_COUNT - 160 // MQTT连接超时计数

#define MQTT_HOST "test.mosquitto.org" //	MQTT服务器域名
#define MQTT_PORT "1883"               //	MQTT服务器端口
#define MQTT_USERNAME ""               //	MQTT用户名
#define MQTT_PASSWORD ""               //	MQTT密码
#define MQTT_PATH ""                   //	MQTT路径

#define ORIGIN_LWT_TOPIC "origin/lwt/topic" //	源端遗嘱主题
#define LWT_MESSAGE "Offline"               //	遗嘱消息

#define MQTT_CONNCFG "AT+MQTTCONNCFG=0,0,0,\"" ORIGIN_LWT_TOPIC "\",\"" LWT_MESSAGE "\",2,0\r\n" //	MQTT连接配置，QoS2
#define MQTT_CONNECT "AT+MQTTCONN=0,\"" MQTT_HOST "\"," MQTT_PORT ",1\r\n"                       //	MQTT连接，自动重连

#define ORIGIN_STATE_TOPIC "origin/state/topic" //	源端状态主题
#define CLIENT_STATE_TOPIC "client/state/topic" //	客户端状态主题

    void ESP8266_Init(void);
    void MQTT_Subscribe(const char *topic);
    void MQTT_Publish(const char *topic, const char *format, ...);
    void AT_SendCommand(const char *command);

#ifdef __cplusplus
}
#endif

#endif
