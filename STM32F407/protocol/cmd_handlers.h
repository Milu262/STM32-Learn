#ifndef CMD_HANDLERS_H
#define CMD_HANDLERS_H

#include <stdint.h>

// 命令处理函数类型（与 hdlc_core 共享）
typedef void (*cmd_handler_t)(const uint8_t* payload, uint16_t payload_len);

// 声明各个命令处理函数（供 hdlc_core.c 使用）

/**
 * @brief 处理 FLASH 读取命令
 * @param payload: 命令负载指针
 * @param len: 负载长度（字节）
 */
void handle_flash_read(const uint8_t* payload, uint16_t len);
/**
 * @brief 处理 I2C 读取寄存器命令
 * @param payload: 命令负载指针
 * @param len: 负载长度（字节）
 */
void handle_i2c_read_reg(const uint8_t* payload, uint16_t len);

/**
 * @brief 处理 I2C 读取16位寄存器命令
 * @param payload: 命令负载指针
 * @param len: 负载长度（字节）
 */
void handle_i2c_read_reg_16(const uint8_t *payload, uint16_t len);
/**
 * @brief 处理 I2C 写入寄存器命令
 * @param payload: 命令负载指针
 * @param len: 负载长度（字节）
 */
void handle_i2c_write_reg(const uint8_t* payload, uint16_t len);
/**
 * @brief 处理 SPI 读取寄存器命令
 * @param payload: 命令负载指针
 * @param len: 负载长度（字节）
 */
void handle_spi_read_reg(const uint8_t* payload, uint16_t len);
/**
 * @brief 处理 SPI 写入寄存器命令
 * @param payload: 命令负载指针
 * @param len: 负载长度（字节）
 */
void handle_spi_write_reg(const uint8_t* payload, uint16_t len);

#endif // CMD_HANDLERS_H