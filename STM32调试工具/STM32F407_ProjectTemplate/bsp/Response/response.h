//#include "stm32f4xx.h"
#include ".\i2c\i2c_handle.h"
#define data_size 100


typedef struct
{
	unsigned int flag;
	unsigned int len;
	uint8_t data[data_size];

}usart_data_typed;

extern usart_data_typed test_data;

int do_process(usart_data_typed* udata);
static void do_uart(uint8_t *data);
static void do_i2c_8RegisterAddr(uint8_t *data);
static void do_i2c_16RegisterAddr(uint8_t *data);
static void do_spi(uint8_t *data);
