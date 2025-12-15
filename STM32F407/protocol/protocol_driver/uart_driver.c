#include "uart_driver.h"
#include "DMA_Init.h"
#include <string.h>

void uart_send_buffer_DMA(const uint8_t *buf, uint16_t len)
{
    // 安全检查 1: 禁止空指针（除非 len == 0）
    if (buf == NULL && len > 0)
    {
        return; // 无效调用，静默忽略或可加 assert
    }

    // 安全检查 2: 长度为 0 直接返回（合法但无需操作）
    if (len == 0)
    {
        return;
    }

    // 安全检查 3: 防止超出 DMA 最大传输限制（虽然 uint16_t 最大 65535，但显式约束更清晰）
    if (len > USART_MAX_LEN)
    {
        return; // 或截断：len = UART_DMA_MAX_LEN;（不推荐，可能丢数据）
    }

    // 调用底层 DMA 发送函数（安全转换 const）
    usart_send_String_DMA((uint8_t *)buf, len);
}

// 调试信息输出（非 HDLC 数据，如 printf）

#ifndef debug_printf
void debug_printf(const char *fmt, ...)
{
    // 在此实现你的调试输出代码（如通过串口打印）
}
#endif