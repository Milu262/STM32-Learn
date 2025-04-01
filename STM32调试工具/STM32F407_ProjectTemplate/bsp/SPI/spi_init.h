
#include "stm32f4xx.h"
#include <stdio.h>

// SPI 号及时钟初始化函数
#define SPI SPI1
#define SPI_CLK RCC_APB2Periph_SPI1
#define GPIO_CLK RCC_AHB1Periph_GPIOA

#define GPIO_PORT GPIOA
#define GPIO_AF GPIO_AF_SPI1
// CS(NSS) 引脚
#define CS_PIN GPIO_Pin_4

// SCK 引脚
#define SPI_SCK_PIN GPIO_Pin_5
#define SPI_SCK_PINSOURCE GPIO_PinSource5

// MISO 引脚
#define SPI_MISO_PIN GPIO_Pin_6
#define SPI_MISO_PINSOURCE GPIO_PinSource6

// MOSI 引脚
#define SPI_MOSI_PIN GPIO_Pin_7
#define SPI_MOSI_PINSOURCE GPIO_PinSource7

#define SPI_CS_ON(x) GPIO_WriteBit(GPIO_PORT, CS_PIN, x ? Bit_SET : Bit_RESET)

void SPI_GPIO_Init(void);
void SPI_MODE_INit(void);
void SPI_BUS_Init(void);