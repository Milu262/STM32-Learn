

#include "bsp_uart.h"
// #include "response.h"
void uart1_init(uint32_t __Baud)
{
	USART_DeInit(BSP_USART); //复位串口1
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(BSP_USART_TX_RCC, ENABLE);
	RCC_APB2PeriphClockCmd(BSP_USART_RCC, ENABLE);
	GPIO_PinAFConfig(BSP_USART_TX_PORT, BSP_USART_TX_AF_PIN, BSP_USART_AF);
	GPIO_PinAFConfig(BSP_USART_RX_PORT, BSP_USART_RX_AF_PIN, BSP_USART_AF);
	// GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = BSP_USART_TX_PIN | BSP_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	USART_InitTypeDef USART_InitStructure;
	// USART_DeInit(BSP_USART);
	// USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = __Baud;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Init(BSP_USART, &USART_InitStructure);
	// USART_ClearFlag(BSP_USART, USART_FLAG_RXNE);
	// USART_ITConfig(BSP_USART, USART_IT_RXNE, ENABLE); // 使能接收中断
	USART_ITConfig(BSP_USART, USART_IT_IDLE, ENABLE); // 使能空闲中断

	USART_DMACmd(BSP_USART, USART_DMAReq_Tx, ENABLE); // 使能串口1的DMA发送
	USART_DMACmd(BSP_USART, USART_DMAReq_Rx, ENABLE); // 使能串口1的DMA接收

	// DMA_Uart1_Init_Config(); // DMA初始化
	USART_Cmd(BSP_USART, ENABLE);					  // 使能串口1
}

void usart_send_data(uint8_t ucch)
{
	USART_SendData(BSP_USART, (uint8_t)ucch);

	while (RESET == USART_GetFlagStatus(BSP_USART, USART_FLAG_TXE))
	{
	}
}

void usart_send_String(uint8_t *ucstr)
{
	while (ucstr && *ucstr)
	{
		usart_send_data(*ucstr++);
	}
}



#if !defined(__MICROLIB)

#if (__ARMCLIB_VERSION <= 6000000)

struct __FILE
{
	int handle;
};
#endif

FILE __stdout;

void _sys_exit(int x)
{
	x = x;
}
#endif

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
	USART_SendData(BSP_USART, (uint8_t)ch);

	while (RESET == USART_GetFlagStatus(BSP_USART, USART_FLAG_TXE))
	{
	}

	// 以下为使用DMA传输

	// DMA_Uart_SendBuff[0] = (uint8_t)ch;
	// USART_DMACmd(BSP_USART, USART_DMAReq_Tx, ENABLE);
	// //检查执行是否完成
	// while (DMA_GetFlagStatus(DEBUG_USART_DMA_STREAM, DMA_FLAG_TCIF7) == RESET)
	// 	;
	// //清除传输完成标志
	// DMA_ClearFlag(DEBUG_USART_DMA_STREAM, DMA_FLAG_TCIF7);
	// // USART_DMACmd(BSP_USART, USART_DMAReq_Tx, DISABLE);

	return ch;
}

int fgetc(FILE *f)
{

	while (USART_GetFlagStatus(BSP_USART, USART_FLAG_RXNE) == RESET)
		;

	return (int)USART_ReceiveData(BSP_USART);
}
