#include "esp8266.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "buzzer.h"

volatile uint16_t espTimeoutCounter = ESP_TIMEOUT_COUNT;

volatile FunctionalState wifiInitState = DISABLE;

volatile ErrorStatus atResponse = ERROR;

volatile FunctionalState wifiState = DISABLE;
volatile FunctionalState mqttState = DISABLE;

extern char txBuffer[USART_MAX_BUFFER_SIZE];

extern uint8_t alarmIntensity;

/*
获取MQTT客户端ID
 */
static char* getMQTTClientID(void)
{
	static char mqttClientID[32]; // 32位十六进制数最大长度为8，所以3个数的总长度为24，再加上字符串结束符'\0'，总长度为25
	uint32_t unique_id[3];		  // MCU的唯一ID

	unique_id[0] = *(uint32_t *)(UID_BASE);
	unique_id[1] = *(uint32_t *)(UID_BASE + 4);
	unique_id[2] = *(uint32_t *)(UID_BASE + 8);

	snprintf(mqttClientID, sizeof(mqttClientID), "%lx%lx%lx", unique_id[0], unique_id[1], unique_id[2]);

	return mqttClientID;
}

/*
配置MQTT用户信息
 */
static void configMQTTUSER(void)
{
	snprintf(txBuffer, sizeof(txBuffer), "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"%s\"\r\n", getMQTTClientID(), MQTT_USERNAME, MQTT_PASSWORD, MQTT_PATH);
	AT_SendCommand(txBuffer);
}

/*
初始化MQTT
 */
static void mqttInit(void)
{
	mqttState = DISABLE;

	if (wifiState)
	{
		configMQTTUSER();			  // 设置MQTT用户信息
		AT_SendCommand(MQTT_CONNCFG); // 设置MQTT连接信息
		AT_SendCommand(MQTT_CONNECT); // 连接MQTT服务器

		while (!mqttState && espTimeoutCounter > MQTT_CONNECT_TIMEOUT_COUNT) // 等待MQTT服务连接成功，超时时间16s左右
		{
			LED_Red_Control(TIM_GetCapture2(TIM3) > 0 ? 0 : 99);
			Delayms(100);
			espTimeoutCounter--;
		}

		LED_Red_Control(alarmIntensity);
	}
}

/*
初始化WIFI
 */
static void wifiInit(void)
{
	wifiState = DISABLE;

	if (!wifiInitState)
	{
		AT_SendCommand("AT+RESTORE\r\n"); // 恢复出厂设置

		Delayms(3000);

		AT_SendCommand("ATE0\r\n");				   // 关闭回显
		AT_SendCommand("AT+CIPV6=1\r\n");		   // 开启IPv6
		AT_SendCommand("AT+CWMODE=1\r\n");		   // 设置为station模式
		AT_SendCommand("AT+CWRECONNCFG=10,0\r\n"); // 重连间隔10s

		AT_SendCommand("AT+CWSTARTSMART=1\r\n"); // 启动smartconfig

		while (!wifiState && espTimeoutCounter > ESPTOUCH_TIMEOUT_COUNT) // 等待配网成功，超时时间60s左右
		{
			LED_Red_Control(TIM_GetCapture2(TIM3) > 0 ? 0 : 99);
			Delayms(500);
			espTimeoutCounter--;
		}

		LED_Red_Control(alarmIntensity);

		while (espTimeoutCounter > ESPTOUCH_TIMEOUT_COUNT) // 等待SmartConfig向客户端返回结果，超时时间12s以下
		{
			Delayms(100);
			espTimeoutCounter--;
		}

		AT_SendCommand("AT+CWSTOPSMART\r\n"); // 停止smartconfig
	}
}

/*
初始化ESP8266
 */
void ESP8266_Init(void)
{
	wifiInit();

	AT_SendCommand("AT+RST\r\n"); // 重启模块

	while (!wifiState && espTimeoutCounter > WIFI_CONNECT_TIMEOUT_COUNT) // 等待wifi连接，超时时间30s左右
	{
		BUZZER_Control(TIM_GetCapture1(TIM3) > 0 ? 0 : 99);
		Delayms(100);
		espTimeoutCounter--;
	}

	BUZZER_Control(alarmIntensity);
	AT_SendCommand("ATE0\r\n"); // 关闭回显

	mqttInit();
}

/*
订阅主题
参数：
	topic：主题
 */
void MQTT_Subscribe(const char *topic)
{
	if (mqttState)
	{
		snprintf(txBuffer, sizeof(txBuffer), "AT+MQTTSUB=0,\"%s\",2\r\n", topic); // QoS2
		AT_SendCommand(txBuffer);
	}
}

/*
发布消息
参数：
	topic：主题
	format：格式化字符串
	...：可变参数
 */
void MQTT_Publish(const char *topic, const char *format, ...)
{
	if (mqttState)
	{
		Delayms(100);

		va_list args;
		va_start(args, format);

		int length = vsnprintf(NULL, 0, format, args); // 使用 vsnprintf 计算格式化后的字符串长度
		va_end(args);

		char *message = (char *)malloc(length + 1); // 分配足够的内存来存储格式化后的字符串

		va_start(args, format);
		vsnprintf(message, length + 1, format, args); // 再次使用 vsnprintf 将带有变量的 message 格式化到新分配的内存中
		va_end(args);

		snprintf(txBuffer, sizeof(txBuffer), "AT+MQTTPUB=0,\"%s\",\"%s\",2,0\r\n", topic, message); // 构建命令字符串，QoS2

		USART_SendString(ESP8266_USART, txBuffer); // 发送命令

		free(message); // 释放分配的内存
	}
}

/*
发送AT指令
参数：
	command：AT指令
 */
void AT_SendCommand(const char *command)
{
	uint8_t atTimeoutCounter = 20;
	atResponse = ERROR;

	USART_SendString(ESP8266_USART, command);

	while (atResponse && atTimeoutCounter > 0) // 超时时间2s左右
	{
		Delayms(100);
		atTimeoutCounter--;
	}
}
