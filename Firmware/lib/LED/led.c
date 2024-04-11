#include "led.h"
#include "esp8266.h"
#include "oled.h"

volatile uint8_t ledIntensity = 0;

/*
初始化LED
 */
void LEDx_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // 使能时钟

    GPIO_InitStructure.GPIO_Pin = LED_RED_PIN | LED_WHITE_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;   // 复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // IO口速度为50MHz
    GPIO_Init(LED_PORT, &GPIO_InitStructure);         // 根据设定参数初始化GPIO
}

/*
控制LED_RED
参数：
    intensity：LED_RED亮度，范围：0~99，0表示关闭LED_RED，99表示最大亮度
 */
void LED_Red_Control(int8_t intensity)
{
    if (intensity < 0)
        TIM_SetCompare2(TIM3, RESET);
    else if (intensity > 99)
        TIM_SetCompare2(TIM3, 99);
    else
        TIM_SetCompare2(TIM3, intensity);
}

/*
控制LED_WHITE
参数：
    intensity：LED_WHITE亮度，范围：0~99，0表示关闭LED_WHITE，99表示最大亮度
 */
void LED_White_Control(int8_t intensity)
{
    if (intensity < 0)
        TIM_SetCompare2(TIM2, ledIntensity = RESET);
    else if (intensity > 99)
        TIM_SetCompare2(TIM2, ledIntensity = 99);
    else
        TIM_SetCompare2(TIM2, ledIntensity = intensity);

    MQTT_Publish(ORIGIN_STATE_TOPIC, "{\\\"LED_Intensity\\\":%d}", ledIntensity);

    OLED_DrawUTF8Lines(0, 15, 32, 16, "%d%%", ledIntensity);
}
