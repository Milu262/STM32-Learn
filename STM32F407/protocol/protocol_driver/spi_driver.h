#ifndef __SPI_DRIVER_H__
#define __SPI_DRIVER_H__

#include <stdint.h>
/**
 * @brief Flash 读取（addr 对齐要求由你保证）
 * @param addr 读取地址
 * @param buf 存储读取数据指针
 * @param len 读取数据长度
 */
void flash_read(uint32_t addr, uint8_t* buf, uint16_t len);

/**
 * @brief Flash 写（addr 对齐要求由你保证）
 * @param addr 写入地址
 * @param buf 待写入数据指针
 * @param len 写入数据长度
 */
void flash_write(uint32_t addr, uint8_t *buf, uint16_t len);

/**
 * @brief Flash 扇区擦除（addr 对齐要求由你保证）
 * @param addr 擦除起始地址
 */
void flash_Sector_erase(uint32_t addr);

/**
 * @brief Flash 块(32K)擦除（addr 对齐要求由你保证）
 * @param addr 擦除起始地址
 */
void flash_Block_erase32(uint32_t addr);

/**
 * @brief Flash 块(64K)擦除（addr 对齐要求由你保证）
 * @param addr 擦除起始地址
 */
void flash_Block_erase64(uint32_t addr);

/**
 * @brief Flash 芯片擦除
 */
void flash_Chip_erase(void);

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