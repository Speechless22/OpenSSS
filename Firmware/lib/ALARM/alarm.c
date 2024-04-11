#include "alarm.h"
#include "led.h"
#include "buzzer.h"
#include "esp8266.h"
#include "oled.h"

volatile uint8_t alarmIntensity = 0;

volatile float temperatureUpperBound = 50.f;	 // 温度上限
volatile float temperatureLowerBound = -50.f;	 // 温度下限
volatile float humidityUpperBound = 100.f;		 // 湿度上限
volatile float humidityLowerBound = 0.f;		 // 湿度下限
volatile float illuminationUpperBound = 10000.f; // 照度上限
volatile float illuminationLowerBound = 0.f;	 // 照度下限

extern float temperature;
extern float humidity;
extern float illumination;

/*
控制报警器
参数：
    intensity：报警器亮度，范围：0~99，0表示关闭报警器，99表示最大亮度
 */
void Alarm_Control(int8_t intensity)
{
    if (intensity < 0)
    {
        BUZZER_Control(alarmIntensity = RESET);
        LED_Red_Control(alarmIntensity = RESET);
    }
    else if (intensity > 99)
    {
        BUZZER_Control(alarmIntensity = 99);
        LED_Red_Control(alarmIntensity = 99);
    }
    else
    {
        BUZZER_Control(alarmIntensity = intensity);
        LED_Red_Control(alarmIntensity = intensity);
    }

    MQTT_Publish(ORIGIN_STATE_TOPIC, "{\\\"Alarm_Intensity\\\":%d}", alarmIntensity);

    OLED_DrawUTF8Lines(95, 15, 32, 16, "%d%%", alarmIntensity);
}

/*
自动报警
 */
void Auto_Alarm(void)
{
    if (temperature > temperatureUpperBound || temperature < temperatureLowerBound ||
        humidity > humidityUpperBound || humidity < humidityLowerBound ||
        illumination > illuminationUpperBound || illumination < illuminationLowerBound)
        Alarm_Control(99);
}
