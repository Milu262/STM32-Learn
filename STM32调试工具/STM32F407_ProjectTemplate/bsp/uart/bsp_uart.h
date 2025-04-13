

#ifndef __BSP_UART_H__
#define __BSP_UART_H__
// #include "DMA_Init.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "NVIC_Init.h"
#include "DMA_Init.h"
// #include "response.h"

#define BSP_USART_RCC RCC_APB2Periph_USART1
#define BSP_USART_TX_RCC RCC_AHB1Periph_GPIOA
#define BSP_USART_RX_RCC RCC_AHB1Periph_GPIOA

#define BSP_USART USART1
#define BSP_USART_TX_PORT GPIOA
#define BSP_USART_TX_PIN GPIO_Pin_9
#define BSP_USART_RX_PORT GPIOA
#define BSP_USART_RX_PIN GPIO_Pin_10
#define BSP_USART_AF GPIO_AF_USART1
#define BSP_USART_TX_AF_PIN GPIO_PinSource9
#define BSP_USART_RX_AF_PIN GPIO_PinSource10




//  extern uint8_t DMA_Uart_SendBuff[SENDBUFF_SIZE];	// 定义发送缓冲区，用于存储要发送的数据;//发送缓冲区

/**
 * @brief  Initializes the UART1 peripheral with the specified baud rate.
 * @param  __Baud: The baud rate to be set for UART1.
 * @retval None
 */
void uart1_init(uint32_t __Baud);
/**
 * @brief  Sends a single data byte through UART.
 * @param  ucch: The data byte to be sent.
 * @retval None
 */
void usart_send_data(uint8_t ucch);
/**
 * @brief  通过UART发送字符串数据.
 * @param  ucstr: 要发送的字符串指针.
 * @retval None
 */
void usart_send_String(uint8_t *ucstr);

#endif
