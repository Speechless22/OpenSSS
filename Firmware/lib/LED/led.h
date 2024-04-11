#ifndef LED_H
#define LED_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

#define LED_PORT GPIOA
#define LED_RED_PIN GPIO_Pin_7
#define LED_WHITE_PIN GPIO_Pin_1

    void LEDx_Init(void);
    void LED_Red_Control(int8_t intensity);
    void LED_White_Control(int8_t intensity);

#ifdef __cplusplus
}
#endif

#endif
