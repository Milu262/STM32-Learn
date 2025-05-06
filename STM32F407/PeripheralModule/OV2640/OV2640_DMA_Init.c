#include "OV2640_DMA_Init.h"
void DMA_DCMI_Init_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    // 使能DMA时钟
    RCC_AHB1PeriphClockCmd(DCMI_DMA_CLK, ENABLE); // DMA时钟使能
    DMA_DeInit(DCMI_DMA_STREAM);
    while (DMA_GetCmdStatus(DCMI_DMA_STREAM) != DISABLE)
    {
    }
    DMA_InitStructure.DMA_Channel = DCMI_DMA_CHANNEL;           // DMA通道
    DMA_InitStructure.DMA_PeripheralBaseAddr = DCMI_DR_ADDRESS; // 外设基地址

    // DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DCMI_DMA_Rx_Buf;      // 内存基地址

    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)SPI1_DR_BASE; // SPI1 的DR基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;         // 数据传输方向：外设到内存    ！！！一定注意方向！！！

    // DMA_InitStructure.DMA_BufferSize = DCMI_RX_BUF_SIZE;                    // DMA缓冲区大小

    DMA_InitStructure.DMA_BufferSize = 1;                            // DMA缓冲区大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不递增

    // DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                 // 内存地址递增

    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;                // 内存不地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word; // 外设数据宽度：字(32位)
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;     // 内存数据宽度：半字(16位)  能否这样使用？

    // DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                           // DMA模式：正常模式

    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;               // DMA模式：循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;           // DMA优先级：高
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;         // FIFO模式使用
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; // FIFO阈值：满
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC8;     // 内存突发传输：8次
    // DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC8;      //为什么使用这个？
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; // 外设突发传输：单次
    DMA_Init(DCMI_DMA_STREAM, &DMA_InitStructure);
    DMA_Cmd(DCMI_DMA_STREAM, ENABLE);
    while (DMA_GetCmdStatus(DCMI_DMA_STREAM) != ENABLE)
    {
    } // 等待DMA可以被设置
    // DMA_ITConfig(DCMI_DMA_STREAM, DMA_IT_TC, ENABLE);// 打开DCMI的DMA传输完成中断,发生中断说明一行的数据传输完成
}