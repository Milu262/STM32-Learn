#include "stm32f4xx.h"

// #define USART_IRQ USART1_IRQn              // 定义USART中断处理函数为USART1中断
// #define USART_DMA_RX_IRQ DMA2_Stream5_IRQn // 定义USART接收DMA中断处理函数为DMA2流5中断
// #define USART_DMA_TX_IRQ DMA2_Stream7_IRQn // 定义USART发送DMA中断处理函数为DMA2流7中断

#define USART_IRQ USART2_IRQn              // 定义USART中断处理函数为USART2中断
#define USART_DMA_RX_IRQ DMA1_Stream5_IRQn // 定义USART接收DMA中断处理函数为DMA2流5中断
#define USART_DMA_TX_IRQ DMA1_Stream6_IRQn // 定义USART发送DMA中断处理函数为DMA2流7中断

#define DCMI_DMA_IRQ DMA2_Stream1_IRQn     // 定义DCMI接收DMA中断处理函数为DMA2流1中断
#define DCMI_DCMI_IRQ DCMI_IRQn                 // 定义DCMI中断处理函数为DCMI中断

/**
 * @brief 配置中断控制器（NVIC）的优先级和使能状态。
 *
 * 该函数主要用于配置USART1（串口1）和DMA2（直接内存访问控制器2）的相关中断。
 * 具体配置包括设置中断优先级和使能中断。
 */
void NVIC_Configuration(void);