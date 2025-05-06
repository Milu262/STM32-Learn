
#include "spi_init.h"

static void SPI_Screen_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* 使能GPIO 时钟 */
  RCC_AHB1PeriphClockCmd(SPI_SCK_GPIO_CLK | SPI_MOSI_GPIO_CLK | CS_PIN_GPIO_CLK, ENABLE);

  // 设置引脚复用
  GPIO_PinAFConfig(SPI_SCK_GPIO_PORT, SPI_SCK_PINSOURCE, SPI_SCK_GPIO_AF);
  // GPIO_PinAFConfig(GPIO_PORT, SPI_MISO_PINSOURCE, GPIO_AF);
  GPIO_PinAFConfig(SPI_MOSI_GPIO_PORT, SPI_MOSI_PINSOURCE, SPI_MOSI_GPIO_AF);

  /*!< 配置 SPI2 引脚: SCK */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /*!< 配置 SPI2 引脚: MOSI */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /*!< 配置 SPI2 引脚: CS */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = CS_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(CS_PIN_GPIO_PORT, &GPIO_InitStructure);

  /* 停止信号 Screen: CS 引脚高电平 */
  SPI2_Screen_CS_ON(1);
}

static void SPI_Screen_Init(void)
{
  /*!< SPI时钟使能 */
  // 只有使用SPI1、SPI4、SPI5、SPI6时，需要使能APB2时钟
  if (SPIScreen == SPI1 || SPIScreen == SPI4 || SPIScreen == SPI5 || SPIScreen == SPI6)
    RCC_APB2PeriphClockCmd(SPI_Screen_CLK, ENABLE);
  else // 否则则需要使能APB1时钟
    RCC_APB1PeriphClockCmd(SPI_Screen_CLK, ENABLE);

  SPI_InitTypeDef SPI_InitStructure =
      {
         .SPI_Direction = SPI_Direction_1Line_Tx, // 设置SPI的数据方向为仅发送数据
          // .SPI_Direction = SPI_Direction_2Lines_FullDuplex, // 设置SPI的数据方向为全双工但是仅发送数据
          .SPI_Mode = SPI_Mode_Master,                      // 设置SPI模式为主模式
          .SPI_DataSize = SPI_DataSize_8b,                  // 设置数据大小为8位
          .SPI_CPOL = SPI_CPOL_High,                        // 设置时钟极性为高电平
          .SPI_CPHA = SPI_CPHA_2Edge,                       // 设置时钟相位为第二个边沿
          .SPI_NSS = SPI_NSS_Soft,                          // 设置NSS信号为软件管理
          .SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2, // 设置波特率预分频值为2，即SPI时钟频率为主时钟频率的1/2
          .SPI_FirstBit = SPI_FirstBit_MSB,                 // 设置数据传输时先发送最高位
          // .SPI_FirstBit = SPI_FirstBit_LSB,                 // 设置数据传输时先发送最高位
          .SPI_CRCPolynomial = 7};                          // 设置CRC多项式为7，用于CRC校验
  SPI_Init(SPIScreen, &SPI_InitStructure);
  SPI_Cmd(SPIScreen, ENABLE);
}
static void SPI_FLASH_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(SPI_FLASH_SCK_GPIO_CLK | SPI_FLASH_MOSI_GPIO_CLK | SPI_FLASH_MISO_GPIO_CLK | SPI_FLASH_CS_GPIO_CLK, ENABLE);
  // 设置引脚复用
  GPIO_PinAFConfig(SPI_FLASH_SCK_GPIO_PORT, SPI_FLASH_SCK_PINSOURCE, SPI_FLASH_SCK_GPIO_AF);
  GPIO_PinAFConfig(SPI_FLASH_MOSI_GPIO_PORT, SPI_FLASH_MOSI_PINSOURCE, SPI_FLASH_MOSI_GPIO_AF);
  GPIO_PinAFConfig(SPI_FLASH_MISO_GPIO_PORT, SPI_FLASH_MISO_PINSOURCE, SPI_FLASH_MISO_GPIO_AF);

  /*!< 配置 SPI_FLASH_SPI 引脚: SCK */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SPI_FLASH_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(SPI_FLASH_SCK_GPIO_PORT, &GPIO_InitStructure);
  /*!< 配置 SPI_FLASH_SPI 引脚: MOSI */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SPI_FLASH_MOSI_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(SPI_FLASH_MOSI_GPIO_PORT, &GPIO_InitStructure);
  /*!< 配置 SPI_FLASH_SPI 引脚: MISO */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SPI_FLASH_MISO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(SPI_FLASH_MISO_GPIO_PORT, &GPIO_InitStructure);
  /*!< 配置 SPI_FLASH_SPI 引脚: CS */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SPI_FLASH_CS_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(SPI_FLASH_CS_GPIO_PORT, &GPIO_InitStructure);

  /* 停止信号 FLASH: CS 引脚高电平 */
  SPI_CS_ON(1);
}
static void SPI_FLASH_Init(void)
{
  /*!< SPI时钟使能 */
  // 只有使用SPI1、SPI4、SPI5、SPI6时，需要使能APB2时钟
  if (SPI_FLASH == SPI1 || SPI_FLASH == SPI4 || SPI_FLASH == SPI5 || SPI_FLASH == SPI6)
    RCC_APB2PeriphClockCmd(SPI_FLASH_CLK, ENABLE);
  else // 否则则需要使能APB1时钟
    RCC_APB1PeriphClockCmd(SPI_FLASH_CLK, ENABLE);

  SPI_InitTypeDef SPI_InitStructure =
      {
          .SPI_Direction = SPI_Direction_2Lines_FullDuplex, // 设置SPI的数据方向为全双工模式
          .SPI_Mode = SPI_Mode_Master,                      // 设置SPI模式为主模式
          .SPI_DataSize = SPI_DataSize_8b,                  // 设置数据大小为8位
          .SPI_CPOL = SPI_CPOL_High,                        // 设置时钟极性为高电平
          .SPI_CPHA = SPI_CPHA_2Edge,                       // 设置时钟相位为第二个边沿
          .SPI_NSS = SPI_NSS_Soft,                          // 设置NSS信号为软件管理
          .SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2, // 设置波特率预分频值为2，即SPI时钟频率为主时钟频率的1/2
          .SPI_FirstBit = SPI_FirstBit_MSB,                 // 设置数据传输时先发送最高位
          .SPI_CRCPolynomial = 7                            // 设置CRC多项式为7，用于CRC校验
      };
  SPI_Init(SPI_FLASH, &SPI_InitStructure);
  SPI_Cmd(SPI_FLASH, ENABLE);
}
void SPI_FLASH_BUS_Init(void)
{
  SPI_FLASH_GPIO_Init();
  SPI_FLASH_Init();
}
void SPI_Screen_BUS_Init(void)
{
  SPI_Screen_GPIO_Init();
  SPI_Screen_Init();
}
