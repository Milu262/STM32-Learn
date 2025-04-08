#include "SPI_Control.h"

static uint8_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  switch (errorCode)
  {
  case 0:
    printf("SPI send buffer is not empty,TXE Event timeout\r\n");
    break;
  case 1:
    printf("SPI receive buffer is not empty,RXNE Event timeout\r\n");
    break;
  case 2:
    printf("SPI Flash write operation timeout\r\n");
    break;
  case 3:
    printf("SPI_FLASH_PageWrite too large!\r\n");
    break;
  default:
    break;
  }
  SPI_CS_ON(1); // 拉高片选信号
  return 0;
}

uint8_t SPI_Send_receive_Byte(uint8_t send_data, uint8_t *receive_data)
{
  // SPI_CS_ON(0); // 拉低片选信号
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
  if (receive_data != NULL)
  {
    *receive_data = SPI_I2S_ReceiveData(FLASH_SPI);
  }
  else
  {
    SPI_I2S_ReceiveData(FLASH_SPI);
  }
  // SPI_CS_ON(1); // 拉高片选信号
  return 1;
}

uint32_t SPI_FLASH_ReadID(void)
{
  uint32_t id = 0;
  uint8_t temp[3] = {0};
  SPI_CS_ON(0); // 拉低片选信号
  SPI_Send_receive_Byte(W25X_JedecDeviceID, NULL);

  SPI_Send_receive_Byte(0x00, temp);

  SPI_Send_receive_Byte(0x00, temp + 1);

  SPI_Send_receive_Byte(0x00, temp + 2);

  SPI_CS_ON(1); // 拉高片选信号
  id = (temp[0] << 16) | (temp[1] << 8) | temp[2];
  return id;
}

void SPI_FLASH_WriteEnable(void)
{
  SPI_CS_ON(0); // 拉低片选信号
  SPI_Send_receive_Byte(W25X_WriteEnable, NULL);
  SPI_CS_ON(1); // 拉高片选信号
}

void SPI_FLASH_WaitForWriteEnd(void)
{
  uint8_t status = 0;
  SPI_CS_ON(0); // 拉低片选信号
  SPI_Send_receive_Byte(W25X_ReadStatusReg, NULL);
  uint16_t spiTimeoutCounter = SPIT_FLAG_TIMEOUT;
  do
  {
    SPI_Send_receive_Byte(0x00, &status);
    if ((spiTimeoutCounter--) == 0)
    {
      SPI_TIMEOUT_UserCallback(2);
      return;
    }
  } while ((status & 0x01) == SET);
  SPI_CS_ON(1); // 拉高片选信号
}

void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
  // 启用SPI Flash写操作
  SPI_FLASH_WriteEnable();
  // 等待上一个写操作完成
  SPI_FLASH_WaitForWriteEnd();
  // 拉低片选信号，开始SPI通信
  SPI_CS_ON(0);
  // 发送扇区擦除命令W25X_SectorErase
  SPI_Send_receive_Byte(W25X_SectorErase, NULL);
  // 发送扇区地址的高8位
  SPI_Send_receive_Byte((SectorAddr & 0xFF0000) >> 16, NULL);
  // 发送扇区地址的中8位
  SPI_Send_receive_Byte((SectorAddr & 0xFF00) >> 8, NULL);
  // 发送扇区地址的低8位
  SPI_Send_receive_Byte(SectorAddr & 0xFF, NULL);
  // 拉高片选信号，结束SPI通信
  SPI_CS_ON(1); // 拉高片选信号
  // 等待当前写操作完成
  SPI_FLASH_WaitForWriteEnd();
}


void SPI_FLASH_PageWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  // 启用SPI Flash写操作
  SPI_FLASH_WriteEnable();
  // 等待上一个写操作完成
  SPI_FLASH_WaitForWriteEnd();
  // 拉低片选信号，开始SPI通信
  SPI_CS_ON(0); 
  // 发送页写入命令W25X_PageProgram
  SPI_Send_receive_Byte(W25X_PageProgram, NULL);
  // 发送写入地址的高8位
  SPI_Send_receive_Byte((WriteAddr & 0xFF0000) >> 16, NULL);
  // 发送写入地址的中8位
  SPI_Send_receive_Byte((WriteAddr & 0xFF00) >> 8, NULL);
  // 发送写入地址的低8位
  SPI_Send_receive_Byte(WriteAddr & 0xFF, NULL);
  // 如果要写入的字节数超过256，则设置为256
  if (NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
    NumByteToWrite = SPI_FLASH_PerWritePageSize;
    SPI_TIMEOUT_UserCallback(3);
  }
  // 循环写入数据
  while (NumByteToWrite--)
  {
    SPI_Send_receive_Byte(*pBuffer++, NULL);
  }
  // 拉高片选信号，结束SPI通信
  SPI_CS_ON(1); 
  // 等待当前写操作完成
  SPI_FLASH_WaitForWriteEnd();
}
