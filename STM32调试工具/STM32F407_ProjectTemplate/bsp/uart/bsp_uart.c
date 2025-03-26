

#include "bsp_uart.h"
#include "stdio.h"

void uart1_init(uint32_t __Baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(BSP_USART_TX_RCC, ENABLE);
	// RCC_AHB1PeriphClockCmd(BSP_USART_RX_RCC,ENABLE);

	GPIO_PinAFConfig(BSP_USART_TX_PORT, BSP_USART_TX_AF_PIN, BSP_USART_AF);
	GPIO_PinAFConfig(BSP_USART_RX_PORT, BSP_USART_RX_AF_PIN, BSP_USART_AF);

	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = BSP_USART_TX_PIN; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);


	GPIO_StructInit(&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = BSP_USART_RX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(BSP_USART_RCC, ENABLE);

	USART_DeInit(BSP_USART);

	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate = __Baud;									// ���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֽڳ���Ϊ8bit
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// 1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// û��У��λ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// ����������Ϊ�շ�ģʽ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ���ṩ����
	USART_Init(BSP_USART, &USART_InitStructure);									// ����ز�����ʼ��������1

	USART_ClearFlag(BSP_USART, USART_FLAG_RXNE);

	NVIC_Configuration();

	USART_ITConfig(BSP_USART, USART_IT_RXNE, ENABLE);
	USART_ITConfig(BSP_USART, USART_IT_IDLE, ENABLE);
	USART_Cmd(BSP_USART, ENABLE);
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

	return ch;
}

int fgetc(FILE *f)
{

	while (USART_GetFlagStatus(BSP_USART, USART_FLAG_RXNE) == RESET)
		;

	return (int)USART_ReceiveData(BSP_USART);
}

static void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;

	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;

	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;

	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}
