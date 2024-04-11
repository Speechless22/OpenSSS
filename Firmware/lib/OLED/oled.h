#ifndef OLED_H
#define OLED_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "u8g2.h"

#define OLED_PORT GPIOB
#define OLED_SCL_PIN GPIO_Pin_6
#define OLED_SDA_PIN GPIO_Pin_7

    void OLED_Init(void);
    void OLED_DrawUTF8Lines(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t line_height, const char *format, ...);
    void OLED_ScrollDrawUTF8Lines(u8g2_int_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t line_height, u8g2_uint_t speed, const char *format, ...);
    void OLED_Clear(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t h);

#ifdef __cplusplus
}
#endif

#endif
