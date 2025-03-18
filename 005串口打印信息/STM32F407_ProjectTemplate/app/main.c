
#include "board.h"
#include "bsp_uart.h"
#include <stdio.h>
#include "i2c_init.h"
#include "i2c_handle.h"

#define SHT30_SENSOR_ADDR 0x44 /*!< Slave address of the SHT30 sensor */
int main(void)
{
	//float t = 0.0;
	//int count = 0;

	// uint8_t recive[4]={0};
	
	board_init();
	
	uart1_init(115200U);
	User_I2C_Init();
	printf("USART Init!!\r\n");
	
if (I2C_ByteWrite(SHT30_SENSOR_ADDR,I2C1 ,0x00,0x00))
{
	/* code */
}


	while(1)
	{

		if(test_data.flag == 1)
		{			
			do_process(&test_data);		
            
		}
		}
	

}
