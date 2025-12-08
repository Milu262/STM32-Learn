#include "DMA_Init.h"
#include <string.h>
volatile uint16_t usart1_rx_len = 0; // 接收数据长度
__attribute__((aligned(4))) static uint8_t DMA_USART_RX_BUF[USART_MAX_LEN] = {
    0}; // DMA接收数据缓冲区
__attribute__((aligned(4))) static uint8_t DMA_USART_TX_BUF[USART_MAX_LEN] = {
    0}; // DMA发送数据缓冲区

// uint32_t DCMI_DMA_Rx_Buf[DCMI_RX_BUF_SIZE] = {0}; // DCMI接收数据缓冲区
void DMA_Uart_Init_Config(void) {
  DMA_InitTypeDef DMA_InitStructure;
  // 使能DMA时钟
  RCC_AHB1PeriphClockCmd(DEBUG_USART_DMA_CLK, ENABLE); // DMA时钟使能
  DMA_DeInit(DEBUG_USART_RX_DMA_STREAM);
  while (DMA_GetCmdStatus(DEBUG_USART_RX_DMA_STREAM) != DISABLE) {
  }
  DMA_InitStructure.DMA_Channel = DEBUG_USART_DMA_CHANNEL; // DMA通道
  DMA_InitStructure.DMA_PeripheralBaseAddr =
      (uint32_t)DEBUG_USART_DR_BASE; // 外设基地址
  DMA_InitStructure.DMA_Memory0BaseAddr =
      (uint32_t)DMA_USART_RX_BUF; // 内存基地址
  DMA_InitStructure.DMA_DIR =
      DMA_DIR_PeripheralToMemory; // 数据传输方向：外设到内存
                                  // ！！！一定注意方向！！！
  DMA_InitStructure.DMA_BufferSize = USART_MAX_LEN; // DMA缓冲区大小
  DMA_InitStructure.DMA_PeripheralInc =
      DMA_PeripheralInc_Disable; // 外设地址不递增
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址递增
  DMA_InitStructure.DMA_PeripheralDataSize =
      DMA_PeripheralDataSize_Byte; // 外设数据宽度：字节
  DMA_InitStructure.DMA_MemoryDataSize =
      DMA_MemoryDataSize_Byte;                  // 内存数据宽度：字节
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; // DMA模式：正常模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_Low; // DMA优先级：低等
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; // FIFO模式禁用
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; // FIFO阈值：满
  DMA_InitStructure.DMA_MemoryBurst =
      DMA_MemoryBurst_Single; // 内存突发传输：单次
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DEBUG_USART_RX_DMA_STREAM, &DMA_InitStructure);
  DMA_Cmd(DEBUG_USART_RX_DMA_STREAM, ENABLE);

  DMA_DeInit(DEBUG_USART_TX_DMA_STREAM);
  // 配置DMA初始化结构体

  DMA_InitStructure.DMA_Channel = DEBUG_USART_DMA_CHANNEL; // DMA通道
  DMA_InitStructure.DMA_PeripheralBaseAddr =
      (uint32_t)DEBUG_USART_DR_BASE; // 外设基地址
  DMA_InitStructure.DMA_Memory0BaseAddr =
      (uint32_t)DMA_USART_TX_BUF; // 内存基地址
  DMA_InitStructure.DMA_DIR =
      DMA_DIR_MemoryToPeripheral; // 数据传输方向：内存到外设
                                  // ！！！一定注意方向！！！
  DMA_InitStructure.DMA_BufferSize = USART_MAX_LEN; // DMA缓冲区大小
  DMA_InitStructure.DMA_PeripheralInc =
      DMA_PeripheralInc_Disable; // 外设地址不递增
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址递增
  DMA_InitStructure.DMA_PeripheralDataSize =
      DMA_PeripheralDataSize_Byte; // 外设数据宽度：字节
  DMA_InitStructure.DMA_MemoryDataSize =
      DMA_MemoryDataSize_Byte;                  // 内存数据宽度：字节
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; // DMA模式：正常模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; // DMA优先级：中等
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable; // FIFO模式禁用
  DMA_InitStructure.DMA_FIFOThreshold =
      DMA_FIFOThreshold_1QuarterFull; // FIFO阈值：满
  DMA_InitStructure.DMA_MemoryBurst =
      DMA_MemoryBurst_Single; // 内存突发传输：单次
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(DEBUG_USART_TX_DMA_STREAM, &DMA_InitStructure);
  // 禁用DEBUG_USART_DMA_STREAM的DMA传输
  DMA_Cmd(DEBUG_USART_TX_DMA_STREAM, DISABLE);
  DMA_ITConfig(DEBUG_USART_TX_DMA_STREAM, DMA_IT_TC,
               ENABLE); // 使能uart发送DMA传输完成中断
}

int uart_copy_receive_data(uint8_t *buf, uint16_t buf_size) {
  if (buf == NULL || buf_size == 0)
    return -1;

  uint16_t len = sizeof(DMA_USART_RX_BUF);
  if (len > buf_size) {
    len = buf_size;
  }

  memcpy(buf, DMA_USART_RX_BUF, len);
  //   usart1_rx_len = 0; // 清空接收数据长度
  return (int)len;
}

int usart_send_String_DMA(const uint8_t *ucstr, uint16_t len) {
  if (ucstr == NULL || len == 0)
    return -1;

  if (len > sizeof(DMA_USART_TX_BUF)) {
    len = sizeof(DMA_USART_TX_BUF);
  }

  memcpy(DMA_USART_TX_BUF, ucstr, len);

  // 超时等待 DMA 释放
  uint32_t timeout = SystemCoreClock / 100;
  while (DMA_GetCmdStatus(DEBUG_USART_TX_DMA_STREAM) != DISABLE) {
    if (--timeout == 0) {
      return -2; // 超时
    }
  }

  // 关键：重载内存地址（即使相同，也显式设置）
  // DMA_MemoryTargetConfig(DEBUG_USART_TX_DMA_STREAM, (uint32_t)DMA_USART_TX_BUF,
  //                        DMA_Memory_0);

  DMA_SetCurrDataCounter(DEBUG_USART_TX_DMA_STREAM, len);

  DMA_Cmd(DEBUG_USART_TX_DMA_STREAM, ENABLE);

  return (int)len;
}