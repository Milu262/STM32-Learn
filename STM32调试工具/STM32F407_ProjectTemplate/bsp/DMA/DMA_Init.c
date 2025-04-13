#include "DMA_Init.h"

volatile uint16_t usart1_rx_len = 0;            // 接收数据长度
volatile uint16_t usart1_tx_len = 0;            // 发送数据长度
uint8_t DMA_USART1_RX_BUF[USART_MAX_LEN] = {0}; // DMA接收数据缓冲区
uint8_t DMA_USART1_TX_BUF[USART_MAX_LEN] = {0}; // DMA发送数据缓冲区

uint8_t DMA_Uart_SendBuff[SENDBUFF_SIZE]; // 定义发送缓冲区，用于存储要发送的数据
void DMA_Uart1_Init_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    // 使能DMA时钟
    RCC_AHB1PeriphClockCmd(DEBUG_USART_DMA_CLK, ENABLE); // DMA时钟使能
    DMA_DeInit(DEBUG_USART1_RX_DMA_STREAM);
    while (DMA_GetCmdStatus(DEBUG_USART1_RX_DMA_STREAM) != DISABLE)
    {
    }
    DMA_InitStructure.DMA_Channel = DEBUG_USART1_DMA_CHANNEL;               // DMA通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;         // 外设基地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DMA_USART1_RX_BUF;    // 内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                 // 数据传输方向：外设到内存
    DMA_InitStructure.DMA_BufferSize = USART_MAX_LEN;                       // DMA缓冲区大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度：字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存数据宽度：字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // DMA模式：正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;                   // DMA优先级：高等
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  // FIFO模式禁用
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;           // FIFO阈值：满
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             // 内存突发传输：单次
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DEBUG_USART1_RX_DMA_STREAM, &DMA_InitStructure);
    DMA_Cmd(DEBUG_USART1_RX_DMA_STREAM, ENABLE);

    DMA_DeInit(DEBUG_USART1_TX_DMA_STREAM);
    // 配置DMA初始化结构体

    DMA_InitStructure.DMA_Channel = DEBUG_USART1_DMA_CHANNEL;               // DMA通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;         // 外设基地址
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DMA_USART1_TX_BUF;    // 内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;                 // 数据传输方向：内存到外设
    DMA_InitStructure.DMA_BufferSize = USART_MAX_LEN;                       // DMA缓冲区大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;        // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度：字节
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;         // 内存数据宽度：字节
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // DMA模式：正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;                   // DMA优先级：中等
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                  // FIFO模式禁用
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;           // FIFO阈值：满
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;             // 内存突发传输：单次
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DEBUG_USART1_TX_DMA_STREAM, &DMA_InitStructure);
    // 禁用DEBUG_USART_DMA_STREAM的DMA传输
    DMA_Cmd(DEBUG_USART1_TX_DMA_STREAM, DISABLE);
    DMA_ITConfig(DEBUG_USART1_TX_DMA_STREAM, DMA_IT_TC, ENABLE); // 使能uart发送DMA传输完成中断
}

void usart_send_String_DMA(uint8_t *ucstr, uint16_t len)
{
    memcpy(DMA_USART1_TX_BUF, ucstr, len); // 将数据拷贝到DMA发送缓冲区
    while (DMA_GetCmdStatus(DEBUG_USART1_TX_DMA_STREAM) != DISABLE)
        ;                                                    // 等待DMA可以被设置
    DMA_SetCurrDataCounter(DEBUG_USART1_TX_DMA_STREAM, len); // 设置当前DMA的传输数据量

    DMA_Cmd(DEBUG_USART1_TX_DMA_STREAM, ENABLE); // 使能DMA传输
}