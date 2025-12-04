#include <stdint.h>
#include <stdio.h>
// #include <string.h>

#include "MY_your_driver.h"
#include "NVIC_Init.h"
#include "board.h"
#include "bsp_uart.h"

#include "DMA_Init.h"
#include "OV2640_init.h"

#include "i2c_init.h"
#include "response.h"
#include "spi_init.h"

#include "Test_code.h"

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
  Ov2640PlayVideo();

  // printf("hello World\n");
  // usart_send_String_DMA((const uint8_t *)tag, 4);

  // uart_send_buffer_DMA((const uint8_t *)TAG, strlen(TAG));
  while (1) {
    if (test_data.flag != 1) {
      continue;
    }
    if (test_data.DataOverflow == 1) {
      printf("Recive Data Too Much!\r\n");
      test_data.DataOverflow = 0; // 清空数据溢出标志位
      test_data.flag = 0;         // 清空接收标志位
      break;
    }
    uart_copy_receive_data(test_data.data, usart1_rx_len);
    test_data.data[usart1_rx_len] = '\0';
    error = do_process(&test_data);
    if (!error) {
      printf("Do process Error!\r\n");
      error = 0;
    }
  }
}
