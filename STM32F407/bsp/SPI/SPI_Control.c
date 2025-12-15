#include "SPI_Control.h"
#include "board.h"
#include "spi_init.h"
#include "stdio.h"
#include "string.h"
// 定义一个静态常量数组spiErrors，用于存储所有可能的SPI错误及其对应的错误信息
static const Error_Code spiErrors[] =
    {
        {0, "SPI send buffer is not empty, TXE Event timeout\r\n"},
        {1, "SPI receive buffer is not empty, RXNE Event timeout\r\n"},
        {2, "SPI Flash write operation timeout\r\n"},
        {3, "SPI_FLASH_PageWrite too large!\r\n"},
        {4, "SPI FLASH PageWrite Failed\r\n"},
        {5, "SPI FLASH BufferRead Failed\r\n"}};

// 定义一个宏SPI_ERROR_COUNT，用于计算spiErrors数组中的元素数量
#define SPI_ERROR_COUNT (sizeof(spiErrors) / sizeof(spiErrors[0]))
static uint8_t SPI_TIMEOUT_UserCallback(uint8_t errorCode)
{
  // 查找并打印错误信息（如果找到）
  for (int i = 0; i < SPI_ERROR_COUNT; i++)
  {
    if (spiErrors[i].errorCode == errorCode)
    {
      printf("%s", spiErrors[i].errorMessage);
      break;
    }
  }

  // 拉高片选信号
  SPI_CS_ON(1);

  return 0;
}

uint8_t SPI_Send_receive_Byte(uint8_t send_data, uint8_t *receive_data)
{
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
  if (receive_data != NULL)
  {
    *receive_data = SPI_I2S_ReceiveData(FLASH_SPI);
  }
  else
  {
    (void)SPI_I2S_ReceiveData(FLASH_SPI);
  }
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

void SPI_FLASH_BlockErase32(uint32_t BlockAddr)
{
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();
  SPI_CS_ON(0);
  SPI_Send_receive_Byte(W25X_BlockErase32K, NULL);
  SPI_Send_receive_Byte((BlockAddr & 0xFF0000) >> 16, NULL);
  SPI_Send_receive_Byte((BlockAddr & 0xFF00) >> 8, NULL);
  SPI_Send_receive_Byte(BlockAddr & 0xFF, NULL);
  SPI_CS_ON(1);
  SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_BlockErase64(uint32_t BlockAddr)
{
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();
  SPI_CS_ON(0);
  SPI_Send_receive_Byte(W25X_BlockErase64K, NULL);
  SPI_Send_receive_Byte((BlockAddr & 0xFF0000) >> 16, NULL);
  SPI_Send_receive_Byte((BlockAddr & 0xFF00) >> 8, NULL);
  SPI_Send_receive_Byte(BlockAddr & 0xFF, NULL);
  SPI_CS_ON(1);
  SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_ChipErase(void)
{ 
  SPI_FLASH_WriteEnable();
  SPI_FLASH_WaitForWriteEnd();
  SPI_CS_ON(0);
  SPI_Send_receive_Byte(W25X_ChipErase, NULL);
  SPI_CS_ON(1);
  SPI_FLASH_WaitForWriteEnd();
}

void SPI_FLASH_PageWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t err = 0;
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
  // 如果要写入的字节数超过256，则停止写入
  if (NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
    SPI_TIMEOUT_UserCallback(3);
  }
  // 循环写入数据
  while (NumByteToWrite--)
  {
    err = SPI_Send_receive_Byte(*pBuffer++, NULL);
    if (!err)
    {
      SPI_TIMEOUT_UserCallback(4);
      return;
    }
  }
  // 拉高片选信号，结束SPI通信
  SPI_CS_ON(1);
  // 等待当前写操作完成
  SPI_FLASH_WaitForWriteEnd();
}
// 待加入错误处理
void SPI_FLASH_BufferWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
  uint8_t *pWriteBuffer = pBuffer;
  uint32_t PageAddr = WriteAddr & ~(SPI_FLASH_PerWritePageSize - 1);   // 页起始地址
  uint16_t Offset = WriteAddr - PageAddr;                              // 页内偏移量
  uint16_t BytesToWriteThisPage = SPI_FLASH_PerWritePageSize - Offset; // 本页剩余可写字节数

  // 先写本页剩余部分（如果有）
  if (NumByteToWrite >= BytesToWriteThisPage)
  {
    SPI_FLASH_PageWrite(pWriteBuffer, PageAddr, BytesToWriteThisPage);
    pWriteBuffer += BytesToWriteThisPage;
    NumByteToWrite -= BytesToWriteThisPage;
    PageAddr += SPI_FLASH_PerWritePageSize; // 跳到下一页
  }
  else
  {
    // 如果NumByteToWrite小于本页剩余可写字节数，则只写NumByteToWrite个字节
    SPI_FLASH_PageWrite(pWriteBuffer, PageAddr + Offset, NumByteToWrite);
    NumByteToWrite = 0; // 已经没有更多字节需要写了
  }

  // 再写完整页（如果有）
  while (NumByteToWrite >= SPI_FLASH_PerWritePageSize)
  {
    SPI_FLASH_PageWrite(pWriteBuffer, PageAddr, SPI_FLASH_PerWritePageSize);
    pWriteBuffer += SPI_FLASH_PerWritePageSize;
    NumByteToWrite -= SPI_FLASH_PerWritePageSize;
    PageAddr += SPI_FLASH_PerWritePageSize; // 跳到下一页
  }

  // 最后写剩余不足一页的部分（如果有）
  if (NumByteToWrite > 0)
  {
    SPI_FLASH_PageWrite(pWriteBuffer, PageAddr, NumByteToWrite);
  }
}

void SPI_FLASH_BufferRead(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
  uint8_t err = 0;
  SPI_CS_ON(0); // 拉低片选信号
  SPI_Send_receive_Byte(W25X_ReadData, NULL);

  // 发送写入地址的高8位
  SPI_Send_receive_Byte((ReadAddr & 0xFF0000) >> 16, NULL);
  // 发送写入地址的中8位
  SPI_Send_receive_Byte((ReadAddr & 0xFF00) >> 8, NULL);
  // 发送写入地址的低8位
  SPI_Send_receive_Byte(ReadAddr & 0xFF, NULL);
  while (NumByteToRead--)
  {
    err = SPI_Send_receive_Byte(0x00, pBuffer);
    if (!err)
    {
      SPI_TIMEOUT_UserCallback(5);
      return;
    }
    pBuffer++;
  }
  SPI_CS_ON(1); // 拉高片选信号
}
