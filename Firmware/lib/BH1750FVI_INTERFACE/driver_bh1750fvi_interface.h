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
 * @file      driver_bh1750fvi_interface.h
 * @brief     driver bh1750fvi interface header file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2022-11-30
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2022/11/30  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#ifndef DRIVER_BH1750FVI_INTERFACE_H
#define DRIVER_BH1750FVI_INTERFACE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "driver_bh1750fvi.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#define BH1750FVI_PORT GPIOB          /**< gpio port for bh1750fvi */
#define BH1750FVI_SCL_PIN GPIO_Pin_10 /**< gpio pin for scl */
#define BH1750FVI_SDA_PIN GPIO_Pin_11 /**< gpio pin for sda */

/**
 * @brief bit operate definition
 */
#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr) *((uint32_t *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

/**
 * @brief iic gpio operate definition
 */
#define GPIOB_ODR_Addr (GPIOB_BASE + 0x0C)
#define GPIOB_IDR_Addr (GPIOB_BASE + 0x08)

#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n)
#define PBin(n) BIT_ADDR(GPIOB_IDR_Addr, n)

#define BH1750FVI_SDA_IN()                         \
    {                                              \
        BH1750FVI_PORT->CRH &= ~(0x0F << (3 * 4)); \
        BH1750FVI_PORT->CRH |= 0x08 << (3 * 4);    \
    }

#define BH1750FVI_SDA_OUT()                        \
    {                                              \
        BH1750FVI_PORT->CRH &= ~(0x0F << (3 * 4)); \
        BH1750FVI_PORT->CRH |= 0x03 << (3 * 4);    \
    }

#define BH1750FVI_IIC_SCL PBout(10)
#define BH1750FVI_IIC_SDA PBout(11)
#define BH1750FVI_READ_SDA PBin(11)

    /**
     * @defgroup bh1750fvi_interface_driver bh1750fvi interface driver function
     * @brief    bh1750fvi interface driver modules
     * @ingroup  bh1750fvi_driver
     * @{
     */

    /**
     * @brief  interface iic bus init
     * @return status code
     *         - 0 success
     *         - 1 iic init failed
     * @note   none
     */
    uint8_t bh1750fvi_interface_iic_init(void);

    /**
     * @brief  interface iic bus deinit
     * @return status code
     *         - 0 success
     *         - 1 iic deinit failed
     * @note   none
     */
    uint8_t bh1750fvi_interface_iic_deinit(void);

    /**
     * @brief     interface iic bus write command
     * @param[in] addr is the iic device write address
     * @param[in] *buf points to a data buffer
     * @param[in] len is the length of data buffer
     * @return    status code
     *            - 0 success
     *            - 1 write failed
     * @note      none
     */
    uint8_t bh1750fvi_interface_iic_write_cmd(uint8_t addr, uint8_t *buf, uint16_t len);

    /**
     * @brief      interface iic bus read command
     * @param[in]  addr is the iic device write address
     * @param[out] *buf points to a data buffer
     * @param[in]  len is the length of data buffer
     * @return     status code
     *             - 0 success
     *             - 1 read failed
     * @note       none
     */
    uint8_t bh1750fvi_interface_iic_read_cmd(uint8_t addr, uint8_t *buf, uint16_t len);

    /**
     * @brief     interface delay ms
     * @param[in] ms
     * @note      none
     */
    void bh1750fvi_interface_delay_ms(uint32_t ms);

    /**
     * @brief     interface print format data
     * @param[in] fmt is the format data
     * @note      none
     */
    void bh1750fvi_interface_debug_print(const char *const fmt, ...);

    /**
     * @}
     */

#ifdef __cplusplus
}
#endif

#endif
