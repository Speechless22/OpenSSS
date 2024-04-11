#include "button.h"
#include "delay.h"
#include "led.h"
#include "alarm.h"
#include "esp8266.h"

extern uint8_t ledIntensity;
extern uint8_t alarmIntensity;

extern float temperatureUpperBound;
extern float temperatureLowerBound;
extern float humidityUpperBound;
extern float humidityLowerBound;
extern float illuminationUpperBound;
extern float illuminationLowerBound;

/*
初始化按键
 */
void BUTTONx_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // 使能时钟

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource11); // 中断线以及中断初始化配置
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource12);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);

	GPIO_InitStructure.GPIO_Pin = BUTTON_LED_PIN | BUTTON_ALARM_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 设置成上拉输入
	GPIO_Init(BUTTON_PORT, &GPIO_InitStructure);  // 初始化GPIOA

	GPIO_InitStructure.GPIO_Pin = RESET_FLAG_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 设置成上拉输入
	GPIO_Init(RESET_FLAG_PORT, &GPIO_InitStructure); // 初始化GPIOB

	EXTI_InitStructure.EXTI_Line = EXTI_Line11 | EXTI_Line12 | EXTI_Line1; // 外部中断线
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; // 下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); // 根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;		  // 使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // 子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // 使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;	  // 使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		  // 子优先级0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			  // 使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);
}

/*
中断服务函数
 */
void EXTI15_10_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line11) != RESET) // 检查是否为第11个外部中断事件
	{
		Delayms(10);

		if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_LED_PIN) == Bit_RESET) // 按键被按下
		{
			LED_White_Control(ledIntensity > 0 ? 0 : 99);
		}

		EXTI_ClearITPendingBit(EXTI_Line11); // 清除LINE11线路挂起位
	}

	if (EXTI_GetITStatus(EXTI_Line12) != RESET)
	{
		Delayms(10);

		if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_ALARM_PIN) == Bit_RESET) // 按键被按下
		{
			Alarm_Control(alarmIntensity > 0 ? 0 : 99);
		}

		EXTI_ClearITPendingBit(EXTI_Line12); // 清除LINE12线路挂起位
	}
}

/*
中断服务函数
 */
void EXTI1_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line1) != RESET)
	{
		Delayms(10);

		if (GPIO_ReadInputDataBit(RESET_FLAG_PORT, RESET_FLAG_PIN) == Bit_RESET) // 按键被按下
		{
			MQTT_Publish(ORIGIN_STATE_TOPIC, "{\\\"TemperatureH\\\":%.1f\\,\\\"TemperatureL\\\":%.1f\\,\\\"HumidityH\\\":%.1f\\,\\\"HumidityL\\\":%.1f\\,\\\"IlluminationH\\\":%.1f\\,\\\"IlluminationL\\\":%.1f}",
						 temperatureUpperBound = 50, temperatureLowerBound = -50, humidityUpperBound = 100, humidityLowerBound = 0, illuminationUpperBound = 10000, illuminationLowerBound = 0);
		}

		EXTI_ClearITPendingBit(EXTI_Line1); // 清除LINE1上的中断标志位
	}
}
