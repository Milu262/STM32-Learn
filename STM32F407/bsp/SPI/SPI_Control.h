
// #include "board.h"
// #include <stdio.h>
#include "spi_init.h"

#define Dummy_Byte 0xFF
#define SPIT_FLAG_TIMEOUT 0xffff
#define FLASH_SPI SPI3
#define SPI_FLASH_PerWritePageSize 256





/*
 * SPI Flash 常用命令定义
 *
 * 这些命令用于与SPI Flash存储器进行通信，执行各种操作，如写使能、写禁止、读取状态寄存器、
 * 写入状态寄存器、读取数据、页编程、块擦除、芯片擦除、掉电、释放掉电状态以及读取设备ID等。
 *
 * 每个命令都作为一个宏定义，其值都是对应的十六进制代码。
 */

/* 写使能命令 */
#define W25X_WriteEnable         0x06  // 在执行写操作前，需要先发送此命令

/* 写禁止命令 */
#define W25X_WriteDisable        0x04  // 在执行写操作后，需要发送此命令来禁止写操作

/* 读取状态寄存器命令 */
#define W25X_ReadStatusReg       0x05  // 通过此命令可以读取状态寄存器的值

/* 写入状态寄存器命令 */
#define W25X_WriteStatusReg      0x01  // 通过此命令可以写入状态寄存器的值

/* 读取数据命令 */
#define W25X_ReadData            0x03  // 通过此命令可以读取存储在 Flash 中的数据

/* 快速读取数据命令 */
#define W25X_FastReadData        0x0B  // 通过此命令可以快速读取存储在 Flash 中的数据

/* 快速读取双倍数据命令 */
#define W25X_FastReadDual        0x3B  // 通过此命令可以快速读取存储在 Flash 中的双倍数据

/* 页编程命令 */
#define W25X_PageProgram         0x02  // 通过此命令可以将数据写入 Flash 的指定页

/* 块擦除命令 */
#define W25X_BlockErase          0xD8  // 通过此命令可以擦除 Flash 的指定块

/* 扇区擦除命令 */
#define W25X_SectorErase         0x20  // 通过此命令可以擦除 Flash 的指定扇区

/* 芯片擦除命令 */
#define W25X_ChipErase           0xC7  // 通过此命令可以擦除整个 Flash 芯片

/* 掉电命令 */
#define W25X_PowerDown           0xB9  // 通过此命令可以使 Flash 芯片进入掉电状态

/* 释放掉电状态命令 */
#define W25X_ReleasePowerDown    0xAB  // 通过此命令可以使 Flash 芯片从掉电状态中恢复

/* 读取设备ID命令 */
#define W25X_DeviceID            0xAB  // 注意：这个命令与释放掉电状态命令相同，具体使用需根据上下文区分
                                       // 通过此命令可以读取 Flash 芯片的设备 ID（但需注意与W25X_ReleasePowerDown的区分）

/* 读取制造商和设备ID命令 */
#define W25X_ManufactDeviceID    0x90  // 通过此命令可以读取 Flash 芯片的制造商和设备 ID

/* 读取Jedec设备ID命令 */
#define W25X_JedecDeviceID       0x9F  // 通过此命令可以读取 Flash 芯片的Jedec设备ID

/* 定义 Flash 芯片的 ID */
#define sFLASH_ID                0XBA6015  // 此 ID 用于识别 Flash 芯片的型号 板上使用的是ZD25WQ16 


// 注意：W25X_DeviceID和W25X_ReleasePowerDown使用了相同的命令码0xAB，
// 这在实际应用中可能需要通过上下文或其他方式来区分它们的使用场景。


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

/**
 * @brief 将数据写入SPI Flash
 *
 * 该函数根据指定的写入地址和字节数，将数据从缓冲区写入SPI Flash。该函数
 * 考虑了SPI Flash的页边界，并处理了数据跨越多个页或不与页边界对齐的情况。
 *
 * @param  pBuffer 指向要写入数据的缓冲区指针
 * @param  WriteAddr 写入的起始地址
 * @param  NumByteToWrite 要写入的字节数
 */
void SPI_FLASH_BufferWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
/**
 * @brief 从SPI Flash读取数据到缓冲区
 *
 * 该函数从指定的SPI Flash地址开始读取数据，并将读取的数据存储到缓冲区中。
 * 它首先发送读取命令和地址，然后循环读取数据直到达到指定的字节数。
 *
 * @param [out] pBuffer 指向存储读取数据的缓冲区指针
 * @param [in] ReadAddr 读取的起始地址
 * @param [in] NumByteToRead 要读取的字节数
 */
void SPI_FLASH_BufferRead(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead);
