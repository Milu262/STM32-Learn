#ifndef __UART_DRIVER_H__
#define __UART_DRIVER_H__
#include <stdint.h>
#include <stdio.h>
#define debug_printf printf

/**
 * @brief 通过 DMA 发送串口数据
 * @param buf 待发送数据指针
 * @param len 数据长度
 */
void uart_send_buffer_DMA(const uint8_t *buf, uint16_t len);

#ifndef debug_printf
/**
 * @brief 调试信息输出（非 HDLC 数据，如 printf）
 * @param fmt 格式化字符串
 * @param ... 可变参数
 */
void debug_printf(const char *fmt, ...);

#endif
#endif