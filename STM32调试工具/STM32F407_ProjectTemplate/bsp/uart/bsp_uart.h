

#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "stm32f4xx.h"

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

void uart1_init(uint32_t __Baud);
void usart_send_data(uint8_t ucch);
void usart_send_String(uint8_t *ucstr);
static void NVIC_Configuration(void);

#endif
