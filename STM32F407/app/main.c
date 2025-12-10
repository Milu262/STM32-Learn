#include <stdint.h>

#include "NVIC_Init.h"
#include "board.h"
#include "bsp_uart.h"

#include "DMA_Init.h"
#include "OV2640_init.h"

#include "i2c_init.h"
#include "response.h"
#include "spi_init.h"

#include "Test_code.h"

#include "hdlc_core.h"

static uint8_t RxBuffer[USART_MAX_LEN] = {0};
static uint16_t RxCount = 0;
int main(void) {
  board_init();           // 板级初始化
  NVIC_Configuration();   // 中断优先级初始化
  uart_init(115200U);     // 串口初始化
  DMA_Uart_Init_Config(); // DMA串口初始化
  User_I2C_Init();        // I2C初始化
  Ov2640PlayVideo();      // Ov2640显示到LCD

  while (1) {
    // if (test_data.flag != 1) {
    //   continue;
    // }
    // if (test_data.DataOverflow == 1) {
    //   printf("Recive Data Too Much!\r\n");
    //   test_data.DataOverflow = 0; // 清空数据溢出标志位
    //   test_data.flag = 0;         // 清空接收标志位
    //   break;
    // }
    // uint16_t RxCount = usart_get_rx_count();
    // uart_copy_receive_data(test_data.data, RxCount);
    // test_data.data[RxCount] = '\0';
    // error = do_process(&test_data);
    // if (!error) {
    //   printf("Do process Error!\r\n");
    //   error = 0;
    // }

    if (get_rx_status() != RX_STATE_RECEIVING)
      continue;
    enter_rx_IDLE();
    RxCount = usart_get_rx_count();
    uart_copy_receive_data(RxBuffer, RxCount);
    hdlc_process_stream(RxBuffer, RxCount);
  }
}
