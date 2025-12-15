#include "spi_driver.h"
#include "SPI_Control.h"
#include "spi_init.h"
#include <string.h>
void flash_read(uint32_t addr, uint8_t *buf, uint16_t len)
{
    if(buf == NULL || len == 0)
    {
        return; // 无效调用
    }
    SPI_FLASH_BufferRead(buf, addr, len);
}

void flash_write(uint32_t addr, uint8_t *buf, uint16_t len)
{
    if(buf == NULL || len == 0)
    {
        return; // 无效调用
    }

    SPI_FLASH_BufferWrite(buf, addr, len);
}

void flash_Sector_erase(uint32_t addr)
{
    // 检查地址对齐
    if(addr % 4096 != 0)
    {
        return; // 无效调用
    }
    SPI_FLASH_SectorErase(addr);
}

void flash_Block_erase32(uint32_t addr)
{
    // 检查地址对齐
    if(addr % (1024*32) != 0)
    {
        return; // 无效调用
    }
    SPI_FLASH_BlockErase32(addr);
}

void flash_Block_erase64(uint32_t addr)
{
    // 检查地址对齐
    if(addr % (1024*64) != 0)
    {
        return; // 无效调用
    }
    SPI_FLASH_BlockErase64(addr);
}

void flash_Chip_erase(void)
{
    SPI_FLASH_ChipErase();
}

int spi_read_reg(uint8_t reg_addr, uint8_t *value)
{
    if (value == NULL)
    {
        return -1;
    }

    uint8_t dummy;
    uint8_t received;

    SPI_CS_ON(0);

    // Step 1: 发送读命令（通常 reg_addr | 0x80 表示读）
    if (SPI_Send_receive_Byte(reg_addr | 0x80, &dummy) != 1)
    {
        SPI_CS_ON(1);
        return -1;
    }

    // Step 2: 发送 dummy byte 并接收数据
    if (SPI_Send_receive_Byte(0x00, &received) != 1)
    {
        SPI_CS_ON(1);
        return -1;
    }

    SPI_CS_ON(1);

    *value = received;
    return 0;
}

int spi_write_reg(uint8_t reg_addr, uint8_t value)
{
    uint8_t dummy;

    SPI_CS_ON(0);

    // Step 1: 发送寄存器地址（写操作通常不加标志位）
    if (SPI_Send_receive_Byte(reg_addr, &dummy) != 1)
    {
        SPI_CS_ON(1);
        return -1;
    }

    // Step 2: 发送要写入的数据
    if (SPI_Send_receive_Byte(value, &dummy) != 1)
    {
        SPI_CS_ON(1);
        return -1;
    }

    SPI_CS_ON(1);
    return 0;
}