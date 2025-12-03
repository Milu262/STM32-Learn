// #include <stdint.h>
#include "MY_your_driver.h"
#include "../bsp/DMA/DMA_Init.h"
#include "../bsp/I2C/i2c_handle.h"
#include "../bsp/SPI/SPI_Control.h"
#include <stdarg.h>
#include <stddef.h>
// ====== 【你必须实现的底层驱动接口】======
// 放在 your_driver.c 中实现

// // 串口输出单字节（用于发送 HDLC 响应）
// void uart_putc(uint8_t ch)
// {
//     // 在此实现你的串口发送代码
// }

void uart_send_buffer_DMA(const uint8_t *buf, uint16_t len) {
  if (buf == NULL && len > 0)
    return; // 无效调用，静默忽略或可加 assert

  if (len == 0)
    return;

  if (len > USART_MAX_LEN)
    return; // 或截断：len = UART_DMA_MAX_LEN;（不推荐，可能丢数据）

  // 调用底层 DMA 发送函数（安全转换 const）
  usart_send_String_DMA(buf, len);
}

// 调试信息输出（非 HDLC 数据，如 printf）
void debug_printf(const char *fmt, ...) {
  // 在此实现你的调试输出代码（如通过串口打印）
}

int flash_read(uint32_t addr, uint8_t *buf, uint16_t len) {
  if (buf == NULL)
    return -1;
  if (len == 0)
    return -1;
  SPI_FLASH_BufferRead(buf, addr, len);
  return 0;
}

int i2c_read_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t *value) {
  if (value == NULL)
    return -1; // 无效指针
  uint32_t result = I2C_byteRead(dev_addr, reg_addr, value);
  return (result == 1) ? 0 : -1;
}

int i2c_write_reg(uint8_t dev_addr, uint8_t reg_addr, uint8_t value) {
  uint32_t result = I2C_ByteWrite(dev_addr, reg_addr, value);
  return (result == 1) ? 0 : -1;
}

int spi_read_reg(uint8_t reg_addr, uint8_t *value) {
  if (value == NULL) {
    return -1;
  }

  uint8_t dummy;
  uint8_t received;

  SPI_CS_ON(0);

  if (SPI_Send_receive_Byte(reg_addr, &dummy) != 1) {
    SPI_CS_ON(1);
    return -1;
  }

  if (SPI_Send_receive_Byte(0x00, &received) != 1) {
    SPI_CS_ON(1);
    return -1;
  }

  SPI_CS_ON(1);

  *value = received;
  return 0;
}

int spi_write_reg(uint8_t reg_addr, uint8_t value) {
  uint8_t dummy;

  SPI_CS_ON(0);

  // Step 1: 发送寄存器地址（写操作通常不加标志位）
  if (SPI_Send_receive_Byte(reg_addr, &dummy) != 1) {
    SPI_CS_ON(1);
    return -1;
  }

  // Step 2: 发送要写入的数据
  if (SPI_Send_receive_Byte(value, &dummy) != 1) {
    SPI_CS_ON(1);
    return -1;
  }

  SPI_CS_ON(1);
  return 0;
}