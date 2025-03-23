
#include "stm32f4xx.h"
#include <stdio.h>
#include "board.h"

/* 通讯等待超时时间 */
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

/**
 * @brief User callback function for I2C timeout.
 *
 * This function is called when an I2C operation times out. It prints an error message
 * based on the provided error code and generates a STOP condition on the I2C bus.
 *
 * @param errorCode The error code indicating the reason for the timeout.
 * @return 0 to indicate a timeout occurred.
 */
static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);

/**
 * @brief Write a byte to a specified address in an I2C device.
 *
 * This function writes a byte to a specified address in an I2C device.
 *
 * @param slave_adress The 7-bit I2C address of the slave device.
 * @param WriteAddr The address to write to in the slave device.
 * @param pBuffer The byte to be written.
 * @return 1 if the write operation was successful, 0 otherwise.
 */
uint32_t I2C_ByteWrite(u8 slave_adress, u8 WriteAddr,u8 pBuffer);

//多字节写，每次写完一字节寄存器地址自动加一
/**
 * @brief Write multiple bytes to a specified address in an I2C device.
 *
 * This function writes multiple bytes to a specified address in an I2C device.
 *
 * @param slave_adress The 7-bit I2C address of the slave device.
 * @param WriteAddr The starting address to write to in the slave device.
 * @param pBuffer A pointer to the buffer containing the data to be written.
 * @param NumByteToWrite The number of bytes to write.
 * @return 1 if the write operation was successful, 0 otherwise.
 */
uint8_t I2C_ByetsWrite(uint8_t slave_adress, uint8_t WriteAddr,uint8_t *pBuffer,uint16_t NumByteToWrite);

/**
 * @brief Read a byte from a specified address in an I2C device.
 *
 * This function reads a byte from a specified address in an I2C device.
 *
 * @param slave_adress The 7-bit I2C address of the slave device.
 * @param ReadAddr The address to read from in the slave device.
 * @param pBuffer A pointer to the buffer where the read data will be stored.
 * @return 1 if the read operation was successful, 0 otherwise.
 */
uint32_t I2C_byteRead(uint8_t slave_adress, u8 ReadAddr, u8 *pBuffer);

/**
 * @brief Read multiple bytes from a specified address in an I2C device.
 *
 * This function reads multiple bytes from a specified address in an I2C device.
 *
 * @param slave_adress The 7-bit I2C address of the slave device.
 * @param ReadAddr The starting address to read from in the slave device.
 * @param pBuffer A pointer to the buffer where the read data will be stored.
 * @param NumByteToRead The number of bytes to read.
 * @return 1 if the read operation was successful, 0 otherwise.
 */
uint32_t I2C_BufferRead(uint8_t slave_adress , u8 ReadAddr,u8* pBuffer, u16 NumByteToRead);

/**
 * @brief Write a byte to a 16-bit Register address in an I2C device.
 *
 * This function writes a byte to a specified 16-bit Register address in an I2C device.
 *
 * @param slave_adress The 7-bit I2C address of the slave device.
 * @param WriteAddr The 16-bit Register address to write to in the slave device.
 * @param pBuffer The byte to be written.
 * @return 1 if the write operation was successful, 0 otherwise.
 */
uint32_t I2C_Write_16addr(u8 slave_adress, u16 WriteAddr, u8 pBuffer);

/**
 * @brief Read a byte from a 16-bit Register address in an I2C device.
 *
 * This function reads a byte from a specified 16-bit Register address in an I2C device.
 *
 * @param slave_adress The 7-bit I2C address of the slave device.
 * @param ReadAddr The 16-bit Register address to read from in the slave device.
 * @param pBuffer A pointer to the buffer where the read data will be stored.
 * @return 1 if the read operation was successful, 0 otherwise.
 */
uint32_t I2C_Read16addr(uint8_t slave_adress, u16 ReadAddr,u8 *pBuffer);

// 定义一个函数，用于查找I2C设备
void Find_i2c_device(void);

static uint8_t i2c_device_adress_find(uint8_t slave_adress);
