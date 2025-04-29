
#include "stm32f4xx.h"
#include <stdio.h>

//SPI1有许多管脚与DCMI复用，所以不使用SPI1

/**
 * @brief SPI3 使用的管脚定义
 * - SCK:  PC10
 * - MOSI: PC12
 * - MISO: PC11 
 * - CS(NSS): PD0
 */

 /**
 * @brief SPI1 使用的管脚定义
 * - SCK:  PA5
 * - MOSI: PA7
 * - CS(NSS): PE9
 */

// SPI 屏幕号及时钟初始化函数
#define SPIScreen SPI1
#define SPI_Screen_CLK             RCC_APB2Periph_SPI1

// SCK 引脚
#define SPI_SCK_GPIO_PORT   GPIOA
#define SPI_SCK_GPIO_CLK    RCC_AHB1Periph_GPIOA
#define SPI_SCK_PIN         GPIO_Pin_5
#define SPI_SCK_PINSOURCE   GPIO_PinSource5
#define SPI_SCK_GPIO_AF     GPIO_AF_SPI1

// MOSI 引脚
#define SPI_MOSI_GPIO_PORT  GPIOA
#define SPI_MOSI_GPIO_CLK   RCC_AHB1Periph_GPIOA
#define SPI_MOSI_PIN        GPIO_Pin_7
#define SPI_MOSI_PINSOURCE  GPIO_PinSource7
#define SPI_MOSI_GPIO_AF    GPIO_AF_SPI1

// CS(NSS) 引脚  PE9
#define CS_PIN_GPIO_PORT    GPIOE
#define CS_PIN_GPIO_CLK     RCC_AHB1Periph_GPIOE
#define CS_PIN              GPIO_Pin_9

// SPI FLASH号及时钟初始化函数
#define SPI_FLASH                   SPI3
#define SPI_FLASH_CLK             RCC_APB1Periph_SPI3

// SCK 引脚
#define SPI_FLASH_SCK_GPIO_PORT   GPIOC
#define SPI_FLASH_SCK_GPIO_CLK    RCC_AHB1Periph_GPIOC
#define SPI_FLASH_SCK_PIN         GPIO_Pin_10
#define SPI_FLASH_SCK_PINSOURCE   GPIO_PinSource10
#define SPI_FLASH_SCK_GPIO_AF     GPIO_AF_SPI3

// MOSI 引脚
#define SPI_FLASH_MOSI_GPIO_PORT  GPIOC
#define SPI_FLASH_MOSI_GPIO_CLK   RCC_AHB1Periph_GPIOC
#define SPI_FLASH_MOSI_PIN        GPIO_Pin_12
#define SPI_FLASH_MOSI_PINSOURCE  GPIO_PinSource12
#define SPI_FLASH_MOSI_GPIO_AF    GPIO_AF_SPI3

//MISO 引脚
#define SPI_FLASH_MISO_GPIO_PORT  GPIOC
#define SPI_FLASH_MISO_GPIO_CLK   RCC_AHB1Periph_GPIOC
#define SPI_FLASH_MISO_PIN        GPIO_Pin_11
#define SPI_FLASH_MISO_PINSOURCE  GPIO_PinSource11
#define SPI_FLASH_MISO_GPIO_AF    GPIO_AF_SPI3

// SPI1 FLASH CS(NSS) 引脚 
#define SPI_FLASH_CS_GPIO_PORT    GPIOD
#define SPI_FLASH_CS_GPIO_CLK     RCC_AHB1Periph_GPIOD
#define SPI_FLASH_CS_PIN          GPIO_Pin_0



#define SPI_CS_ON(x) GPIO_WriteBit(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_PIN, x ? Bit_SET : Bit_RESET)
#define SPI2_Screen_CS_ON(x) GPIO_WriteBit(CS_PIN_GPIO_PORT, CS_PIN, x ? Bit_SET : Bit_RESET)

static void SPI_Screen_GPIO_Init(void);
static void SPI_Screen_Init(void);
void SPI_Screen_BUS_Init(void);
static void SPI_FLASH_GPIO_Init(void);
static void SPI_FLASH_Init(void);
void SPI_FLASH_BUS_Init(void);