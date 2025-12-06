#ifndef __I2C_INIT_H__
#define __I2C_INIT_H__
#include "stm32f4xx.h"

/* STM32 I2C 速率 */
#define I2C_Speed 400000

/* STM32 自身的 I2C 地址，这个地址只要与 STM32 外挂的 I2C 器件地址不一样即可 */
#define I2C_OWN_ADDRESS7 0X0A

/*I2C 接口 */
#define I2C_Channel         I2C1
#define I2C_CLK             RCC_APB1Periph_I2C1

#define I2C_SCL_PIN         GPIO_Pin_8
#define I2C_SCL_GPIO_PORT   GPIOB
#define I2C_SCL_GPIO_CLK    RCC_AHB1Periph_GPIOB
#define I2C_SCL_SOURCE      GPIO_PinSource8
#define I2C_SCL_AF          GPIO_AF_I2C1

#define I2C_SDA_PIN         GPIO_Pin_9
#define I2C_SDA_GPIO_PORT   GPIOB
#define I2C_SDA_GPIO_CLK    RCC_AHB1Periph_GPIOB
#define I2C_SDA_SOURCE      GPIO_PinSource9
#define I2C_SDA_AF          GPIO_AF_I2C1

/**
 * @brief 用户 I2C 初始化函数
 * @note 初始化 I2C 接口，包括 GPIO 配置、I2C 模式配置、I2C 时钟使能
 * @param 无
 * @retval 无
 */
void User_I2C_Init(void);
#endif