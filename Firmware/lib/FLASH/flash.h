#ifndef FLASH_H
#define FLASH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <string.h>
#include "stm32f10x_flash.h"

#define FLASH_SAVE_ADDR ((uint32_t)0x0800FC00) // Flash的最后一页 0x08000000 + 65536 - 1024 = 0x0800FC00

#define WIFI_INIT_STATE_FLASH_SAVE_ADDR FLASH_SAVE_ADDR // 是否运行WiFi初始化保存地址

#define ALARM_INTENSITY_FLASH_SAVE_ADDR ((uint32_t)0x0800FC04)     // Alarm强度保存地址
#define LED_WHITE_INTENSITY_FLASH_SAVE_ADDR ((uint32_t)0x0800FC08) // LED白灯强度保存地址

#define TEMPERATURE_UPPER_BOUND_FLASH_SAVE_ADDR ((uint32_t)0x0800FC0C)  // 温度上限保存地址
#define TEMPERATURE_LOWER_BOUND_FLASH_SAVE_ADDR ((uint32_t)0x0800FC10)  // 温度下限保存地址
#define HUMIDITY_UPPER_BOUND_FLASH_SAVE_ADDR ((uint32_t)0x0800FC14)     // 湿度上限保存地址
#define HUMIDITY_LOWER_BOUND_FLASH_SAVE_ADDR ((uint32_t)0x0800FC18)     // 湿度下限保存地址
#define ILLUMINATION_UPPER_BOUND_FLASH_SAVE_ADDR ((uint32_t)0x0800FC1C) // 照度上限保存地址
#define ILLUMINATION_LOWER_BOUND_FLASH_SAVE_ADDR ((uint32_t)0x0800FC20) // 照度下限保存地址

    void Flash_Init(void);
    void SaveToFlash(void);
    void ReadFromFlash(void);

#ifdef __cplusplus
}
#endif

#endif
