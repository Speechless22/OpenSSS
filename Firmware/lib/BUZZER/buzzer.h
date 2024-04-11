#ifndef BUZZER_H
#define BUZZER_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_tim.h"

#define BUZZER_PORT GPIOA
#define BUZZER_PIN GPIO_Pin_6

    void BUZZER_Init(void);
    void BUZZER_Control(int8_t intensity);

#ifdef __cplusplus
}
#endif

#endif
