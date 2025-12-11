#ifndef MY_YOUR_DRIVER_H
#define MY_YOUR_DRIVER_H

#include "stdint.h"
#include <stdio.h>
#define debug_printf printf
// ====== 【你必须实现的底层驱动接口】======
// 放在 your_driver.c 中实现

// /**
//  * @brief 串口输出单字节（用于发送 HDLC 响应）
//  * @param ch 待发送字节
//  */
// void uart_putc(uint8_t ch);

/**
 * @brief 通过 DMA 发送串口数据
 * @param buf 待发送数据指针
 * @param len 数据长度
 */
void uart_send_buffer_DMA(const uint8_t* buf, uint16_t len);

#ifndef debug_printf
/**
 * @brief 调试信息输出（非 HDLC 数据，如 printf）
 * @param fmt 格式化字符串
 * @param ... 可变参数
 */
void debug_printf(const char* fmt, ...);

#endif


/**
 * @brief Flash 读取（addr 对齐要求由你保证）
 * @param addr 读取地址
 * @param buf 存储读取数据指针
 * @param len 读取数据长度
 */
void flash_read(uint32_t addr, uint8_t* buf, uint16_t len);

/**
 * @brief I2C 读寄存器（7-bit 地址）
 * @param dev_addr 设备地址
 * @param reg_addr 寄存器地址
 * @param value 存储读取值指针
 * @return int 0 成功，-1 失败
 */
int i2c_read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t* value);

/**
 * @brief I2C 读寄存器（16-bit 地址）
 * @param dev_addr 设备地址
 * @param reg_addr 寄存器地址
 * @param value 存储读取值指针
 * @return int 0 成功，-1 失败
 */
int i2c_read_reg_16(uint8_t dev_addr, uint16_t reg_addr, uint8_t *value);

/**
 * @brief I2C 写寄存器（7-bit 地址）
 * @param dev_addr 设备地址
 * @param reg_addr 寄存器地址
 * @param value 待写入值
 * @return int 0 成功，-1 失败
 */
int i2c_write_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t value);

/**
 * @brief I2C 写寄存器（16-bit 地址）
 * @param dev_addr 设备地址
 * @param reg_addr 寄存器地址
 * @param value 待写入值
 * @return int 0 成功，-1 失败
 */
int i2c_write_reg_16(uint8_t dev_addr, uint16_t reg_addr, uint8_t value);

/**
 * @brief SPI 读寄存器（CS 自动控制）
 * @param reg_addr 寄存器地址
 * @param value 存储读取值指针
 * @return int 0 成功，-1 失败
 */
int spi_read_reg(uint8_t reg_addr, uint8_t* value);

/**
 * @brief SPI 写寄存器（CS 自动控制）
 * @param reg_addr 寄存器地址
 * @param value 待写入值
 * @return int 0 成功，-1 失败
 */
int spi_write_reg(uint8_t reg_addr, uint8_t value);

#endif