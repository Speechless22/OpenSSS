#include "buzzer.h"

/*
初始化蜂鸣器
 */
void BUZZER_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能PA端口时钟

    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO口速度为50MHz
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);
}

/*
控制蜂鸣器
参数：
    intensity：蜂鸣器强度，范围：0~99，0表示关闭蜂鸣器，99表示最大强度
 */
void BUZZER_Control(int8_t intensity)
{
    if (intensity < 0)
        TIM_SetCompare1(TIM3, RESET);
    else if (intensity > 99)
        TIM_SetCompare1(TIM3, 49);
    else
        TIM_SetCompare1(TIM3, intensity / 2);
}
