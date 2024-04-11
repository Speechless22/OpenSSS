#include "oled.h"
#include "delay.h"

u8g2_t u8g2;

/*
u8g2回调函数
参数：
    u8x8：u8x8结构体
    msg：消息
    arg_int：整型参数
    arg_ptr：指针参数
 */
static uint8_t u8x8_stm32_gpio_and_delay_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg)
    {
    case U8X8_MSG_GPIO_AND_DELAY_INIT: // called once during init phase of u8g2/u8x8
    {
        GPIO_InitTypeDef GPIO_InitStructure;

        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

        GPIO_InitStructure.GPIO_Pin = OLED_SCL_PIN | OLED_SDA_PIN;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
        GPIO_Init(OLED_PORT, &GPIO_InitStructure); // 初始化SCL和SDA引脚为输出

        Delay_Init();

        break; // can be used to setup pins
    }

    case U8X8_MSG_DELAY_NANO: // delay arg_int * 1 nano second
        Delayus(arg_int / 1000);
        break;

    case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
        Delayus(arg_int / 10);
        break;

    case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
        Delayus(arg_int * 10);
        break;

    case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
        Delayms(arg_int);
        break;

    case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
        break;               // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us

    case U8X8_MSG_GPIO_D0: // D0 or SPI clock pin: Output level in arg_int
                           // case U8X8_MSG_GPIO_SPI_CLOCK:
        break;

    case U8X8_MSG_GPIO_D1: // D1 or SPI data pin: Output level in arg_int
                           // case U8X8_MSG_GPIO_SPI_DATA:
        break;

    case U8X8_MSG_GPIO_D2: // D2 pin: Output level in arg_int
        break;

    case U8X8_MSG_GPIO_D3: // D3 pin: Output level in arg_int
        break;

    case U8X8_MSG_GPIO_D4: // D4 pin: Output level in arg_int
        break;

    case U8X8_MSG_GPIO_D5: // D5 pin: Output level in arg_int
        break;

    case U8X8_MSG_GPIO_D6: // D6 pin: Output level in arg_int
        break;

    case U8X8_MSG_GPIO_D7: // D7 pin: Output level in arg_int
        break;

    case U8X8_MSG_GPIO_E: // E/WR pin: Output level in arg_int
        break;

    case U8X8_MSG_GPIO_CS: // CS (chip select) pin: Output level in arg_int
        break;
    case U8X8_MSG_GPIO_DC: // DC (data/cmd, A0, register select) pin: Output level in arg_int
        break;

    case U8X8_MSG_GPIO_RESET: // Reset pin: Output level in arg_int
        break;

    case U8X8_MSG_GPIO_CS1: // CS1 (chip select) pin: Output level in arg_int
        break;

    case U8X8_MSG_GPIO_CS2: // CS2 (chip select) pin: Output level in arg_int
        break;

    case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
        if (arg_int)              // 如果arg_int为真，设置SCL引脚为高
            GPIO_SetBits(OLED_PORT, OLED_SCL_PIN);
        else // 否则，设置SCL引脚为低
            GPIO_ResetBits(OLED_PORT, OLED_SCL_PIN);
        break; // arg_int=1: Input dir with pullup high for I2C clock pin

    case U8X8_MSG_GPIO_I2C_DATA: // arg_int=0: Output low at I2C data pin
        if (arg_int)             // 如果arg_int为真，设置SDA引脚为高
            GPIO_SetBits(OLED_PORT, OLED_SDA_PIN);
        else // 否则，设置SDA引脚为低
            GPIO_ResetBits(OLED_PORT, OLED_SDA_PIN);
        break; // arg_int=1: Input dir with pullup high for I2C data pin

    case U8X8_MSG_GPIO_MENU_SELECT:
        u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
        break;

    case U8X8_MSG_GPIO_MENU_NEXT:
        u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
        break;

    case U8X8_MSG_GPIO_MENU_PREV:
        u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
        break;

    case U8X8_MSG_GPIO_MENU_HOME:
        u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
        break;

    default:
        u8x8_SetGPIOResult(u8x8, 1); // default return value
        break;
    }
    return 1;
}

/*
初始化OLED
 */
void OLED_Init(void)
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8x8_stm32_gpio_and_delay_cb);
    // u8g2_Setup_sh1106_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8x8_stm32_gpio_and_delay_cb);

    u8g2_InitDisplay(&u8g2);
    u8g2_SetPowerSave(&u8g2, 0);
}

/*
在指定位置绘制多行UTF8字符串
参数：
    x：x坐标
    y：y坐标
    w：宽度
    line_height：行高
    format：格式化字符串
    ...：可变参数
 */
void OLED_DrawUTF8Lines(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t line_height, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    int length = vsnprintf(NULL, 0, format, args); // 使用 vsnprintf 计算格式化后的字符串长度
    va_end(args);

    char *str = (char *)malloc(length + 1); // 分配足够的内存来存储格式化后的字符串

    va_start(args, format);
    vsnprintf(str, length + 1, format, args); // 再次使用 vsnprintf 将带有变量的 str 格式化到新分配的内存中
    va_end(args);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);

    OLED_Clear(x, y - line_height + 1, w, u8g2_DrawUTF8Lines(&u8g2, x, y, w, line_height, str));
    u8g2_DrawUTF8Lines(&u8g2, x, y, w, line_height, str); // 默认基点为左下角
    u8g2_SendBuffer(&u8g2);

    free(str); // 释放分配的内存
}

/*
在指定位置绘制多行UTF8字符串并滚动
参数：
    x：x坐标
    y：y坐标
    w：宽度
    line_height：行高
    speed：滚动速度
    format：格式化字符串
    ...：可变参数
 */
void OLED_ScrollDrawUTF8Lines(u8g2_int_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t line_height, u8g2_uint_t speed, const char *format, ...)
{
    va_list args;
    va_start(args, format);

    int length = vsnprintf(NULL, 0, format, args); // 使用 vsnprintf 计算格式化后的字符串长度
    va_end(args);

    char *str = (char *)malloc(length + 1); // 分配足够的内存来存储格式化后的字符串

    va_start(args, format);
    vsnprintf(str, length + 1, format, args); // 再次使用 vsnprintf 将带有变量的 str 格式化到新分配的内存中
    va_end(args);

    u8g2_SetFont(&u8g2, u8g2_font_unifont_t_symbols);

    for (; x > -w; x -= speed)
    {
        OLED_Clear(x, y - line_height + 1, w, u8g2_DrawUTF8Lines(&u8g2, x, y, w, line_height, str));
        u8g2_DrawUTF8Lines(&u8g2, x, y, w, line_height, str); // 默认基点为左下角
        u8g2_SendBuffer(&u8g2);
    }

    free(str); // 释放分配的内存
}

/*
在指定位置清除指定大小的区域
参数：
    x：x坐标
    y：y坐标
    w：宽度
    h：高度
 */
void OLED_Clear(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t w, u8g2_uint_t h)
{
    u8g2_SetDrawColor(&u8g2, 0);
    u8g2_DrawBox(&u8g2, x, y, w, h); // 基点为左上角
    u8g2_SetDrawColor(&u8g2, 1);
}
