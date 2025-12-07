#include <stdio.h>
#include <string.h>
#include "DMA_Init.h"
#include "board.h"
#include "bsp_uart.h"
#include "response.h"


// #define SHT30_SENSOR_ADDR 0x44 /*!< Slave address of the SHT30 sensor */

int main(void)
{
	uint8_t error = 0;
	// uint16_t color = 0;
	error = hardware_init();

	while (1)
	{
		// response_handle(error, DMA_USART1_RX_BUF, usart1_rx_len);//串口接收数据处理

		if (test_data.flag != 1)
		{
			continue;
		}
		if (test_data.DataOverflow == 1)
		{
			printf("Recive Data Too Much!\r\n");
			test_data.DataOverflow = 0; // 清空数据溢出标志位
			test_data.flag = 0;			// 清空接收标志位
			break;
		}
		// memcpy(test_data.data, DMA_USART1_RX_BUF, usart1_rx_len); // 将接收到的数据复制到test_data.data
		usart_copy_String_DMA(test_data.data, usart1_rx_len);
		test_data.data[usart1_rx_len] = '\0';
		error = do_process(&test_data);
		if (!error)
		{
			printf("Do process Error!\r\n");
			error = 0;
		}
	}
}
