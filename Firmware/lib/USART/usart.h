#ifndef USART_H
#define USART_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include "cJSON.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "misc.h"

#define USART1_PORT GPIOA
#define USART1_TX_PIN GPIO_Pin_9
#define USART1_RX_PIN GPIO_Pin_10

#define USART_MAX_BUFFER_SIZE 256 // 最大缓存

    void USARTx_Init(void);
    void USART_SendString(USART_TypeDef *USARTx, const char *str);

#ifdef __cplusplus
}
#endif

#endif
