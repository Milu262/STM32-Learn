
#include "board.h"
#include "bsp_uart.h"
#include "NVIC_Init.h"
#include <stdio.h>
#include "i2c_init.h"
#include "i2c_handle.h"
#include "response.h"
#include "spi_init.h"
#include "SPI_Control.h"
#include "DMA_Init.h"

#include "Test_code.h"

// #define SHT30_SENSOR_ADDR 0x44 /*!< Slave address of the SHT30 sensor */
extern uint8_t DMA_Uart_SendBuff[SENDBUFF_SIZE];
int main(void)
{
	// uint8_t error = 0;
	board_init();
	NVIC_Configuration();
	uart1_init(115200U);
	DMA_Uart1_Init_Config();

	// User_I2C_Init();
	// SPI_BUS_Init();
	// 测试DMA传输
	// for (uint8_t i = 0; i < SENDBUFF_SIZE; i++)
	// {
	//     DMA_Uart_SendBuff[i] = 'A';
	// 	 //printf("DMA_Uart_SendBuff[%d] = %x\r\n", i, DMA_Uart_SendBuff[i]);
	// }
	// USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
	// test_SPI_Flash();

	// DMA_InitTypeDef DMA_InitStructure;
	// RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); // ¿ªÆôDMAÊ±ÖÓ
	// DMA_DeInit(DMA2_Stream5);
	// DMA_InitStructure.DMA_Channel = DMA_Channel_4;							// Í¨µÀÑ¡Ôñ
	// DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;		// DMAÍâÉèµØÖ·
	// DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)DMA_USART1_RX_BUF;	// DMA ´æ´¢Æ÷0µØÖ·
	// DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;					// ´æ´¢Æ÷µ½ÍâÉèÄ£Ê½
	// DMA_InitStructure.DMA_BufferSize = USART_MAX_LEN;						// Êý¾Ý´«ÊäÁ¿
	// DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		// ÍâÉè·ÇÔöÁ¿Ä£Ê½
	// DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// ´æ´¢Æ÷ÔöÁ¿Ä£Ê½
	// DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // ÍâÉèÊý¾Ý³¤¶È:8Î»
	// DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			// ´æ´¢Æ÷Êý¾Ý³¤¶È:8Î»
	// DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							// Ê¹ÓÃÆÕÍ¨Ä£Ê½
	// DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;					// ¸ßµÈÓÅÏÈ¼¶
	// DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;					// ²»¿ªÆôFIFOÄ£Ê½
	// DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;			// FIFOãÐÖµ
	// DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;				// ´æ´¢Æ÷Í»·¢µ¥´Î´«Êä
	// DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;		// ÍâÉèÍ»·¢µ¥´Î´«Êä
	// DMA_Init(DMA2_Stream5, &DMA_InitStructure);
	// DMA_Cmd(DMA2_Stream5, ENABLE); // Ê¹ÄÜDMA2_Stream5Í¨µÀ

	// DMA_DeInit(DMA2_Stream7); // ³õÊ¼»¯DMA Stream
	// while (DMA_GetCmdStatus(DMA2_Stream7) != DISABLE)
	// 	; // µÈ´ýDMA¿ÉÅäÖÃ

	// /* ÅäÖÃDMA2 Stream7£¬USART1·¢ËÍ */
	// DMA_InitStructure.DMA_Channel = DMA_Channel_4;							// Í¨µÀÑ¡Ôñ
	// DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;			// DMAÍâÉèµØÖ·
	// DMA_InitStructure.DMA_Memory0BaseAddr = (u32)DMA_USART1_TX_BUF;			// DMA ´æ´¢Æ÷0µØÖ·
	// DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;					// ´æ´¢Æ÷µ½ÍâÉèÄ£Ê½
	// DMA_InitStructure.DMA_BufferSize = USART_MAX_LEN;						// Êý¾Ý´«ÊäÁ¿
	// DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;		// ÍâÉè·ÇÔöÁ¿Ä£Ê½
	// DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					// ´æ´¢Æ÷ÔöÁ¿Ä£Ê½
	// DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // ÍâÉèÊý¾Ý³¤¶È:8Î»
	// DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;			// ´æ´¢Æ÷Êý¾Ý³¤¶È:8Î»
	// DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;							// Ê¹ÓÃÆÕÍ¨Ä£Ê½
	// DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;					// ÖÐµÈÓÅÏÈ¼¶
	// DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	// DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	// DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;			// ´æ´¢Æ÷Í»·¢µ¥´Î´«Êä
	// DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; // ÍâÉèÍ»·¢µ¥´Î´«Êä
	// DMA_Init(DMA2_Stream7, &DMA_InitStructure);							// ³õÊ¼»¯DMA Stream7

	// DMA_ITConfig(DMA2_Stream7, DMA_IT_TC, ENABLE); // DMA2´«ÊäÍê³ÉÖÐ¶Ï
	// DMA_Cmd(DMA2_Stream7, DISABLE);				   // ²»Ê¹ÄÜ

	while (1)
	{
		/* code */
	}

	// while (1)
	// {

	// 	if (test_data.flag == 1)
	// 	{
	// 		error = do_process(&test_data);
	// 		if (!error)
	// 		{
	// 			printf("do_process Fail!\r\n");
	// 			error = 0;
	// 		}
	// 	}
	// }
}
