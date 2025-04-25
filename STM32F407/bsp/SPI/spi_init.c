
#include "spi_init.h"

void SPI_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* 使能GPIO 时钟 */
  RCC_AHB1PeriphClockCmd(GPIO_CLK, ENABLE);

  /*!< SPI时钟使能 */
  RCC_APB2PeriphClockCmd(SPI_CLK, ENABLE);

  // 设置引脚复用
  GPIO_PinAFConfig(GPIO_PORT, SPI_SCK_PINSOURCE, GPIO_AF);
  GPIO_PinAFConfig(GPIO_PORT, SPI_MISO_PINSOURCE, GPIO_AF);
  GPIO_PinAFConfig(GPIO_PORT, SPI_MOSI_PINSOURCE, GPIO_AF);

  /*!< 配置 SPI_SPI 引脚: SCK */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_PORT, &GPIO_InitStructure);

  /*!< 配置 SPI_FLASH_SPI 引脚: MISO */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_PORT, &GPIO_InitStructure);

  /*!< 配置 SPI_FLASH_SPI 引脚: MOSI */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = SPI_MOSI_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_PORT, &GPIO_InitStructure);

  /*!< 配置 SPI_FLASH_SPI 引脚: CS */
  GPIO_StructInit(&GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = CS_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIO_PORT, &GPIO_InitStructure);

  /* 停止信号 FLASH: CS 引脚高电平 */
  SPI_CS_ON(1);
}

void SPI_MODE_INit(void)
{
  SPI_InitTypeDef SPI_InitStructure =
      {
          .SPI_Direction = SPI_Direction_2Lines_FullDuplex,  // 设置SPI方向为全双工，即两线全双工模式
          .SPI_Mode = SPI_Mode_Master,                       // 设置SPI模式为主模式
          .SPI_DataSize = SPI_DataSize_8b,                   // 设置数据大小为8位
          .SPI_CPOL = SPI_CPOL_High,                         // 设置时钟极性为高电平
          .SPI_CPHA = SPI_CPHA_2Edge,                        // 设置时钟相位为第二个边沿
          .SPI_NSS = SPI_NSS_Soft,                           // 设置NSS信号为软件管理
          .SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2,  // 设置波特率预分频值为2，即SPI时钟频率为主时钟频率的1/2
          .SPI_FirstBit = SPI_FirstBit_MSB,                  // 设置数据传输时先发送最高位
          .SPI_CRCPolynomial = 7};                           // 设置CRC多项式为7，用于CRC校验
  SPI_Init(SPI, &SPI_InitStructure);
  SPI_Cmd(SPI, ENABLE);
}

void SPI_BUS_Init(void)
{
  SPI_GPIO_Init();
  SPI_MODE_INit();
}

