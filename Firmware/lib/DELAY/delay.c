#include "delay.h"

/*
初始化延时函数
 */
void Delay_Init(void)
{
   SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); // 选择外部时钟  HCLK/8
   SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;            // 关闭定时器
}

/*
us延时程序
 */
void Delayus(uint32_t us)
{
   uint32_t temp;
   SysTick->LOAD = (SystemCoreClock / 8) / 1000000 * us; // 时间加载
   SysTick->VAL = 0x00;                                  // 清空计数器
   SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;             // 开始倒数
   do
   {
      temp = SysTick->CTRL;
   } while ((temp & 0x01) && !(temp & (1 << 16))); // 等待时间到达
   SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;      // 关闭计数器
   SysTick->VAL = 0X00;                            // 清空计数器
}

/*
ms延时程序
 */
void Delayms(uint16_t ms)
{
   uint32_t temp;
   SysTick->LOAD = (SystemCoreClock / 8) / 1000 * ms; // 时间加载
   SysTick->VAL = 0x00;                               // 清空计数器
   SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;          // 开始倒数
   do
   {
      temp = SysTick->CTRL;
   } while ((temp & 0x01) && !(temp & (1 << 16))); // 等待时间到达
   SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;      // 关闭计数器
   SysTick->VAL = 0X00;                            // 清空计数器
}
