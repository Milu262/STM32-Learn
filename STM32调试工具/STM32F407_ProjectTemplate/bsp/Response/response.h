// #include "stm32f4xx.h"
#include ".\i2c\i2c_handle.h"
#define data_size 100


typedef struct
{
	unsigned int flag;//标志位
	unsigned int len;
	uint8_t data[data_size];


} usart_data_typed;

// 声明一个外部变量 test_data，其类型为 usart_data_typed
// 这意味着 test_data 变量定义在别的源文件中，但在此文件中可以访问和使用
extern usart_data_typed test_data;

uint8_t do_process(usart_data_typed *udata);
static void do_uart(uint8_t *data);
static void do_i2c_8RegisterAddr(uint8_t *data);
static void do_i2c_16RegisterAddr(uint8_t *data);
static void do_spi(uint8_t *data);
