#ifndef BUTTON_H
#define BUTTON_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "misc.h"

#define BUTTON_PORT GPIOA
#define BUTTON_LED_PIN GPIO_Pin_11
#define BUTTON_ALARM_PIN GPIO_Pin_12

#define RESET_FLAG_PORT GPIOB
#define RESET_FLAG_PIN GPIO_Pin_1

    void BUTTONx_Init(void);

#ifdef __cplusplus
}
#endif

#endif
