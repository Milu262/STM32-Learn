#include "stm32f4xx.h"

//-------------------DCMI---------------------------------------------------------------------------

// 定义DCMI使用的DMA时钟，DMA2是STM32的DMA控制器2，用于管理DMA传输
#define DCMI_DMA_CLK RCC_AHB1Periph_DMA2

// 定义DCMI使用的DMA通道，DMA_Channel_1是DMA控制器中的一个通道，用于特定的DMA传输
#define DCMI_DMA_CHANNEL DMA_Channel_1

// 定义DCMI使用的DMA流，DMA2_Stream1是DMA控制器中的一个流，用于特定的DMA传输
#define DCMI_DMA_STREAM DMA2_Stream1

// DCMI数据寄存器地址
#define DCMI_DR_ADDRESS (uint32_t)&DCMI->DR 

//定义SPI1的数据地址
#define SPI1_DR_BASE (SPI1_BASE + 0x0C)

void DMA_DCMI_Init_Config(void);