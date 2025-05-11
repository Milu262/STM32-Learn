
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
#include "SPI_Screen_init.h"
#include "OV2640_init.h"

#include "Test_code.h"

// #define SHT30_SENSOR_ADDR 0x44 /*!< Slave address of the SHT30 sensor */

int main(void)
{
	uint8_t error = 0;
	uint16_t color = 0;
	board_init();			 // 板级初始化
	NVIC_Configuration();	 // 中断优先级初始化
	uart1_init(115200U);	 // 串口1初始化
	DMA_Uart1_Init_Config(); // DMA串口1初始化
	User_I2C_Init();		 // I2C初始化
	// SPI_FLASH_BUS_Init();	 // SPI FLASH总线初始化
	// test_SPI_Flash();		 // 测试SPI FLASH
	SPI_Screen_BUS_Init(); // SPI屏幕总线初始化

	OV2640_Hardware_Init();
	LCD_Screen_Init();									 // LCD屏幕初始化
	LCD_Address_Set(0, 0, LCD_WIDTH - 1, LCD_HIDTH - 1); // 设置显示区
	LCD_Fill(0, 0, LCD_WIDTH, LCD_HIDTH, 0x0000);		 //  清屏

	OV2640_Image_Config(); // OV2640图像配置

	// GPIO_SetBits(GPIOE, GPIO_Pin_7);

	SPI_Cmd(LCD_SPI, DISABLE); // 设置前先关闭SPI
	// 设置选择 16 位数据帧格式
	SPI_DataSizeConfig(LCD_SPI, SPI_DataSize_16b);

	// //设置数据传输时先发送最低位
	// SPI_FirstBitConfig(LCD_SPI, SPI_FirstBit_LSB);

	SPI_Cmd(LCD_SPI, ENABLE);
	LCD_SPI_CS_ON(0); // 使能SPI的数据传输

	DMA_Cmd(DMA2_Stream1, ENABLE); // DMA2,Stream1
	DCMI_Cmd(ENABLE);			   // DCMI启动
	DCMI_CaptureCmd(ENABLE);	   // DCMI采集启动

	while (1)
	{
		response_handle(error, DMA_USART1_RX_BUF,usart1_rx_len);
	}
}
