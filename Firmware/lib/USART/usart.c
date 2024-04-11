#include "usart.h"
#include "delay.h"
#include "led.h"
#include "alarm.h"
#include "esp8266.h"

char txBuffer[USART_MAX_BUFFER_SIZE];
char rxBuffer[USART_MAX_BUFFER_SIZE];

uint8_t rxIndex = 0;

extern ErrorStatus atResponse;

extern FunctionalState wifiInitState;

extern FunctionalState wifiState;
extern FunctionalState mqttState;

extern uint8_t alarmIntensity;
extern uint8_t ledIntensity;

extern float temperatureUpperBound;
extern float temperatureLowerBound;
extern float humidityUpperBound;
extern float humidityLowerBound;
extern float illuminationUpperBound;
extern float illuminationLowerBound;

/*
处理JSON数据
 */
static void processJSONData(const char *json)
{
	cJSON *root = cJSON_Parse(json);

	if (root != NULL)
	{
		cJSON *ClientState = cJSON_GetObjectItem(root, "ClientState");
		cJSON *LEDIntensity = cJSON_GetObjectItem(root, "LED_Intensity");
		cJSON *AlarmIntensity = cJSON_GetObjectItem(root, "Alarm_Intensity");
		cJSON *TemperatureUpperBound = cJSON_GetObjectItem(root, "TemperatureH");
		cJSON *TemperatureLowerBound = cJSON_GetObjectItem(root, "TemperatureL");
		cJSON *HumidityUpperBound = cJSON_GetObjectItem(root, "HumidityH");
		cJSON *HumidityLowerBound = cJSON_GetObjectItem(root, "HumidityL");
		cJSON *IlluminationUpperBound = cJSON_GetObjectItem(root, "IlluminationH");
		cJSON *IlluminationLowerBound = cJSON_GetObjectItem(root, "IlluminationL");

		if (ClientState != NULL && strcmp(ClientState->valuestring, "READY") == 0)
			MQTT_Publish(ORIGIN_STATE_TOPIC, "{\\\"LED_Intensity\\\":%d\\,\\\"Alarm_Intensity\\\":%d\\,\\\"TemperatureH\\\":%.1f\\,\\\"TemperatureL\\\":%.1f\\,\\\"HumidityH\\\":%.1f\\,\\\"HumidityL\\\":%.1f\\,\\\"IlluminationH\\\":%.1f\\,\\\"IlluminationL\\\":%.1f}",
						 ledIntensity, alarmIntensity, temperatureUpperBound, temperatureLowerBound, humidityUpperBound, humidityLowerBound, illuminationUpperBound, illuminationLowerBound);

		if (LEDIntensity != NULL)
			LED_White_Control(LEDIntensity->valueint);
		if (AlarmIntensity != NULL)
			Alarm_Control(AlarmIntensity->valueint);

		if (TemperatureUpperBound != NULL)
			temperatureUpperBound = TemperatureUpperBound->valuedouble;
		if (TemperatureLowerBound != NULL)
			temperatureLowerBound = TemperatureLowerBound->valuedouble;
		if (HumidityUpperBound != NULL)
			humidityUpperBound = HumidityUpperBound->valuedouble;
		if (HumidityLowerBound != NULL)
			humidityLowerBound = HumidityLowerBound->valuedouble;
		if (IlluminationUpperBound != NULL)
			illuminationUpperBound = IlluminationUpperBound->valuedouble;
		if (IlluminationLowerBound != NULL)
			illuminationLowerBound = IlluminationLowerBound->valuedouble;

		cJSON_Delete(root);
	}
}

/*
处理接收到的数据
 */
static void processReceivedData(const char *data)
{
	if (strstr(data, "MQTTSUBRECV") != NULL && strstr(data, CLIENT_STATE_TOPIC) != NULL)
	{
		char *start = strchr(data, '{');
		char *end = strchr(data, '}');

		if (start != NULL && end != NULL && start < end)
		{
			size_t jsonLength = end - start + 1;
			char *json = malloc(jsonLength + 1);

			if (json != NULL)
			{
				strncpy(json, start, jsonLength);
				json[jsonLength] = '\0';

				processJSONData(json);

				free(json);
			}
		}
	}

	else if (strstr(data, "OK") != NULL)
		atResponse = SUCCESS;
	else if (strstr(data, "WIFI CONNECTED") != NULL)
		wifiState = ENABLE;
	else if (strstr(data, "WIFI DISCONNECT") != NULL)
		wifiState = DISABLE;
	else if (strstr(data, "MQTTCONNECTED") != NULL)
		mqttState = ENABLE;
	else if (strstr(data, "MQTTDISCONNECTED") != NULL)
		mqttState = DISABLE;

	if (wifiState && mqttState)
		wifiInitState = ENABLE;
	else
		wifiInitState = DISABLE;
}

/*
初始化串口
 */
void USARTx_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

	USART_Cmd(USART1, ENABLE); // 使能串口

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); // 使能接收中断

	// PA2	TXD
	GPIO_InitStructure.GPIO_Pin = USART1_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART1_PORT, &GPIO_InitStructure);

	// PA3	RXD
	GPIO_InitStructure.GPIO_Pin = USART1_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(USART1_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// 接收和发送
	USART_InitStructure.USART_Parity = USART_Parity_No;								// 无校验
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 1位停止位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// 8位数据位
	USART_Init(USART1, &USART_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStructure);
}

/*
串口发送字符串
 */
void USART_SendString(USART_TypeDef *USARTx, const char *str)
{
	while (*str)
	{
		USART_SendData(USARTx, *str++);
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
			;
	}
}

/*
串口中断服务函数
 */
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		char receivedChar = USART_ReceiveData(USART1);

		if (receivedChar == '\r' || receivedChar == '\n')
		{
			rxBuffer[rxIndex] = '\0';
			processReceivedData(rxBuffer);
			memset(rxBuffer, 0, sizeof(rxBuffer));
			rxIndex = 0;
		}
		else
		{
			rxBuffer[rxIndex++] = receivedChar;
			if (rxIndex >= USART_MAX_BUFFER_SIZE - 1)
				rxIndex = 0;
		}
	}
}
