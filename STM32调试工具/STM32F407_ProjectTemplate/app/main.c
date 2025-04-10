
#include "board.h"
#include "bsp_uart.h"
#include <stdio.h>
#include "i2c_init.h"
#include "i2c_handle.h"
#include "response.h"
#include "spi_init.h"
#include "SPI_Control.h"

#include "Test_code.h"

// #define SHT30_SENSOR_ADDR 0x44 /*!< Slave address of the SHT30 sensor */

int main(void)
{
	uint8_t error = 0;
	board_init();
	uart1_init(115200U);
	User_I2C_Init();
	SPI_BUS_Init();
	test_SPI_Flash();

	// error = I2C_ByteWrite(0x34, 0x32, 0x30);
	// Find_i2c_device();
	// error = I2C_ByteWrite(eeprom_adress << 1, 0xf1, 0xff);
	// //写一个字节大概需要4ms，保险使用5ms
	// delay_1ms(5);
	// //I2C_EE_WaitEepromStandbyState(eeprom_adress<<1);

	while (1)
	{

		if (test_data.flag == 1)
		{
			error = do_process(&test_data);
			if (!error)
			{
				printf("do_process Fail!\r\n");
				error = 0;
			}
		}
	}
}
