#ifndef __I2C_DRIVER_H__
#define __I2C_DRIVER_H__ 
#include <stdint.h>

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
#endif