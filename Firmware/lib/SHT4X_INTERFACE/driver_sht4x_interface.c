/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      driver_sht4x_interface_template.c
 * @brief     driver sht4x interface template source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2023-06-25
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2023/06/25  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_sht4x_interface.h"
#include "delay.h"

/**
 * @brief iic bus send start
 * @note  none
 */
static void a_iic_start(void)
{
    SHT4x_SDA_OUT();
    SHT4x_IIC_SDA = 1;
    SHT4x_IIC_SCL = 1;
    Delayus(4);
    SHT4x_IIC_SDA = 0;
    Delayus(4);
    SHT4x_IIC_SCL = 0;
}

/**
 * @brief iic bus send stop
 * @note  none
 */
static void a_iic_stop(void)
{
    SHT4x_SDA_OUT();
    SHT4x_IIC_SCL = 0;
    SHT4x_IIC_SDA = 0;
    Delayus(4);
    SHT4x_IIC_SCL = 1;
    Delayus(4);
    SHT4x_IIC_SDA = 1;
    Delayus(4);
}

/**
 * @brief  iic wait ack
 * @return status code
 *         - 0 get ack
 *         - 1 no ack
 * @note   none
 */
static uint8_t a_iic_wait_ack(void)
{
    uint16_t uc_err_time = 0;

    SHT4x_SDA_IN();
    SHT4x_IIC_SDA = 1;
    Delayus(1);
    SHT4x_IIC_SCL = 1;
    Delayus(1);
    while (SHT4x_READ_SDA != 0)
    {
        uc_err_time++;
        if (uc_err_time > 250)
        {
            a_iic_stop();

            return 1;
        }
    }
    SHT4x_IIC_SCL = 0;

    return 0;
}

/**
 * @brief iic bus send ack
 * @note  none
 */
static void a_iic_ack(void)
{
    SHT4x_IIC_SCL = 0;
    SHT4x_SDA_OUT();
    SHT4x_IIC_SDA = 0;
    Delayus(2);
    SHT4x_IIC_SCL = 1;
    Delayus(2);
    SHT4x_IIC_SCL = 0;
}

/**
 * @brief iic bus send nack
 * @note  none
 */
static void a_iic_nack(void)
{
    SHT4x_IIC_SCL = 0;
    SHT4x_SDA_OUT();
    SHT4x_IIC_SDA = 1;
    Delayus(2);
    SHT4x_IIC_SCL = 1;
    Delayus(2);
    SHT4x_IIC_SCL = 0;
}

/**
 * @brief     iic send one byte
 * @param[in] txd is the sent byte
 * @note      none
 */
static void a_iic_send_byte(uint8_t txd)
{
    uint8_t t;

    SHT4x_SDA_OUT();
    SHT4x_IIC_SCL = 0;
    for (t = 0; t < 8; t++)
    {
        SHT4x_IIC_SDA = (txd & 0x80) >> 7;
        txd <<= 1;
        Delayus(2);
        SHT4x_IIC_SCL = 1;
        Delayus(2);
        SHT4x_IIC_SCL = 0;
        Delayus(2);
    }
}

/**
 * @brief     iic read one byte
 * @param[in] ack is the sent ack
 * @return    read byte
 * @note      none
 */
static uint8_t a_iic_read_byte(uint8_t ack)
{
    uint8_t i;
    uint8_t receive = 0;

    SHT4x_SDA_IN();
    for (i = 0; i < 8; i++)
    {
        SHT4x_IIC_SCL = 0;
        Delayus(2);
        SHT4x_IIC_SCL = 1;
        receive <<= 1;
        if (SHT4x_READ_SDA != 0)
        {
            receive++;
        }
        Delayus(1);
    }
    if (ack != 0)
    {
        a_iic_ack();
    }
    else
    {
        a_iic_nack();
    }

    return receive;
}

/**
 * @brief  interface iic bus init
 * @return status code
 *         - 0 success
 *         - 1 iic init failed
 * @note   none
 */
uint8_t sht4x_interface_iic_init(void)
{
    GPIO_InitTypeDef GPIO_InitSturcture;

    /* enable iic gpio clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    /* iic gpio init */
    GPIO_InitSturcture.GPIO_Pin = SHT4x_PIN_SCL | SHT4x_PIN_SDA;
    GPIO_InitSturcture.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitSturcture.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SHT4x_PORT, &GPIO_InitSturcture);

    /* set sda high */
    SHT4x_IIC_SDA = 1;

    /* set scl high */
    SHT4x_IIC_SCL = 1;

    return 0;
}

/**
 * @brief  interface iic bus deinit
 * @return status code
 *         - 0 success
 *         - 1 iic deinit failed
 * @note   none
 */
uint8_t sht4x_interface_iic_deinit(void)
{
    return 0;
}

/**
 * @brief     interface iic bus write command
 * @param[in] addr is the iic device write address
 * @param[in] *buf points to a data buffer
 * @param[in] len is the length of the data buffer
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 * @note      none
 */
uint8_t sht4x_interface_iic_write_cmd(uint8_t addr, uint8_t *buf, uint16_t len)
{
    uint16_t i;

    /* send a start */
    a_iic_start();

    /* send the write addr */
    a_iic_send_byte(addr);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* write the data */
    for (i = 0; i < len; i++)
    {
        /* send one byte */
        a_iic_send_byte(buf[i]);
        if (a_iic_wait_ack() != 0)
        {
            a_iic_stop();

            return 1;
        }
    }

    /* send a stop */
    a_iic_stop();

    return 0;
}

/**
 * @brief      interface iic bus read command
 * @param[in]  addr is the iic device write address
 * @param[out] *buf points to a data buffer
 * @param[in]  len is the length of the data buffer
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 * @note       none
 */
uint8_t sht4x_interface_iic_read_cmd(uint8_t addr, uint8_t *buf, uint16_t len)
{
    /* send a start */
    a_iic_start();

    /* send the read addr */
    a_iic_send_byte(addr + 1);
    if (a_iic_wait_ack() != 0)
    {
        a_iic_stop();

        return 1;
    }

    /* read the data */
    while (len != 0)
    {
        /* if the last */
        if (len == 1)
        {
            /* send nack */
            *buf = a_iic_read_byte(0);
        }
        else
        {
            /* send ack */
            *buf = a_iic_read_byte(1);
        }
        len--;
        buf++;
    }

    /* send a stop */
    a_iic_stop();

    return 0;
}

/**
 * @brief     interface delay ms
 * @param[in] ms
 * @note      none
 */
void sht4x_interface_delay_ms(uint32_t ms)
{
    Delayms(ms);
}

/**
 * @brief     interface print format data
 * @param[in] fmt is the format data
 * @note      none
 */
void sht4x_interface_debug_print(const char *const fmt, ...)
{
}
