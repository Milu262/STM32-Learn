#include "i2c_handle.h"

static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
    /* 使用串口 printf 输出错误信息，方便调试 */
    // EEPROM_ERROR("I2C 等待超时!errorCode = %d", errorCode);
    switch (errorCode)
    {
    case 0:
        printf("I2C start signal transmission failed!\r\n");
        break;
    case 1:
        printf("I2C Device address No response!\r\n");
        break;
    case 2:
        printf("I2C Write the wrong register address!\r\n");
        break;
    case 3:
        printf("I2C Write data error!\r\n");
        break;
    case 4:
        printf("I2C send read register address error!\r\n");
        break;
    case 5:
        printf("I2C read data error!\r\n");
        break;
    case 10:
        printf("I2C bus is busy!\r\n");
        break;
    default:
        break;
    }
    I2C_GenerateSTOP(I2C1, ENABLE);
    return 0;
}

uint32_t I2C_ByteWrite(uint8_t slave_adress, uint8_t WriteAddr, uint8_t pBuffer)
{

    uint32_t I2CTimeout;
    /* 产生 I2C 起始信号 */
    I2C_GenerateSTART(I2C1, ENABLE);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV5 事件并清除标志 */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }

    /* 发送设备地址 */
    I2C_Send7bitAddress(I2C1, slave_adress, I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV6 事件并清除标志 */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(1);
    }
    /* 发送要写入的寄存器地址*/
    I2C_SendData(I2C1, WriteAddr);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV8 事件并清除标志 */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(2);
    }
    /* 发送一字节要写入的数据 */
    I2C_SendData(I2C1, pBuffer);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV8 事件并清除标志 */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(3);
    }

    /* 发送停止信号 */
    I2C_GenerateSTOP(I2C1, ENABLE);

    return 1;
}

uint32_t I2C_BufferWrite(uint8_t slave_adress, uint8_t WriteAddr, uint8_t *pBuffer, uint16_t NumByteToWrite)
{
    uint16_t i;
    uint8_t res;

    /* 每写一个字节调用一次 I2C_EE_ByteWrite 函数 */
    for (i = 0; i < NumByteToWrite; i++)
    {
        /* 等待 EEPROM 准备完毕 */
        // I2C_EE_WaitEepromStandbyState();
        /* 按字节写入数据 */
        res = I2C_ByteWrite(slave_adress, WriteAddr++, *pBuffer++);
        if (res == 0)
        {
            printf("I2C 8bit Register Address Buffer write error!\r\n");
            return 0;
        }
    }
    return res;
}

uint32_t I2C_BufferRead(uint8_t slave_adress, uint8_t ReadAddr, uint8_t *pBuffer, uint16_t NumByteToRead)
{
    uint32_t I2CTimeout = I2CT_LONG_TIMEOUT;

    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(10);
    }
    /* Send START condition */
    I2C_GenerateSTART(I2C1, ENABLE);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C1, slave_adress, I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(1);
    }
    /* Clear EV6 by setting again the PE bit */
    I2C_Cmd(I2C1, ENABLE);

    /* Send the EEPROM's internal address to write to */
    I2C_SendData(I2C1, ReadAddr);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(4);
    }
    /* Send STRAT condition a second time */
    I2C_GenerateSTART(I2C1, ENABLE);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }
    /* Send EEPROM address for read */
    I2C_Send7bitAddress(I2C1, slave_adress, I2C_Direction_Receiver);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(1);
    }
    /* While there is data to be read */
    while (NumByteToRead)
    {
        if (NumByteToRead == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(I2C1, DISABLE);

            /* Send STOP Condition */
            I2C_GenerateSTOP(I2C1, ENABLE);
        }

        I2CTimeout = I2CT_LONG_TIMEOUT;
        while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == 0)
        {
            if ((I2CTimeout--) == 0)
                return I2C_TIMEOUT_UserCallback(5);
        }
        {
            /* Read a byte from the device */
            *pBuffer = I2C_ReceiveData(I2C1);

            /* Point to the next location where the byte read will be saved */
            pBuffer++;

            /* Decrement the read bytes counter */
            NumByteToRead--;
        }
    }

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    return 1;
}

uint32_t I2C_byteRead(uint8_t slave_adress, uint8_t ReadAddr, uint8_t *pBuffer)
{
    uint32_t I2CTimeout = I2CT_LONG_TIMEOUT;

    //*((u8 *)0x4001080c) |=0x80;
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(10);
    }
    /* Send START condition */
    I2C_GenerateSTART(I2C1, ENABLE);
    //*((u8 *)0x4001080c) &=~0x80;

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C1, slave_adress, I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
        {
            return I2C_TIMEOUT_UserCallback(1);
        }
    }
    // printf("I2C1 EV6\n");
    /* Clear EV6 by setting again the PE bit */
    I2C_Cmd(I2C1, ENABLE);

    /* Send the EEPROM's internal address to write to */
    I2C_SendData(I2C1, ReadAddr);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(4);
    }
    /* Send STRAT condition a second time */
    I2C_GenerateSTART(I2C1, ENABLE);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }
    /* Send EEPROM address for read */
    I2C_Send7bitAddress(I2C1, slave_adress, I2C_Direction_Receiver);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(1);
    }
    /* While there is data to be read */

    /* Disable Acknowledgement */
    I2C_AcknowledgeConfig(I2C1, DISABLE);

    /* Send STOP Condition */
    I2C_GenerateSTOP(I2C1, ENABLE);

    I2CTimeout = I2CT_LONG_TIMEOUT;
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == 0)
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(5);
    }

    /* Read a byte from the device */
    *pBuffer = I2C_ReceiveData(I2C1);

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    return 1;
}

uint32_t I2C_Write_16addr(uint8_t slave_adress, uint16_t WriteAddr, uint8_t pBuffer)
{

    uint32_t I2CTimeout;
    /* 产生 I2C 起始信号 */
    I2C_GenerateSTART(I2C1, ENABLE);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV5 事件并清除标志 */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }

    /* 发送设备地址 */
    I2C_Send7bitAddress(I2C1, slave_adress, I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV6 事件并清除标志 */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(1);
    }

    /* 发送要写入的寄存器地址*/
    I2C_SendData(I2C1, WriteAddr >> 8);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV8 事件并清除标志 */
    while (!I2C_CheckEvent(I2C1,
                           I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(2);
    }

    I2C_SendData(I2C1, WriteAddr);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV8 事件并清除标志 */
    while (!I2C_CheckEvent(I2C1,
                           I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(2);
    }
    /* 发送一字节要写入的数据 */
    I2C_SendData(I2C1, pBuffer);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV8 事件并清除标志 */
    while (!I2C_CheckEvent(I2C1,
                           I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(3);
    }

    /* 发送停止信号 */
    I2C_GenerateSTOP(I2C1, ENABLE);

    return 1;
}

uint32_t I2C_Read16addr(uint8_t slave_adress, uint16_t ReadAddr, uint8_t *pBuffer)
{
    uint32_t I2CTimeout = I2CT_LONG_TIMEOUT;

    //*((u8 *)0x4001080c) |=0x80;
    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(10);
    }
    /* Send START condition */
    I2C_GenerateSTART(I2C1, ENABLE);
    //*((u8 *)0x4001080c) &=~0x80;

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C1, slave_adress, I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(1);
    }
    /* Clear EV6 by setting again the PE bit */
    I2C_Cmd(I2C1, ENABLE);

    /* Send the EEPROM's internal address to write to */
    I2C_SendData(I2C1, ReadAddr >> 8);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(4);
    }

    I2C_SendData(I2C1, ReadAddr);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(4);
    }
    /* Send STRAT condition a second time */
    I2C_GenerateSTART(I2C1, ENABLE);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }
    /* Send EEPROM address for read */
    I2C_Send7bitAddress(I2C1, slave_adress, I2C_Direction_Receiver);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(1);
    }
    /* While there is data to be read */

    /* Disable Acknowledgement */
    I2C_AcknowledgeConfig(I2C1, DISABLE);

    /* Send STOP Condition */
    I2C_GenerateSTOP(I2C1, ENABLE);

    I2CTimeout = I2CT_LONG_TIMEOUT;
    while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == 0)
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(5);
    }

    /* Read a byte from the device */
    *pBuffer = I2C_ReceiveData(I2C1);

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    return 1;
}

uint32_t I2C_BufferWrite_16addr(uint8_t slave_adress, uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumByteToWrite)
{
    uint16_t i;
    uint8_t res;

    /* 每写一个字节调用一次 I2C_EE_ByteWrite 函数 */
    for (i = 0; i < NumByteToWrite; i++)
    {
        /* 等待 EEPROM 准备完毕 */
        // I2C_EE_WaitEepromStandbyState();
        /* 按字节写入数据 */
        res = I2C_Write_16addr(slave_adress, WriteAddr++, *pBuffer++);
        if (res == 0)
        {
            printf("I2C 16bit Register Address Buffer write error!\r\n");
            return 0;
        }
    }
    return res;
}

uint32_t I2C_BufferRead_16addr(uint8_t slave_adress, uint16_t ReadAddr, uint8_t *pBuffer, uint16_t NumByteToRead)
{
    uint32_t I2CTimeout = I2CT_LONG_TIMEOUT;

    while (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(10);
    }
    /* Send START condition */
    I2C_GenerateSTART(I2C1, ENABLE);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }

    /* Send EEPROM address for write */
    I2C_Send7bitAddress(I2C1, slave_adress, I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(1);
    }
    I2C_Cmd(I2C1, ENABLE);

    /* Send the EEPROM's internal address to write to */
    I2C_SendData(I2C1, ReadAddr >> 8);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(4);
    }

    I2C_SendData(I2C1, ReadAddr);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV8 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(4);
    }
    /* Send STRAT condition a second time */
    I2C_GenerateSTART(I2C1, ENABLE);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV5 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }
    /* Send EEPROM address for read */
    I2C_Send7bitAddress(I2C1, slave_adress, I2C_Direction_Receiver);

    I2CTimeout = I2CT_FLAG_TIMEOUT;

    /* Test on EV6 and clear it */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(1);
    }
    /* While there is data to be read */
    while (NumByteToRead)
    {
        if (NumByteToRead == 1)
        {
            /* Disable Acknowledgement */
            I2C_AcknowledgeConfig(I2C1, DISABLE);

            /* Send STOP Condition */
            I2C_GenerateSTOP(I2C1, ENABLE);
        }

        I2CTimeout = I2CT_LONG_TIMEOUT;
        while (I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED) == 0)
        {
            if ((I2CTimeout--) == 0)
                return I2C_TIMEOUT_UserCallback(5);
        }
        {
            /* Read a byte from the device */
            *pBuffer = I2C_ReceiveData(I2C1);

            /* Point to the next location where the byte read will be saved */
            pBuffer++;

            /* Decrement the read bytes counter */
            NumByteToRead--;
        }
    }

    /* Enable Acknowledgement to be ready for another reception */
    I2C_AcknowledgeConfig(I2C1, ENABLE);

    return 1;
}

void Find_i2c_device(void)
{
    uint32_t i2c_device = 0;

    for (uint8_t i = 0x01; i < 0x80; i++)
    {
        if (i2c_device_adress_find(i << 1))
        {
            printf("find i2c device address: 0x%x\r\n", i);
            i2c_device++;
        }
    }
    if (!i2c_device)
    {
        printf("No device is connected to the I2C bus!\r\n");
    }
}

static uint8_t i2c_device_adress_find(uint8_t slave_adress)
{
    uint32_t I2CTimeout;
    /* 产生 I2C 起始信号 */
    I2C_GenerateSTART(I2C1, ENABLE);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV5 事件并清除标志 */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }

    /* 发送设备地址 */
    I2C_Send7bitAddress(I2C1, slave_adress, I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV6 事件并清除标志 */
    while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
        {
            I2C_GenerateSTOP(I2C1, ENABLE);
            return 0;
        }
    }
    I2C_GenerateSTOP(I2C1, ENABLE);
    return 1;
}

void I2C_EE_WaitEepromStandbyState(uint8_t EEPROM_ADDRESS)
{
    vu16 SR1_Tmp = 0;

    do
    {
        /* Send START condition */
        I2C_GenerateSTART(I2C1, ENABLE);
        /* Read EEPROM_I2C SR1 register */
        SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
        /* Send EEPROM address for write */
        I2C_Send7bitAddress(I2C1, EEPROM_ADDRESS, I2C_Direction_Transmitter);
    } while (!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));

    /* Clear AF flag */
    I2C_ClearFlag(I2C1, I2C_FLAG_AF);
    /* STOP condition */
    I2C_GenerateSTOP(I2C1, ENABLE);
}
