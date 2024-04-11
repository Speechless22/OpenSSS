#include "flash.h"

FunctionalState oldWifiInitState = DISABLE;

uint8_t oldAlarmIntensity = 0;
uint8_t oldLedWhiteIntensity = 0;

float oldTemperatureUpperBound = 0.f;
float oldTemperatureLowerBound = 0.f;
float oldHumidityUpperBound = 0.f;
float oldHumidityLowerBound = 0.f;
float oldIlluminationUpperBound = 0.f;
float oldIlluminationLowerBound = 0.f;

extern FunctionalState wifiInitState;

extern uint8_t alarmIntensity;
extern uint8_t ledIntensity;

extern float temperatureUpperBound;
extern float temperatureLowerBound;
extern float humidityUpperBound;
extern float humidityLowerBound;
extern float illuminationUpperBound;
extern float illuminationLowerBound;

/*
初始化FLASH
 */
void Flash_Init(void)
{
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); // 配置FLASH预取指
    FLASH_SetLatency(FLASH_Latency_2);                    // 配置FLASH延时周期
}

/*
保存数据到FLASH
 */
void SaveToFlash(void)
{
    uint32_t temp;

    if (wifiInitState != oldWifiInitState ||
        alarmIntensity != oldAlarmIntensity ||
        ledIntensity != oldLedWhiteIntensity ||
        temperatureUpperBound != oldTemperatureUpperBound ||
        temperatureLowerBound != oldTemperatureLowerBound ||
        humidityUpperBound != oldHumidityUpperBound ||
        humidityLowerBound != oldHumidityLowerBound ||
        illuminationUpperBound != oldIlluminationUpperBound ||
        illuminationLowerBound != oldIlluminationLowerBound)
    {
        oldWifiInitState = wifiInitState;
        oldAlarmIntensity = alarmIntensity;
        oldLedWhiteIntensity = ledIntensity;
        oldTemperatureUpperBound = temperatureUpperBound;
        oldTemperatureLowerBound = temperatureLowerBound;
        oldHumidityUpperBound = humidityUpperBound;
        oldHumidityLowerBound = humidityLowerBound;
        oldIlluminationUpperBound = illuminationUpperBound;
        oldIlluminationLowerBound = illuminationLowerBound;

        FLASH_Unlock();

        FLASH_ErasePage(FLASH_SAVE_ADDR);

        FLASH_ProgramWord(WIFI_INIT_STATE_FLASH_SAVE_ADDR, wifiInitState);

        FLASH_ProgramWord(ALARM_INTENSITY_FLASH_SAVE_ADDR, alarmIntensity);
        FLASH_ProgramWord(LED_WHITE_INTENSITY_FLASH_SAVE_ADDR, ledIntensity);

        memcpy(&temp, &temperatureUpperBound, sizeof(temp));
        FLASH_ProgramWord(TEMPERATURE_UPPER_BOUND_FLASH_SAVE_ADDR, temp);
        memcpy(&temp, &temperatureLowerBound, sizeof(temp));
        FLASH_ProgramWord(TEMPERATURE_LOWER_BOUND_FLASH_SAVE_ADDR, temp);

        memcpy(&temp, &humidityUpperBound, sizeof(temp));
        FLASH_ProgramWord(HUMIDITY_UPPER_BOUND_FLASH_SAVE_ADDR, temp);
        memcpy(&temp, &humidityLowerBound, sizeof(temp));
        FLASH_ProgramWord(HUMIDITY_LOWER_BOUND_FLASH_SAVE_ADDR, temp);

        memcpy(&temp, &illuminationUpperBound, sizeof(temp));
        FLASH_ProgramWord(ILLUMINATION_UPPER_BOUND_FLASH_SAVE_ADDR, temp);
        memcpy(&temp, &illuminationLowerBound, sizeof(temp));
        FLASH_ProgramWord(ILLUMINATION_LOWER_BOUND_FLASH_SAVE_ADDR, temp);

        FLASH_Lock();
    }
}

/*
从FLASH读取数据
 */
void ReadFromFlash(void)
{
    wifiInitState = (*(__IO uint8_t *)WIFI_INIT_STATE_FLASH_SAVE_ADDR);

    alarmIntensity = (*(__IO uint8_t *)ALARM_INTENSITY_FLASH_SAVE_ADDR);
    ledIntensity = (*(__IO uint8_t *)LED_WHITE_INTENSITY_FLASH_SAVE_ADDR);

    temperatureUpperBound = (*(__IO float *)TEMPERATURE_UPPER_BOUND_FLASH_SAVE_ADDR);
    temperatureLowerBound = (*(__IO float *)TEMPERATURE_LOWER_BOUND_FLASH_SAVE_ADDR);
    humidityUpperBound = (*(__IO float *)HUMIDITY_UPPER_BOUND_FLASH_SAVE_ADDR);
    humidityLowerBound = (*(__IO float *)HUMIDITY_LOWER_BOUND_FLASH_SAVE_ADDR);
    illuminationUpperBound = (*(__IO float *)ILLUMINATION_UPPER_BOUND_FLASH_SAVE_ADDR);
    illuminationLowerBound = (*(__IO float *)ILLUMINATION_LOWER_BOUND_FLASH_SAVE_ADDR);
}
