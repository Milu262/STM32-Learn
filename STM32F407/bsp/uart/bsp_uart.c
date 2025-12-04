#include "bsp_uart.h"
#include <stdio.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <unistd.h>
void uart_init(uint32_t __Baud) {
  USART_DeInit(BSP_USART); // 复位串口
  GPIO_InitTypeDef GPIO_InitStructure;

  // RCC_APB2PeriphClockCmd(BSP_USART_RCC, ENABLE);//串口1
  RCC_APB1PeriphClockCmd(BSP_USART_RCC, ENABLE); // 串口2

  RCC_AHB1PeriphClockCmd(BSP_USART_TX_RCC, ENABLE);
  RCC_AHB1PeriphClockCmd(BSP_USART_RX_RCC, ENABLE);

  GPIO_PinAFConfig(BSP_USART_TX_PORT, BSP_USART_TX_AF_PIN, BSP_USART_AF);
  GPIO_PinAFConfig(BSP_USART_RX_PORT, BSP_USART_RX_AF_PIN, BSP_USART_AF);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = BSP_USART_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(BSP_USART_TX_PORT, &GPIO_InitStructure);

  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = BSP_USART_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(BSP_USART_RX_PORT, &GPIO_InitStructure);

  USART_InitTypeDef USART_InitStructure;

  // USART_DeInit(BSP_USART);
  // USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = __Baud;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_HardwareFlowControl =
      USART_HardwareFlowControl_None;
  USART_Init(BSP_USART, &USART_InitStructure);
  // USART_ClearFlag(BSP_USART, USART_FLAG_RXNE);
  // USART_ITConfig(BSP_USART, USART_IT_RXNE, ENABLE); // 使能接收中断
  USART_ITConfig(BSP_USART, USART_IT_IDLE, ENABLE); // 使能空闲中断

  USART_DMACmd(BSP_USART, USART_DMAReq_Tx, ENABLE); // 使能串口1的DMA发送
  USART_DMACmd(BSP_USART, USART_DMAReq_Rx, ENABLE); // 使能串口1的DMA接收

  // DMA_Uart1_Init_Config(); // DMA初始化
  USART_Cmd(BSP_USART, ENABLE); // 使能串口1
}

void usart_send_data(uint8_t ucch) {
  USART_SendData(BSP_USART, (uint8_t)ucch);

  while (RESET == USART_GetFlagStatus(BSP_USART, USART_FLAG_TXE)) {
  }
}

void usart_send_String(uint8_t *ucstr) {
  while (ucstr && *ucstr) {
    usart_send_data(*ucstr++);
  }
}

// #if !defined(__MICROLIB)

// #if (__ARMCLIB_VERSION <= 6000000)

// struct __FILE {
//   int handle;
// };
// #endif

// FILE __stdout;

// void _sys_exit(int x) { x = x; }
// #endif

/* retarget the C library printf function to the USART */
// int fputc(int ch, FILE *f) {
//   // USART_SendData(BSP_USART, (uint8_t)ch);
//   // while (RESET == USART_GetFlagStatus(BSP_USART, USART_FLAG_TXE))
//   // {
//   // }

//   // 以下为使用DMA传输
//   DMA_USART1_TX_BUF[0] = (uint8_t)ch;
//   while (DMA_GetCmdStatus(DEBUG_USART_TX_DMA_STREAM) !=
//          DISABLE) // 当DMA的命令为DISABLE时，跳出循环
//     ;             // 等待DMA可以被设置
//                   // 设置DMA传输模式

//   DMA_SetCurrDataCounter(DEBUG_USART_TX_DMA_STREAM,
//                          1); // 设置当前DMA的传输数据量

//   DMA_Cmd(DEBUG_USART_TX_DMA_STREAM, ENABLE); // 使能DMA传输

//   return ch;
// }

// int fgetc(FILE *f) {

//   while (USART_GetFlagStatus(BSP_USART, USART_FLAG_RXNE) == RESET)
//     ;

//   return (int)USART_ReceiveData(BSP_USART);
// }

// 用于 DMA 发送单字节（复用你的逻辑）

// 在linux的环境下，使用GCC + newlib 编译时，printf 调用的是 _write()，而不是
// fputc

// 该函数是用于判断 stdout 是否是终端（用于决定是否自动 flush）（非必须实现）
int _isatty(int fd) {
  return (fd == STDIN_FILENO || fd == STDOUT_FILENO || fd == STDERR_FILENO) ? 1
                                                                            : 0;
}

// newlib 重定向：printf -> usart_send_String_DMA
int _write(int fd, const char *ptr, int len) {
  if (fd == STDOUT_FILENO || fd == STDERR_FILENO) {
    // 先发送固定测试数据，绕过 ptr
    // static const uint8_t test[] = "OK\n";
    // usart_send_String_DMA(test, 3);
    usart_send_String_DMA((const uint8_t *)ptr, len);
    while (DMA_GetCmdStatus(DEBUG_USART_TX_DMA_STREAM) != DISABLE)
      ; // 等待完成
    return len;
  }
  errno = EBADF;
  return -1;
}