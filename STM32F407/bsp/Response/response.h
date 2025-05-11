// #include "stm32f4xx.h"
#include ".\i2c\i2c_handle.h"
#define data_size 256 + 32	 // 数据长度
#define Frame_header 0x55	 // 接收帧头
#define SendFrameHeader 0xAA // 发送帧头

#define MODE_UART 0x01
#define MODE_I2C_8BIT_ADDR 0x02
#define MODE_I2C_16BIT_ADDR 0x03
#define MODE_SPI 0x04

typedef struct
{
	unsigned int flag; // 标志位
	// unsigned int err;		   // 数据类型
	unsigned int DataOverflow; // 数据溢出
	unsigned int len;
	uint8_t data[data_size];	 // 接收数据
	uint8_t SendData[data_size]; // 发送数据

} usart_data_typed;

// 声明一个外部变量 test_data，其类型为 usart_data_typed
// 这意味着 test_data 变量定义在别的源文件中，但在此文件中可以访问和使用
extern usart_data_typed test_data;
extern uint8_t Frame_it;

uint8_t do_process(usart_data_typed *udata);
static void do_uart(uint8_t *data);
static void do_i2c_8RegisterAddr(uint8_t *data);
static void do_i2c_16RegisterAddr(uint8_t *data);
static void do_spi(uint8_t *data);

uint8_t response_handle(uint8_t error,uint8_t *UartRxBuf,uint16_t UartRxLen);
