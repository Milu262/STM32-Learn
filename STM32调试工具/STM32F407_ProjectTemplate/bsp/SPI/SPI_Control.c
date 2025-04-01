#include "SPI_Control.h"

static uint8_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  switch (errorCode)
  {
  case 0:
    printf("SPI 发送缓冲区为空，TXE 事件超时\r\n");
    break;
  case 1:
    printf("SPI 接收缓冲区非空，RXNE 事件超时\r\n");
    break;

  default:
    break;
  }
  SPI_CS_ON(1); // 拉高片选信号
  return 0;
}

uint8_t SPI_Send_receive_Byte(uint8_t send_data, uint8_t *receive_data)
{
  //SPI_CS_ON(0); // 拉低片选信号
  uint16_t spiTimeoutCounter = SPIT_FLAG_TIMEOUT;
  /* 等待发送缓冲区为空，TXE 事件 */
  while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
    if ((spiTimeoutCounter--) == 0)
      return SPI_TIMEOUT_UserCallback(0);
  }
  /* 写入数据寄存器，把要写入的数据写入发送缓冲区 */
  SPI_I2S_SendData(FLASH_SPI, send_data);

  spiTimeoutCounter = SPIT_FLAG_TIMEOUT;

  /* 等待接收缓冲区非空，RXNE 事件 */
  while (SPI_I2S_GetFlagStatus(FLASH_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
    if ((spiTimeoutCounter--) == 0)
      return SPI_TIMEOUT_UserCallback(1);
  }

  /* 读取数据寄存器，获取接收缓冲区数据 */
  // return SPI_I2S_ReceiveData(FLASH_SPI);
  if(receive_data != NULL)
  {
    *receive_data = SPI_I2S_ReceiveData(FLASH_SPI);
  }
  else
  {
    SPI_I2S_ReceiveData(FLASH_SPI);
  }
  //SPI_CS_ON(1); // 拉高片选信号
  return 1;
}

uint32_t SPI_FLASH_ReadID(void)
{
  uint32_t id = 0;
  uint8_t temp[3] = {0};
  SPI_CS_ON(0); // 拉低片选信号
  SPI_Send_receive_Byte(W25X_JedecDeviceID, NULL);

  SPI_Send_receive_Byte(0x00, temp);

  SPI_Send_receive_Byte(0x00, temp+1);

  SPI_Send_receive_Byte(0x00, temp+2);

  SPI_CS_ON(1); // 拉高片选信号
  id = (temp[0] << 16) | (temp[1] << 8) | temp[2];
  return id;
}
