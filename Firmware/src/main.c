#include "alarm.h"
#include "button.h"
#include "buzzer.h"
#include "clock.h"
#include "delay.h"
#include "driver_bh1750fvi_basic.h"
#include "driver_sht4x_basic.h"
#include "esp8266.h"
#include "flash.h"
#include "led.h"
#include "oled.h"
#include "timer.h"
#include "usart.h"

float temperature = 0;	// 温度
float humidity = 0;		// 湿度
float illumination = 0; // 照度

extern float temperatureUpperBound;
extern float temperatureLowerBound;
extern float humidityUpperBound;
extern float humidityLowerBound;
extern float illuminationUpperBound;
extern float illuminationLowerBound;

extern FunctionalState wifiState;
extern FunctionalState mqttState;

extern uint8_t alarmIntensity;
extern uint8_t ledIntensity;

int main(void)
{
	Flash_Init();

	Clock_Init();

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	OLED_Init(); // 包含Delay_Init()

	ReadFromFlash();

	OLED_DrawUTF8Lines(0, 15, 128, 16, "Starting");

	OLED_DrawUTF8Lines(0, 31, 0, 16, "BUTTON");
	BUTTONx_Init();
	OLED_DrawUTF8Lines(79, 31, 48, 16, "[Done]");

	OLED_DrawUTF8Lines(0, 47, 0, 16, "LED");
	LEDx_Init();
	OLED_DrawUTF8Lines(79, 47, 48, 16, "[Done]");

	OLED_DrawUTF8Lines(0, 63, 0, 16, "BUZZER");
	BUZZER_Init();
	OLED_DrawUTF8Lines(79, 63, 48, 16, "[Done]");

	OLED_Clear(0, 16, 128, 64);

	OLED_DrawUTF8Lines(0, 31, 0, 16, "TIMER");
	TIMx_Init();
	OLED_DrawUTF8Lines(79, 31, 48, 16, "[Done]");

	OLED_DrawUTF8Lines(0, 47, 0, 16, "SHT4x");
	sht4x_basic_init(SHT4X_ADDRESS_0);
	OLED_DrawUTF8Lines(79, 47, 48, 16, "[Done]");

	OLED_DrawUTF8Lines(0, 63, 0, 16, "BH1750FVI");
	bh1750fvi_basic_init(BH1750FVI_ADDRESS_LOW);
	OLED_DrawUTF8Lines(79, 63, 48, 16, "[Done]");

	OLED_Clear(0, 32, 128, 64);

	OLED_DrawUTF8Lines(0, 15, 128, 16, "Waiting for ESP\nto initialize");

	OLED_DrawUTF8Lines(0, 47, 0, 16, "USART");
	USARTx_Init();
	OLED_DrawUTF8Lines(79, 47, 48, 16, "[Done]");

	OLED_DrawUTF8Lines(0, 63, 0, 16, "ESP8266");
	ESP8266_Init();
	OLED_DrawUTF8Lines(79, 63, 48, 16, "[Done]");

	MQTT_Subscribe(CLIENT_STATE_TOPIC);

	LED_White_Control(ledIntensity); // 客厅灯亮度初始化
	OLED_DrawUTF8Lines(31, 15, 64, 16, "%s", (wifiState && mqttState) ? "Online" : "Offline");
	Alarm_Control(alarmIntensity); // 报警器强度初始化

	MQTT_Publish(ORIGIN_STATE_TOPIC, "{\\\"TemperatureH\\\":%.1f\\,\\\"TemperatureL\\\":%.1f\\,\\\"HumidityH\\\":%.1f\\,\\\"HumidityL\\\":%.1f\\,\\\"IlluminationH\\\":%.1f\\,\\\"IlluminationL\\\":%.1f}",
				 temperatureUpperBound, temperatureLowerBound, humidityUpperBound, humidityLowerBound, illuminationUpperBound, illuminationLowerBound);

	while (SET)
	{
		sht4x_basic_read(&temperature, &humidity);
		bh1750fvi_basic_read(&illumination);

		OLED_DrawUTF8Lines(31, 15, 64, 16, "%s", (wifiState && mqttState) ? "Online" : "Offline");

		OLED_DrawUTF8Lines(0, 31, 128, 16, "Temp: %.1f ℃\nHum: %.1f %%\nIll: %.1f lx",
						   temperature, humidity, illumination);

		MQTT_Publish(ORIGIN_STATE_TOPIC, "{\\\"Temperature\\\":%.1f\\,\\\"Humidity\\\":%.1f\\,\\\"Illumination\\\":%.1f}",
					 temperature, humidity, illumination);

		Delayms(3000);

		OLED_DrawUTF8Lines(0, 31, 128, 16, "%.1f<Temp<%.1f\n%.1f<Hum<%.1f\n%.1f<Ill<%.1f",
						   temperatureLowerBound, temperatureUpperBound,
						   humidityLowerBound, humidityUpperBound,
						   illuminationLowerBound, illuminationUpperBound);

		Auto_Alarm();

		Delayms(3000);

		SaveToFlash();
	}
}
