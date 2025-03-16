#include "i2c_init.h"

static void I2C_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能 I2C 外设时钟 */
    // 启用I2C外设的时钟，以便后续的配置能够生效
    RCC_APB1PeriphClockCmd(I2C_CLK, ENABLE);

    /* 使能 I2C 引脚的 GPIO 时钟 */
    RCC_AHB1PeriphClockCmd(I2C_SCL_GPIO_CLK | I2C_SDA_GPIO_CLK, ENABLE);

    /* 连接引脚源 PXx 到 I2C_SCL*/
    GPIO_PinAFConfig(I2C_SCL_GPIO_PORT, I2C_SCL_SOURCE, I2C_SCL_AF);
    /* 连接引脚源 PXx 到 to I2C_SDA*/
    GPIO_PinAFConfig(I2C_SDA_GPIO_PORT, I2C_SDA_SOURCE, I2C_SDA_AF);

    /* 配置 SCL 引脚 */
    GPIO_InitStructure.GPIO_Pin = I2C_SCL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(I2C_SCL_GPIO_PORT, &GPIO_InitStructure);
    /*配置 SDA 引脚 */
    GPIO_InitStructure.GPIO_Pin = I2C_SDA_PIN;

    GPIO_Init(I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
}

static void I2C_Mode_Config(void)
{
    I2C_InitTypeDef I2C_InitStructure = {
        .I2C_Mode = I2C_Mode_I2C,
        .I2C_DutyCycle = I2C_DutyCycle_2,    // 占空比
        .I2C_OwnAddress1 = I2C_OWN_ADDRESS7, // 主机的地址
        .I2C_Ack = I2C_Ack_Enable,
        .I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit, // 地址长度
        .I2C_ClockSpeed = I2C_Speed};
    /* 写入配置 */
    I2C_Init(I2C_Channel, &I2C_InitStructure);
    /* 使能 I2C */
    I2C_Cmd(I2C_Channel, ENABLE);
}

void User_I2C_Init(void)
{
    I2C_GPIO_Config();
    I2C_Mode_Config();
}

static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode)
{
    /* 使用串口 printf 输出错误信息，方便调试 */
    // EEPROM_ERROR("I2C 等待超时!errorCode = %d", errorCode);
    return 0;
}

uint32_t I2C_ByteWrite(u8 slave_adress, u8 *pBuffer, u8 WriteAddr)
{

    uint32_t I2CTimeout;
    /* 产生 I2C 起始信号 */
    I2C_GenerateSTART(I2C_Channel, ENABLE);
    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV5 事件并清除标志 */
    while (!I2C_CheckEvent(I2C_Channel, I2C_EVENT_MASTER_MODE_SELECT))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(0);
    }

    /* 发送设备地址 */
    I2C_Send7bitAddress(I2C_Channel, slave_adress,
                        I2C_Direction_Transmitter);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV6 事件并清除标志 */
    while (!I2C_CheckEvent(I2C_Channel,
                           I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(1);
    }

    /* 发送要写入的寄存器地址*/
    I2C_SendData(I2C_Channel, WriteAddr);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV8 事件并清除标志 */
    while (!I2C_CheckEvent(I2C_Channel,
                           I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(2);
    }
    /* 发送一字节要写入的数据 */
    I2C_SendData(I2C_Channel, *pBuffer);

    I2CTimeout = I2CT_FLAG_TIMEOUT;
    /* 检测 EV8 事件并清除标志 */
    while (!I2C_CheckEvent(I2C_Channel,
                           I2C_EVENT_MASTER_BYTE_TRANSMITTED))
    {
        if ((I2CTimeout--) == 0)
            return I2C_TIMEOUT_UserCallback(3);
    }

    /* 发送停止信号 */
    I2C_GenerateSTOP(I2C_Channel, ENABLE);

    return 1;
}

uint8_t I2C_ByetsWrite(uint8_t slave_adress,uint8_t *pBuffer, uint8_t WriteAddr,uint16_t NumByteToWrite)
{
    uint16_t i;
    uint8_t res;

    /* 每写一个字节调用一次 I2C_EE_ByteWrite 函数 */
    for (i = 0; i < NumByteToWrite; i++)
    {
        /* 等待 EEPROM 准备完毕 */
        // I2C_EE_WaitEepromStandbyState();
        /* 按字节写入数据 */
        res = I2C_ByteWrite(slave_adress,pBuffer++, WriteAddr++);
    }
    return res;
}