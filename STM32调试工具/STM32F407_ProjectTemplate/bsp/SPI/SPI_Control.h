
#include "stm32f4xx.h"
// #include <stdio.h>
#include "spi_init.h"

#define Dummy_Byte 0xFF
#define SPIT_FLAG_TIMEOUT 0x1000
#define FLASH_SPI SPI1
#define SPI_FLASH_PerWritePageSize      256
/*FLASH 常用命令 */
/**
 * @brief  W25X_WriteEnable 命令，用于写使能
 * @note   在执行写操作前，需要先发送此命令
 */
#define W25X_WriteEnable 0x06

/**
 * @brief  W25X_WriteDisable 命令，用于写禁止
 * @note   在执行写操作后，需要发送此命令来禁止写操作
 */
#define W25X_WriteDisable 0x04

/**
 * @brief  W25X_ReadStatusReg 命令，用于读取状态寄存器
 * @note   通过此命令可以读取状态寄存器的值
 */
#define W25X_ReadStatusReg 0x05

/**
 * @brief  W25X_WriteStatusReg 命令，用于写入状态寄存器
 * @note   通过此命令可以写入状态寄存器的值
 */
#define W25X_WriteStatusReg 0x01

/**
 * @brief  W25X_ReadData 命令，用于读取数据
 * @note   通过此命令可以读取存储在 Flash 中的数据
 */
#define W25X_ReadData 0x03

/**
 * @brief  W25X_FastReadData 命令，用于快速读取数据
 * @note   通过此命令可以快速读取存储在 Flash 中的数据
 */
#define W25X_FastReadData 0x0B

/**
 * @brief  W25X_FastReadDual 命令，用于快速读取双倍数据
 * @note   通过此命令可以快速读取存储在 Flash 中的双倍数据
 */
#define W25X_FastReadDual 0x3B

/**
 * @brief  W25X_PageProgram 命令，用于页编程
 * @note   通过此命令可以将数据写入 Flash 的指定页
 */
#define W25X_PageProgram 0x02

/**
 * @brief  W25X_BlockErase 命令，用于块擦除
 * @note   通过此命令可以擦除 Flash 的指定块
 */
#define W25X_BlockErase 0xD8

/**
 * @brief  W25X_SectorErase 命令，用于扇区擦除
 * @note   通过此命令可以擦除 Flash 的指定扇区
 */
#define W25X_SectorErase 0x20

/**
 * @brief  W25X_ChipErase 命令，用于芯片擦除
 * @note   通过此命令可以擦除整个 Flash 芯片
 */
#define W25X_ChipErase 0xC7

/**
 * @brief  W25X_PowerDown 命令，用于掉电
 * @note   通过此命令可以使 Flash 芯片进入掉电状态
 */
#define W25X_PowerDown 0xB9

/**
 * @brief  W25X_ReleasePowerDown 命令，用于释放掉电状态
 * @note   通过此命令可以使 Flash 芯片从掉电状态中恢复
 */
#define W25X_ReleasePowerDown 0xAB

/**
 * @brief  W25X_DeviceID 命令，用于读取设备 ID
 * @note   通过此命令可以读取 Flash 芯片的设备 ID
 */
#define W25X_DeviceID 0xAB

/**
 * @brief  W25X_ManufactDeviceID 命令，用于读取制造商和设备 ID
 * @note   通过此命令可以读取 Flash 芯片的制造商和设备 ID
 */
#define W25X_ManufactDeviceID 0x90

/**
 * @brief  W25X_JedecDeviceID 命令，用于读取 Jedec 设备 ID
 * @note   通过此命令可以读取 Flash 芯片的 Jedec 设备 ID
 */
#define W25X_JedecDeviceID 0x9F

/**
 * @brief  定义 Flash 芯片的 ID
 * @note   此 ID 用于识别 Flash 芯片的型号
 */
#define sFLASH_ID 0XEF4018

/**
 * @brief  通过 SPI 总线发送和接收一个字节的数据
 * @param  send_data: 要发送的数据
 * @param  receive_data: 指向接收数据的指针
 * @retval 1 表示操作成功，0 表示操作失败
 */
uint8_t SPI_Send_receive_Byte(uint8_t send_data, uint8_t *receive_data);

/**
 * @brief  读取SPI Flash的ID
 * @param  无
 * @retval SPI Flash的ID
 */
uint32_t SPI_FLASH_ReadID(void);

/**
 * @brief  使能SPI Flash的写操作
 * @param  无
 * @retval 无
 */
void SPI_FLASH_WriteEnable(void);

/**
 * @brief  等待SPI Flash写入结束
 * @param  无
 * @retval 无
 */
void SPI_FLASH_WaitForWriteEnd(void);
/**
 * @brief  擦除SPI Flash的一个扇区
 * @param  SectorAddr: 要擦除的扇区地址
 * @retval 无
 */
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
/**
 * @brief  向SPI Flash的一个页写入数据
 * @param  pBuffer: 指向要写入数据的指针
 * @param  WriteAddr: 要写入的地址
 * @param  NumByteToWrite: 要写入的字节数（不能大于256）
 * @retval 无
 */
void SPI_FLASH_PageWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);