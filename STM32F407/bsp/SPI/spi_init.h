
#include "stm32f4xx.h"
#include <stdio.h>

// SPI 屏幕号及时钟初始化函数
#define SPIScreen SPI2
#define SPI_Screen_CLK             RCC_APB1Periph_SPI2

// SCK 引脚
#define SPI_SCK_GPIO_PORT   GPIOB
#define SPI_SCK_GPIO_CLK    RCC_AHB1Periph_GPIOB
#define SPI_SCK_PIN         GPIO_Pin_10
#define SPI_SCK_PINSOURCE   GPIO_PinSource10
#define SPI_SCK_GPIO_AF     GPIO_AF_SPI2

// MOSI 引脚
#define SPI_MOSI_GPIO_PORT  GPIOC
#define SPI_MOSI_GPIO_CLK   RCC_AHB1Periph_GPIOC
#define SPI_MOSI_PIN        GPIO_Pin_3
#define SPI_MOSI_PINSOURCE  GPIO_PinSource3
#define SPI_MOSI_GPIO_AF    GPIO_AF_SPI2

// CS(NSS) 引脚  PE9
#define CS_PIN_GPIO_PORT    GPIOE
#define CS_PIN_GPIO_CLK     RCC_AHB1Periph_GPIOE
#define CS_PIN              GPIO_Pin_9

// SPI FLASH号及时钟初始化函数
#define SPI_FLASH SPI1
#define SPI_FLASH_CLK             RCC_APB2Periph_SPI1

// SCK 引脚
#define SPI_FLASH_SCK_GPIO_PORT   GPIOA
#define SPI_FLASH_SCK_GPIO_CLK    RCC_AHB1Periph_GPIOA
#define SPI_FLASH_SCK_PIN         GPIO_Pin_5
#define SPI_FLASH_SCK_PINSOURCE   GPIO_PinSource5
#define SPI_FLASH_SCK_GPIO_AF     GPIO_AF_SPI1

// MOSI 引脚
#define SPI_FLASH_MOSI_GPIO_PORT  GPIOA
#define SPI_FLASH_MOSI_GPIO_CLK   RCC_AHB1Periph_GPIOA
#define SPI_FLASH_MOSI_PIN        GPIO_Pin_7
#define SPI_FLASH_MOSI_PINSOURCE  GPIO_PinSource7
#define SPI_FLASH_MOSI_GPIO_AF    GPIO_AF_SPI1

//MISO 引脚
#define SPI_FLASH_MISO_GPIO_PORT  GPIOA
#define SPI_FLASH_MISO_GPIO_CLK   RCC_AHB1Periph_GPIOA
#define SPI_FLASH_MISO_PIN        GPIO_Pin_6
#define SPI_FLASH_MISO_PINSOURCE  GPIO_PinSource6
#define SPI_FLASH_MISO_GPIO_AF    GPIO_AF_SPI1

// SPI1 FLASH CS(NSS) 引脚 
#define SPI_FLASH_CS_GPIO_PORT    GPIOA
#define SPI_FLASH_CS_GPIO_CLK     RCC_AHB1Periph_GPIOA
#define SPI_FLASH_CS_PIN          GPIO_Pin_4



#define SPI_CS_ON(x) GPIO_WriteBit(SPI_FLASH_CS_GPIO_PORT, SPI_FLASH_CS_PIN, x ? Bit_SET : Bit_RESET)
#define SPI2_Screen_CS_ON(x) GPIO_WriteBit(CS_PIN_GPIO_PORT, CS_PIN, x ? Bit_SET : Bit_RESET)

static void SPI2_GPIO_Init(void);
static void SPI2_Screen_Init(void);
void SPI2_BUS_Init(void);
static void SPI1_GPIO_Init(void);
static void SPI1_FLASH_Init(void);
void SPI1_BUS_Init(void);