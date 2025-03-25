#include "response.h"

usart_data_typed test_data;

int do_process(usart_data_typed *udata)
{
    // usart_send_String(udata->data);
    uint8_t Header = udata->data[0];
    uint8_t mode_sel = udata->data[1];
    if (Header != 0x55)

    {
        // printf("\n 0x%x \r\n 0X%x",udata->data[1],udata->data[2]);
        udata->flag = 0;
        udata->len = 0;
        return 1;
    }
    switch (mode_sel)
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
    uint8_t i2c_mode_sel = data[2];
    uint8_t slave_adress = data[3];
    uint8_t NumByteToProsess = data[4];
    uint8_t RegisterAddr = data[5];
    uint8_t *Value = &data[6];
    switch (i2c_mode_sel)
    {
    case 0x01: // 写一个字节寄存器
        I2C_ByteWrite(slave_adress, RegisterAddr, *Value);
        break;
    case 0x02: // 写多个字节寄存器
        I2C_ByetsWrite(slave_adress, RegisterAddr, Value, NumByteToProsess);
        break;
    case 0x03: // 读一个字节寄存器
        I2C_byteRead(slave_adress, RegisterAddr, Value);
        printf("RegisterAddr : 0x%02x Return Value = 0x%02x \r\n", RegisterAddr, *Value);
        break;
    case 0x04: // 读多个字节寄存器
        I2C_BufferRead(slave_adress, RegisterAddr, Value, NumByteToProsess);
        for (size_t i = 0; i < NumByteToProsess; i++)
        {
            printf("RegisterAddr : 0x%02x Return Value : 0x%02x \r\n", RegisterAddr + i, Value[i]);
        }
        break;
    default:
        printf("Use 8bit RegisterAddr mode Error! \r\n");
        break;
    }
}

static void do_i2c_16RegisterAddr(uint8_t *data)
{
}

static void do_spi(uint8_t *data)
{
}
