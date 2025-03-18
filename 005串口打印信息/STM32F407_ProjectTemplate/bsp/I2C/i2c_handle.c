#include "i2c_handle.h"

static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
    /* 使用串口 printf 输出错误信息，方便调试 */
    // EEPROM_ERROR("I2C 等待超时!errorCode = %d", errorCode);
    switch (errorCode)
    {
    case 0:
        /* code */
        break;
    case 1:
        /* code */
        break;
    case 2:
        /* code */
        break;
    case 3:
        /* code */
        break;
    default:
        break;
    }
    return 0;
}

uint32_t I2C_ByteWrite(u8 slave_adress, I2C_TypeDef *I2Cx, u8 *pBuffer, u8 WriteAddr)
{

    uint32_t I2CTimeout;
    /* 产生 I2C 起始信号 */
    I2C_GenerateSTART(I2Cx, ENABLE);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV5 事件并清除标志 */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }

    /* 发送设备地址 */
    I2C_Send7bitAddress(I2Cx, slave_adress,
                        I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV6 事件并清除标志 */
    while (!I2C_CheckEvent(I2Cx,
                           I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(1);
    }

    /* 发送要写入的寄存器地址*/
    I2C_SendData(I2Cx, WriteAddr);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV8 事件并清除标志 */
    while (!I2C_CheckEvent(I2Cx,
                           I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(2);
    }
    /* 发送一字节要写入的数据 */
    I2C_SendData(I2Cx, *pBuffer);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV8 事件并清除标志 */
    while (!I2C_CheckEvent(I2Cx,
                           I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(3);
    }

    /* 发送停止信号 */
    I2C_GenerateSTOP(I2Cx, ENABLE);

    return 1;
}

uint8_t I2C_ByetsWrite(uint8_t slave_adress, I2C_TypeDef *I2Cx, uint8_t *pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
    uint16_t i;
    uint8_t res;

    /* 每写一个字节调用一次 I2C_EE_ByteWrite 函数 */
    for (i = 0; i < NumByteToWrite; i++)
    {
        /* 等待 EEPROM 准备完毕 */
        // I2C_EE_WaitEepromStandbyState();
        /* 按字节写入数据 */
        res = I2C_ByteWrite(slave_adress, I2Cx, pBuffer++, WriteAddr++);
    }
    return res;
}

uint32_t I2C_BufferRead(uint8_t slave_adress, I2C_TypeDef *I2Cx, u8 *pBuffer, u8 ReadAddr, u16 NumByteToRead)
{
    uint32_t I2CTimeout = I2CT_LONG_TIMEOUT;

    //*((u8 *)0x4001080c) |=0x80;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(4);
    }
    /* Send START condition */
    I2C_GenerateSTART(I2Cx, ENABLE);
    //*((u8 *)0x4001080c) &=~0x80;

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(5);
    }

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2Cx, slave_adress, I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(6);
    }
    /* Clear EV6 by setting again the PE bit */
    I2C_Cmd(I2Cx, ENABLE);

    /* Send the EEPROM's internal address to write to */
    I2C_SendData(I2Cx, ReadAddr);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(7);
    }
    /* Send STRAT condition a second time */
    I2C_GenerateSTART(I2Cx, ENABLE);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(8);
    }
    /* Send EEPROM address for read */
    I2C_Send7bitAddress(I2Cx, slave_adress, I2C_Direction_Receiver);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(9);
    }
    /* While there is data to be read */
    while (NumByteToRead)
    {
        if (NumByteToRead == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(I2Cx, DISABLE);

            /* Send STOP Condition */
            I2C_GenerateSTOP(I2Cx, ENABLE);
        }

        I2CTimeout = I2CT_LONG_TIMEOUT;
        while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) == 0)
        {
            if ((I2CTimeout--) == 0)
                return I2C_TIMEOUT_UserCallback(10);
        }
        {
            /* Read a byte from the device */
            *pBuffer = I2C_ReceiveData(I2Cx);

            /* Point to the next location where the byte read will be saved */
            pBuffer++;

            /* Decrement the read bytes counter */
            NumByteToRead--;
        }
    }

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2Cx, ENABLE);

    return 1;
}

uint32_t I2C_byteRead(uint8_t slave_adress, I2C_TypeDef *I2Cx, u8 *pBuffer, u8 ReadAddr)
{
    uint32_t I2CTimeout = I2CT_LONG_TIMEOUT;

    //*((u8 *)0x4001080c) |=0x80;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(11);
    }
    /* Send START condition */
    I2C_GenerateSTART(I2Cx, ENABLE);
    //*((u8 *)0x4001080c) &=~0x80;

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(12);
    }

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2Cx, slave_adress, I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(13);
    }
    /* Clear EV6 by setting again the PE bit */
    I2C_Cmd(I2Cx, ENABLE);

    /* Send the EEPROM's internal address to write to */
    I2C_SendData(I2Cx, ReadAddr);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(14);
    }
    /* Send STRAT condition a second time */
    I2C_GenerateSTART(I2Cx, ENABLE);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(15);
    }
    /* Send EEPROM address for read */
    I2C_Send7bitAddress(I2Cx, slave_adress, I2C_Direction_Receiver);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(16);
    }
    /* While there is data to be read */

    /* Disable Acknowledgement */
    I2C_AcknowledgeConfig(I2Cx, DISABLE);

    /* Send STOP Condition */
    I2C_GenerateSTOP(I2Cx, ENABLE);

    I2CTimeout = I2CT_LONG_TIMEOUT;
    while (I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) == 0)
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(17);
    }

    /* Read a byte from the device */
    *pBuffer = I2C_ReceiveData(I2Cx);

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2Cx, ENABLE);

    return 1;
}