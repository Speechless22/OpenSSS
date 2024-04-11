#include "clock.h"

void Clock_Init(void)
{
    RCC_DeInit(); // 重置RCC

    RCC_HSEConfig(RCC_HSE_ON); // 启动HSE（High Speed External）时钟
    while (RCC_WaitForHSEStartUp() != SUCCESS)
        ;

    RCC_HCLKConfig(RCC_SYSCLK_Div1); // HCLK = SYSCLK

    RCC_PCLK1Config(RCC_HCLK_Div2); // PCLK1 = HCLK/2

    RCC_PCLK2Config(RCC_HCLK_Div1); // PCLK2 = HCLK

    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); // 如果HSE频率为8MHz，那么PLLCLK（PLL时钟）为8MHz * 9 = 72MHz

    RCC_PLLCmd(ENABLE); // 启动PLL

    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) // 等待PLL稳定
        ;

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); // PLLCLK作为系统时钟

    while (RCC_GetSYSCLKSource() != 0x08) // 等待PLLCLK稳定
        ;

    SystemCoreClockUpdate(); // 更新SystemCoreClock变量
}
