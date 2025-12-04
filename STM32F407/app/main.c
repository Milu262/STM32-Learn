#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "MY_your_driver.h"
#include "NVIC_Init.h"
#include "board.h"
#include "bsp_uart.h"

#include "DMA_Init.h"
#include "OV2640_init.h"
#include "SPI_Control.h"
#include "SPI_Screen_init.h"
#include "i2c_handle.h"
#include "i2c_init.h"
#include "response.h"
#include "spi_init.h"

#include "Test_code.h"
// static uint8_t USART2_TX_BUF[4] = {0};

// static const uint8_t tag[4]={0x4d,0x41,0x49,0x4e};
int main(void) {
  uint8_t error = 0;
  board_init(); // 板级初始化
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  NVIC_Configuration(); // 中断优先级初始化
  uart_init(115200U);   // 串口初始化

  DMA_Uart_Init_Config(); // DMA串口初始化

  User_I2C_Init(); // I2C初始化
  // SPI_FLASH_BUS_Init();	 // FLASH总线初始化
  // test_SPI_Flash();		 // 测试FLASH
  SPI_Screen_BUS_Init(); // SPI屏幕总线初始化

  OV2640_Hardware_Init();
  LCD_Screen_Init(); // LCD屏幕初始化

  LCD_Address_Set(0, 0, LCD_WIDTH - 1, LCD_HIDTH - 1); // 设置显示区
  LCD_Fill(0, 0, LCD_WIDTH, LCD_HIDTH, 0x0000);        //  清屏

  error = OV2640_Image_Config(); // OV2640图像配置
                                 // GPIO_SetBits(GPIOB, GPIO_Pin_2);

  // GPIO_SetBits(GPIOE, GPIO_Pin_7);

  SPI_Cmd(LCD_SPI, DISABLE); // 设置前先关闭SPI
  // 设置选择 16 位数据帧格式
  SPI_DataSizeConfig(LCD_SPI, SPI_DataSize_16b);

  SPI_Cmd(LCD_SPI, ENABLE);
  LCD_SPI_CS_ON(0); // 使能SPI的数据传输

  DMA_Cmd(DMA2_Stream1, ENABLE); // DMA2,Stream1
  DCMI_Cmd(ENABLE);              // DCMI启动

  DCMI_CaptureCmd(ENABLE); // DCMI采集启动
  // 添加延时确保所有外设初始化完成
  // for (volatile int i = 0; i < 1000000; i++)
  //   ;
  // 手动写入
  // USART2_TX_BUF[0] = 'M';
  // USART2_TX_BUF[1] = 'A';
  // USART2_TX_BUF[2] = 'I';
  // USART2_TX_BUF[3] = 'N';
  //   uint8_t buf[] = "MAIN";
  // // 用轮询（非 DMA）发送这 4 字节
  // for (int i = 0; i < 4; i++) {
  //     while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
  //     USART_SendData(USART2, buf[i]);
  // }
// 延时确保所有外设初始化完成
  for(volatile int i=0; i<0x100000; i++);
  
  // 更精确的延时函数
  // 系统时钟168MHz，每个循环约6个周期，要延时8.68us需要延时约243次循环
  const int bit_delay = 243;
  
  // 使用PE7引脚发送测试数据（注意：需要确认PE7已被正确初始化为输出）
  // 如果PE7未初始化，先进行初始化：
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  GPIO_InitTypeDef pe7_init;
  pe7_init.GPIO_Pin = GPIO_Pin_7;
  pe7_init.GPIO_Mode = GPIO_Mode_OUT;
  pe7_init.GPIO_OType = GPIO_OType_PP;
  pe7_init.GPIO_Speed = GPIO_Speed_2MHz;
  pe7_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &pe7_init);
  
  const char str[] = "MAIN\r\n";
  for(int i=0; i<sizeof(str)-1; i++) {
    char ch = str[i];
    
    // 发送起始位(低电平)
    GPIO_ResetBits(GPIOE, GPIO_Pin_7);
    for(volatile int d=0; d<bit_delay; d++);
    
    // 发送数据位(低位先发)
    for(int bit=0; bit<8; bit++) {
      if(ch & (1<<bit)) {
        GPIO_SetBits(GPIOE, GPIO_Pin_7);
      } else {
        GPIO_ResetBits(GPIOE, GPIO_Pin_7);
      }
      for(volatile int d=0; d<bit_delay; d++);
    }
    
    // 发送停止位(高电平)
    GPIO_SetBits(GPIOE, GPIO_Pin_7);
    for(volatile int d=0; d<bit_delay; d++);
  }
  // printf("hello World\n");
  // usart_send_String_DMA((const uint8_t *)tag, 4);

  // uart_send_buffer_DMA((const uint8_t *)TAG, strlen(TAG));
  while (1) {
    // response_handle(error, DMA_USART1_RX_BUF,
    // usart1_rx_len);//串口接收数据处理
    if (test_data.flag != 1) {
      continue;
    }
    if (test_data.DataOverflow == 1) {
      printf("Recive Data Too Much!\r\n");
      test_data.DataOverflow = 0; // 清空数据溢出标志位
      test_data.flag = 0;         // 清空接收标志位
      break;
    }
    // memcpy(test_data.data, DMA_USART1_RX_BUF,
    //        usart1_rx_len); // 将接收到的数据复制到test_data.data
    uart_copy_receive_data(test_data.data, usart1_rx_len);
    test_data.data[usart1_rx_len] = '\0';
    error = do_process(&test_data);
    if (!error) {
      printf("Do process Error!\r\n");
      error = 0;
    }
  }
}
