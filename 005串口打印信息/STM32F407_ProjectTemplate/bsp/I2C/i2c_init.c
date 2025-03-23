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
    //GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
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
        //.I2C_Ack = I2C_Ack_Disable,
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

