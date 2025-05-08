
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
/**
 * @brief  配置SPI数据传输顺序：MSB First 或 LSB First
 * @param  SPIx: SPI实例，如SPI1、SPI2等
 * @param  SPI_FirstBit: SPI_FirstBit_MSB 或 SPI_FirstBit_LSB
 * @retval None
 */
void SPI_FirstBitConfig(SPI_TypeDef* SPIx, uint16_t SPI_FirstBit)
{
    /* 检查参数是否合法 */
    assert_param(IS_SPI_ALL_PERIPH(SPIx));
    assert_param(IS_SPI_FIRST_BIT(SPI_FirstBit));

    /* 清除当前LSBFIRST位的设置 */
    SPIx->CR1 &= ~SPI_CR1_LSBFIRST;

    /* 设置新的LSBFIRST位 */
    SPIx->CR1 |= SPI_FirstBit;
}

int main(void)
{
	uint8_t error = 0;
	uint16_t color = 0;
	board_init();			 // 板级初始化
	NVIC_Configuration();	 // 中断优先级初始化
	uart1_init(115200U);	 // 串口1初始化
	DMA_Uart1_Init_Config(); // DMA串口1初始化
	User_I2C_Init(); // I2C初始化
	// SPI_FLASH_BUS_Init();	 // SPI FLASH总线初始化
	// test_SPI_Flash();		 // 测试SPI FLASH
	SPI_Screen_BUS_Init(); // SPI屏幕总线初始化

	OV2640_Hardware_Init();
	LCD_Screen_Init();									 // LCD屏幕初始化
	LCD_Address_Set(0, 0, LCD_WIDTH - 1, LCD_HIDTH - 1); // 设置显示区
	LCD_Fill(0, 0, LCD_WIDTH, LCD_HIDTH, 0x0000);		 //  清屏

	OV2640_Image_Config(); // OV2640图像配置

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
		if (test_data.flag == 1)
		{
			if (test_data.DataOverflow == 1)
			{
				printf("Recive Data Too Much!\r\n");
				test_data.DataOverflow = 0; // 清空数据溢出标志位
				test_data.flag = 0;			// 清空接收标志位
				continue;
			}
			memcpy(test_data.data, DMA_USART1_RX_BUF, usart1_rx_len); // 将接收到的数据复制到test_data.data
			test_data.data[usart1_rx_len] = '\0';
			error = do_process(&test_data);
			if (!error)
			{
				printf("Do process Error!\r\n");
				error = 0;
			}
		}
	}
}



