#include "response.h"
#include <string.h>
// #include "DMA_Init.h"
usart_data_typed test_data;
uint8_t Frame_it;

static void do_uart(uint8_t *data)
{
}

static void do_i2c_8RegisterAddr(uint8_t *data)
{
    uint32_t err = 0;

    uint8_t i2c_mode_sel = data[2];
    uint8_t slave_adress = data[3];
    uint16_t NumByteToProsess = data[4];
    uint8_t RegisterAddr = data[5];
    uint8_t *Value = &data[6];

    // if (NumByteToProsess == 0xff)
    // {
    //     NumByteToProsess++;
    // }

    switch (i2c_mode_sel)
    {
    case 0x01: // 写一个字节寄存器
        I2C_ByteWrite(slave_adress, RegisterAddr, *Value);
        break;
    case 0x02: // 写多个字节寄存器  待添加当实际接收到的写字节个数，少于NumByteToProsess时情景的判断
        I2C_BufferWrite(slave_adress, RegisterAddr, Value, NumByteToProsess);
        break;
    case 0x03: // 读一个字节寄存器
        err = I2C_byteRead(slave_adress, RegisterAddr, Value);
        if (err)
            printf("RegisterAddr : 0x%02x Return Value = 0x%02x \r\n", RegisterAddr, *Value);
        break;
    case 0x04: // 读多个字节寄存器
        err = I2C_BufferRead(slave_adress, RegisterAddr, Value, NumByteToProsess);
        if (err)
        {
            for (uint16_t i = 0; i < NumByteToProsess; i++)
            {
                printf("RegisterAddr : 0x%02x Return Value : 0x%02x \r\n", RegisterAddr + i, Value[i]);
            }
        }
        break;
    case 0x05:
        Find_i2c_device();
        break;
    case 0x06: // EEPROM按页写入
        if (I2C_EE_8Addr_PageWrite(slave_adress, RegisterAddr, Value, NumByteToProsess))
            printf("EEPROM Page Write Success! \r\n");
        break;
    case 0x07:                                                                         // EEPROM按页读
        err = I2C_BufferRead(slave_adress, RegisterAddr, Value, NumByteToProsess + 1); // 需要读取256个字节
        if (err)
        {
            test_data.SendData[0] = SendFrameHeader; // 发送帧头
            for (uint8_t i = 1; i < 6; i++)          // 返回帧结构
            {
                test_data.SendData[i] = data[i];
            }
            for (uint16_t i = 0; i < NumByteToProsess + 1; i++) // 读到的数据,NumByteToProsess+1 = 256
            {
                test_data.SendData[i + 6] = Value[i];
                // printf("RegisterAddr : 0x%02x Return Value : 0x%02x \r\n", RegisterAddr + i,test_data.SendData[i + 6]);//测试打印数据
            }
            for (uint16_t i = 0; i < NumByteToProsess + 6 + 1; i++)
            {
                usart_send_data(test_data.SendData[i]);
            }
            // test_data.SendData[6 + NumByteToProsess] = '\0';
            // usart_send_String(test_data.SendData); // 串口发送数据
        }
        break;
    default:
        printf("Use 8bit RegisterAddr mode Error! \r\n");
        break;
    }
}

static void do_i2c_16RegisterAddr(uint8_t *data)
{
    uint32_t err = 0;

    uint8_t i2c_mode_sel = data[2];
    uint8_t slave_adress = data[3];
    uint8_t NumByteToProsess = data[4];
    uint8_t RegisterAddr_H = data[5];
    uint8_t RegisterAddr_L = data[6];
    uint16_t RegisterAddr = (uint16_t)(RegisterAddr_H << 8 | RegisterAddr_L);
    uint8_t *Value = &data[7];
    switch (i2c_mode_sel)
    {
    case 0x01: // 写一个字节寄存器
        I2C_Write_16addr(slave_adress, RegisterAddr, *Value);
        break;
    case 0x02: // 写多个字节寄存器  待添加当实际接收到的写字节个数，少于NumByteToProsess时情景的判断
        I2C_BufferWrite_16addr(slave_adress, RegisterAddr, Value, NumByteToProsess);
        break;
    case 0x03: // 读一个字节寄存器
        err = I2C_Read16addr(slave_adress, RegisterAddr, Value);
        if (err)
            printf("RegisterAddr : 0x%04x Return Value = 0x%02x \r\n", RegisterAddr, *Value);
        break;
    case 0x04: // 读多个字节寄存器
        err = I2C_BufferRead_16addr(slave_adress, RegisterAddr, Value, NumByteToProsess);
        if (err)
        {
            for (size_t i = 0; i < NumByteToProsess; i++)
            {
                printf("RegisterAddr : 0x%04x Return Value : 0x%02x \r\n", RegisterAddr + i, Value[i]);
            }
        }
        break;
    case 0x05:
        Find_i2c_device();
        break;
    case 0x06: // EEPROM按页写入
        err = I2C_EE_16Addr_PageWrite(slave_adress, RegisterAddr, Value, NumByteToProsess);
        if (err)
            printf("EEPROM Page Write Success! \r\n");
        break;
    case 0x07:                                                                                // EEPROM按页读
        err = I2C_BufferRead_16addr(slave_adress, RegisterAddr, Value, NumByteToProsess + 1); // 需要读取256个字节
        if (err)
        {
            test_data.SendData[0] = SendFrameHeader; // 发送帧头
            for (uint8_t i = 1; i < 7; i++)          // 返回帧结构
            {
                test_data.SendData[i] = data[i];
            }
            for (uint16_t i = 0; i < NumByteToProsess + 1; i++) // 读到的数据,NumByteToProsess+1 = 256
            {
                test_data.SendData[i + 7] = Value[i];
                // printf("RegisterAddr : 0x%02x Return Value : 0x%02x \r\n", RegisterAddr + i,test_data.SendData[i + 6]);//测试打印数据
            }
            for (uint16_t i = 0; i < NumByteToProsess + 7 + 1; i++)
            {
                usart_send_data(test_data.SendData[i]);
            }
            // test_data.SendData[6 + NumByteToProsess] = '\0';
            // usart_send_String(test_data.SendData); // 串口发送数据
        }
        break;
    default:
        printf("Use 16bit RegisterAddr mode Error! \r\n");
        break;
    }
}

static void do_spi(uint8_t *data)
{
}

uint8_t do_process(usart_data_typed *udata)
{
    // usart_send_String(udata->data);
    uint8_t Header = udata->data[0];
    uint8_t mode_sel = udata->data[1];
    // 待添加帧尾的CRC校验

    if (Header != Frame_header)
    {
        udata->flag = 0;
        udata->len = 0;
        return 0;
    }
    switch (mode_sel)
    {
    case MODE_UART:
        do_uart(udata->data);
        break;
    case MODE_I2C_8BIT_ADDR:
        do_i2c_8RegisterAddr(udata->data);
        break;
    case MODE_I2C_16BIT_ADDR:
        do_i2c_16RegisterAddr(udata->data);
        break;
    case MODE_SPI:
        do_spi(udata->data);
        break;
    default:
        break;
    }
    udata->flag = 0;
    udata->len = 0;
    return 1;
}
uint8_t response_handle(uint8_t error, uint8_t *UartRxBuf, uint16_t UartRxLen)
{
    if (test_data.flag == 1)
    {
        if (test_data.DataOverflow == 1)
        {
            printf("Recive Data Too Much!\r\n");
            test_data.DataOverflow = 0; // 清空数据溢出标志位
            test_data.flag = 0;         // 清空接收标志位
            return 0;
        }
        memcpy(test_data.data, UartRxBuf, UartRxLen); // 将接收到的数据复制到test_data.data
        test_data.data[UartRxLen] = '\0';
        error = do_process(&test_data);
        if (!error)
        {
            printf("Do process Error!\r\n");
            error = 0;
        }
    }
    return 1;
}