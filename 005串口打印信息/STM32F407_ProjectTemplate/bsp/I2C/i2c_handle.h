
#include "stm32f4xx.h"


/* 通讯等待超时时间 */
#define I2CT_FLAG_TIMEOUT ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

//错误处理函数
static uint32_t I2C_TIMEOUT_UserCallback(uint8_t errorCode);
//按字节写
uint32_t I2C_ByteWrite(u8 slave_adress,I2C_TypeDef *I2Cx ,u8 *pBuffer, u8 WriteAddr);
//多字节写，每次写完一字节寄存器地址自动加一
uint8_t I2C_ByetsWrite(uint8_t slave_adress,I2C_TypeDef *I2Cx,uint8_t *pBuffer, uint8_t WriteAddr,uint16_t NumByteToWrite);
//读一个字节
uint32_t I2C_byteRead(uint8_t slave_adress, I2C_TypeDef *I2Cx, u8 *pBuffer, u8 ReadAddr);
//读多个字节
uint32_t I2C_BufferRead(uint8_t slave_adress,I2C_TypeDef *I2Cx ,u8* pBuffer, u8 ReadAddr, u16 NumByteToRead);