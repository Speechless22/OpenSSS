#include "timer.h"
#include "delay.h"
#include "buzzer.h"
#include "button.h"
#include "led.h"
#include "alarm.h"
#include "esp8266.h"

extern uint8_t ledIntensity;
extern uint8_t alarmIntensity;

extern uint16_t espTimeoutCounter;

extern FunctionalState wifiState;

/*
初始化定时器
 */
void TIMx_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE); // 时钟使能

	// 定时器频率 = 定时器输入频率 / ((预分频器 + 1) * (定时器周期 + 1))
	TIM_TimeBaseStructure.TIM_Period = 100 - 1;								  // 设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 2) / 400000 - 1; // 设置用来作为TIMx时钟频率除数的预分频值，BUZZER频率为4000Hz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;					  // 设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;				  // TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);							  // 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);							  // 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

	TIM_TimeBaseStructure.TIM_Prescaler = (SystemCoreClock / 2) / 1000 - 1; // TIM4频率为10Hz，即100ms
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;			  // 选择定时器模式:TIM脉冲宽度调制模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 比较输出使能
	TIM_OCInitStructure.TIM_Pulse = 0;							  // 设置待装入捕获比较寄存器的脉冲值
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	  // 输出极性:TIM输出比较极性高
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);					  // 根据指定的参数初始化外设TIMx
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);					  // 根据指定的参数初始化外设TIMx
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);					  // 根据指定的参数初始化外设TIMx

	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable); // 使能TIM2的预装载寄存器
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); // 使能TIM3的预装载寄存器
	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable); // 使能TIM3的预装载寄存器

	/* TIM Interrupts enable */
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM2, ENABLE); // 使能TIMx外设
	TIM_Cmd(TIM3, ENABLE); // 使能TIMx外设
	TIM_Cmd(TIM4, ENABLE); // 使能TIMx外设

	/* NVIC configuration */
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
中断服务函数
 */
void TIM4_IRQHandler(void)
{
	static uint32_t ledCounter = 0;
	static uint32_t alarmCounter = 0;
	static uint32_t offlineCounter = 0;

	static FlagStatus ledIncrease = SET;   // 增加标志
	static FlagStatus alarmIncrease = SET; // 增加标志

	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET) // 检查是否为TIM4更新中断
	{
		if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_ALARM_PIN) == Bit_RESET) // 按键被按下
		{
			alarmCounter++;

			if (alarmCounter >= 10) // 长按事件
			{
				alarmCounter = 0;
				// 处理长按事件
				if (alarmIncrease)
				{
					Alarm_Control(alarmIntensity + 10);
					if (alarmIntensity == 99)
						alarmIncrease = RESET;
				}
				else
				{
					Alarm_Control(alarmIntensity - 10);
					if (alarmIntensity == 0)
						alarmIncrease = SET;
				}
			}
		}
		else
			alarmCounter = 0;

		if (GPIO_ReadInputDataBit(BUTTON_PORT, BUTTON_LED_PIN) == Bit_RESET) // 按键被按下
		{
			ledCounter++;

			if (ledCounter >= 10) // 长按事件
			{
				ledCounter = 0;
				// 处理长按事件
				if (ledIncrease)
				{
					LED_White_Control(ledIntensity + 10);
					if (ledIntensity == 99)
						ledIncrease = RESET;
				}
				else
				{
					LED_White_Control(ledIntensity - 10);
					if (ledIntensity == 0)
						ledIncrease = SET;
				}
			}
		}
		else
			ledCounter = 0;

		if (GPIO_ReadInputDataBit(RESET_FLAG_PORT, RESET_FLAG_PIN) == Bit_RESET) // 按键被按下
		{
			offlineCounter++;

			if (offlineCounter >= 30) // 长按事件
			{
				offlineCounter = 0;
				// 处理长按事件

				BUZZER_Control(99);
				Delayms(100);
				BUZZER_Control(alarmIntensity);

				if (wifiState)
				{
					AT_SendCommand("AT+CWQAP\r\n"); // 断开WIFI连接
				}
				else if (espTimeoutCounter <= 10)
				{
					AT_SendCommand("AT+CWJAP\r\n"); // 连接WIFI
				}
				espTimeoutCounter = 10;
			}
		}
		else
			offlineCounter = 0;

		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
	}
}
