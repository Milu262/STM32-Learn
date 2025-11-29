#ifndef CRC16_H
#define CRC16_H

#include <stdint.h>

/**
 * @brief 计算 CRC-16/CCITT 校验值（初始值 0xFFFF，多项式 0x1021）
 * @param data: 输入数据指针
 * @param len: 数据长度（字节）
 * @return 16 位 CRC 值（大端序，可直接用于 HDLC 帧）
 */
uint16_t crc16_ccitt(const uint8_t* data, uint16_t len);

#endif