#include "response.h"

usart_data_typed test_data;

int do_process(usart_data_typed *udata)
{
    //usart_send_String(udata->data);

    if (udata->data[0] != 0x55)

    {
        // printf("\n 0x%x \r\n 0X%x",udata->data[1],udata->data[2]);
        udata->flag = 0;
        udata->len = 0;
        return 1;
    }
    switch (udata->data[1])
    {
    case 0x01:
        do_uart(udata->data);
        break;
    case 0x02:
        do_i2c_8RegisterAddr(udata->data);
        break;
    case 0x03:
        do_i2c_16RegisterAddr(udata->data);
        break;
    case 0x04:
        do_spi(udata->data);
        break;
    default:
        break;
    }
    udata->flag = 0;
    udata->len = 0;
    return 0;
}

static void do_uart(uint8_t *data)
{
    
}

static void do_i2c_8RegisterAddr(uint8_t *data)
{
    if (data[2] == 0x01)
    {
        I2C_ByteWrite(data[3],data[4],data[5]);
    }
    else 
    {
        I2C_ByetsWrite(data[3],data[4],data+5,data[2]);
    }
    
}

static void do_i2c_16RegisterAddr(uint8_t *data)
{
    
}

static void do_spi(uint8_t *data)
{
    
}
