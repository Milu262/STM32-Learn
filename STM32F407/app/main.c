
#include <stdio.h>
#include <string.h>
#include "../bsp/DMA/DMA_Init.h"
#include "../board/board.h"
#include "../bsp/uart/bsp_uart.h"
#include "hdlc_core.h"
#include "../bsp/Response/response.h"

// #define SHT30_SENSOR_ADDR 0x44 /*!< Slave address of the SHT30 sensor */

static uint8_t ReceiveData[USART_MAX_LEN] = {0};
static uint16_t RxCount = 0;
int main(void)
{

	uint8_t error = 0;
	// uint16_t color = 0;
	error = hardware_init();

	while (1)
	{

		// if (test_data.flag != 1)
		// {
		// 	continue;
		// }
		// if (test_data.DataOverflow == 1)
		// {
		// 	printf("Recive Data Too Much!\r\n");
		// 	test_data.DataOverflow = 0; // 清空数据溢出标志位
		// 	test_data.flag = 0;			// 清空接收标志位
		// 	break;
		// }

		if (get_rx_status() != RX_STATE_RECEIVING)
			continue;
		enter_rx_IDLE();
		RxCount = GetUsartRxCount();

		// response_handle(RxCount);
		uart_copy_receive_data(ReceiveData, RxCount);
		// usart_send_String_DMA(ReceiveData,RxCount);//接收正常
		hdlc_process_stream(ReceiveData, RxCount);

		// usart_copy_String_DMA(test_data.data, RxCount);
		// test_data.data[RxCount] = '\0';
		// usart_copy_String_DMA(test_data.data, usart1_rx_len);
		// test_data.data[usart1_rx_len] = '\0';
		// error = do_process(&test_data);
		// if (!error)
		// {
		// 	printf("Do process Error!\r\n");
		// 	error = 0;
		// }
	}
}
