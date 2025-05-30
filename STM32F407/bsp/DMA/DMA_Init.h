#include "stm32f4xx.h"
#include <stdio.h>
#include "string.h"

#define USART_MAX_LEN 256+32


//定义SPI1的数据地址
#define SPI1_DR_BASE (SPI1_BASE + 0x0C)
//---------------------串口1-------------------------------------------------------------------------
// // DMA
// //  定义调试串口数据寄存器基地址，USART1_BASE是USART1的基地址，0x04是数据寄存器相对于基地址的偏移量
// #define DEBUG_USART_DR_BASE (USART1_BASE + 0x04)

// // 定义调试串口DMA时钟，DMA2是STM32的DMA控制器2，用于管理DMA传输
// #define DEBUG_USART_DMA_CLK RCC_AHB1Periph_DMA2

// // 定义调试串口DMA通道，DMA_Channel_4是DMA控制器中的一个通道，用于特定的DMA传输
// #define DEBUG_USART_DMA_CHANNEL DMA_Channel_4

// // 定义USART1的发送数据使用的DMA流为DMA2的Stream7
// #define DEBUG_USART_TX_DMA_STREAM DMA2_Stream7

// // 定义USART1的接收数据使用的DMA流为DMA2的Stream5
// #define DEBUG_USART_RX_DMA_STREAM DMA2_Stream5



//-------------------串口2---------------------------------------------------------------------------
// DMA
//  定义调试串口数据寄存器基地址，USART1_BASE是USART1的基地址，0x04是数据寄存器相对于基地址的偏移量
#define DEBUG_USART_DR_BASE (USART2_BASE + 0x04)

// 定义调试串口DMA时钟，DMA1是STM32的DMA控制器1，用于管理DMA传输
#define DEBUG_USART_DMA_CLK RCC_AHB1Periph_DMA1

// 定义调试串口DMA通道，DMA_Channel_4是DMA控制器中的一个通道，用于特定的DMA传输
#define DEBUG_USART_DMA_CHANNEL DMA_Channel_4

// 定义USART2的发送数据使用的DMA流为DMA1的Stream6
#define DEBUG_USART_TX_DMA_STREAM DMA1_Stream6

// 定义USART2的接收数据使用的DMA流为DMA1的Stream5
#define DEBUG_USART_RX_DMA_STREAM DMA1_Stream5


extern volatile uint16_t usart1_rx_len;
extern volatile uint16_t usart1_tx_len;
extern uint8_t DMA_USART1_RX_BUF[USART_MAX_LEN];
extern uint8_t DMA_USART1_TX_BUF[USART_MAX_LEN];



/**
 * @brief 初始化DMA（直接存储器访问）配置
 *
 * 该函数用于配置和初始化DMA传输参数，包括DMA时钟使能、DMA流重置、
 * 配置DMA初始化结构体以及启用DMA传输。
 */
void DMA_Uart1_Init_Config(void);


/**
 * @brief  通过UART的DMA方式发送字符串数据.
 * @param  ucstr: 要发送的字符串指针.
 * @param  len: 要发送的数据长度.
 * @retval None
 */
void usart_send_String_DMA(uint8_t *ucstr, uint16_t len);
