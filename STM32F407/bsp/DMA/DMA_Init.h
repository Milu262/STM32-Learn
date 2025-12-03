#include "stm32f4xx.h"
// #include <stdio.h>
// #include <string.h>

/**
 * 最坏情况：payload 中全是 0x7E 或 0x7D，每个字节变成 2 字节（转义），256 字节
→ 512 字节！

但通常不会这么极端。若你控制 PC 端不发送大量转义字符，300 足够。

若需绝对安全，可设为 520（256×2 + 8），但占用 RAM 多。
 */
#define USART_MAX_LEN 520

// 定义SPI1的数据地址
#define SPI1_DR_BASE (SPI1_BASE + 0x0C)
//---------------------串口1-------------------------------------------------------------------------
// // DMA
// //
// 定义调试串口数据寄存器基地址，USART1_BASE是USART1的基地址，0x04是数据寄存器相对于基地址的偏移量
// #define DEBUG_USART_DR_BASE (USART1_BASE + 0x04)

// // 定义调试串口DMA时钟，DMA2是STM32的DMA控制器2，用于管理DMA传输
// #define DEBUG_USART_DMA_CLK RCC_AHB1Periph_DMA2

// //
// 定义调试串口DMA通道，DMA_Channel_4是DMA控制器中的一个通道，用于特定的DMA传输
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
void DMA_Uart_Init_Config(void);

/**
 * @brief  通过 UART 的 DMA 方式发送一段数据。
 *         该函数将数据拷贝到内部 DMA 发送缓冲区，并启动 DMA 传输。
 *         若 DMA 正在忙，会等待其释放（带超时防止死锁）。
 *         为避免缓冲区溢出，发送长度不会超过内部缓冲区大小。
 *
 * @param  ucstr: 指向要发送数据的指针（不可为 NULL）。
 * @param  len:   要发送的数据字节数（若为 0 则直接返回失败）。
 *
 * @retval >0:    成功发送的字节数（≤ 内部缓冲区大小）。
 * @retval -1:    参数错误（ucstr 为 NULL 或 len 为 0）。
 * @retval -2:    DMA 超时（上一次传输未在限定时间内完成）。
 *
 * @note   本函数为阻塞式调用，直到 DMA 启动成功或超时。
 *         不应在中断服务程序（ISR）中调用，除非确保无重入风险。
 */
int usart_send_String_DMA(const uint8_t *ucstr, uint16_t len);
