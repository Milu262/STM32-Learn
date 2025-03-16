
#include "stm32f4xx.h"

/* STM32 I2C 速率 */
#define I2C_Speed 400000

/* STM32 自身的 I2C 地址，这个地址只要与 STM32 外挂的 I2C 器件地址不一样即可 */
#define I2C_OWN_ADDRESS7 0X0A

/*I2C 接口 */
#define I2C_Channel I2C1
#define I2C_CLK RCC_APB1Periph_I2C1

#define I2C_SCL_PIN GPIO_Pin_8
#define I2C_SCL_GPIO_PORT GPIOB
#define I2C_SCL_GPIO_CLK RCC_AHB1Periph_GPIOB
#define I2C_SCL_SOURCE GPIO_PinSource8
#define I2C_SCL_AF GPIO_AF_I2C1

#define I2C_SDA_PIN GPIO_Pin_9
#define I2C_SDA_GPIO_PORT GPIOB
#define I2C_SDA_GPIO_CLK RCC_AHB1Periph_GPIOB
#define I2C_SDA_SOURCE GPIO_PinSource9
#define I2C_SDA_AF GPIO_AF_I2C1

/* 通讯等待超时时间 */
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

static void I2C_GPIO_Config(void);
static void I2C_Mode_Config(void);
void User_I2C_Init(void);
uint8_t I2C_ByetsWrite(uint8_t slave_adress,uint8_t *pBuffer, uint8_t WriteAddr,uint16_t NumByteToWrite);