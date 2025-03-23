
#include "board.h"
#include "bsp_uart.h"
#include <stdio.h>
#include "i2c_init.h"
#include "i2c_handle.h"
#include "response.h"

#define SHT30_SENSOR_ADDR 0x44 /*!< Slave address of the SHT30 sensor */
int main(void)
{
	// float t = 0.0;
	// int count = 0;

	// uint8_t recive[4]={0};
	uint32_t error = 0;
	board_init();
	uart1_init(115200U);
	printf("USART Init!!\r\n");
	User_I2C_Init();


	//error = I2C_ByteWrite(0x34, 0x32, 0x30);
	Find_i2c_device();
	while (1)
	{

		// if (test_data.flag == 1)
		// {
		// 	error = do_process(&test_data);
		// 	if(error == 1)
		// 	{

		// 	}
		// }
	}
}
