#include "cmd_handlers.h"
#include "hdlc_core.h" // 用于 hdlc_send_frame 和 CMD_XXX 定义
#include "MY_your_driver.h" // 包含你实现的底层驱动接口
#include <stddef.h> // 包含 NULL 定义

// #include "flash.h"          // 假设你的 flash_read 在 flash.h 中声明
// #include "i2c_driver.h"     // 假设 i2c_read_reg / i2c_write_reg 在此
// #include "spi_driver.h"     // 假设 spi_read_reg / spi_write_reg 在此

// ──────────────── FLASH ────────────────
void handle_flash_read(const uint8_t *payload, uint16_t len)
{
    if (len < 6)
        return;
    uint32_t addr = ((uint32_t)payload[0] << 24) |
                    ((uint32_t)payload[1] << 16) |
                    ((uint32_t)payload[2] << 8) |
                    (uint32_t)payload[3];
    uint16_t length = (payload[4] << 8) | payload[5];
    if (length > 256)
        length = 256;

    uint8_t buf[256];
    flash_read(addr, buf, length);
    hdlc_send_frame(CMD_FLASH_READ_RESULT, buf, length);
}

// ──────────────── I2C READ ────────────────
void handle_i2c_read_reg(const uint8_t *payload, uint16_t len)
{
    if (len != 2)
        return;
    uint8_t dev_addr = payload[0];
    uint8_t reg_addr = payload[1];
    uint8_t value;
    int ok = (i2c_read_reg(dev_addr, reg_addr, &value) == 0);
    hdlc_send_frame(CMD_I2C_READ_RESULT, ok ? &value : NULL, ok ? 1 : 0);
}

// ──────────────── I2C WRITE ────────────────
void handle_i2c_write_reg(const uint8_t *payload, uint16_t len)
{
    if (len != 3)
        return;
    uint8_t dev_addr = payload[0];
    uint8_t reg_addr = payload[1];
    uint8_t value = payload[2];
    i2c_write_reg(dev_addr, reg_addr, value);
    hdlc_send_frame(CMD_I2C_WRITE_ACK, NULL, 0);
}

// ──────────────── SPI READ ────────────────
void handle_spi_read_reg(const uint8_t *payload, uint16_t len)
{
    if (len != 1)
        return;
    uint8_t reg_addr = payload[0];
    uint8_t value;
    int ok = (spi_read_reg(reg_addr, &value) == 0);
    hdlc_send_frame(CMD_SPI_READ_RESULT, ok ? &value : NULL, ok ? 1 : 0);
}

// ──────────────── SPI WRITE ────────────────
void handle_spi_write_reg(const uint8_t *payload, uint16_t len)
{
    if (len != 2)
        return;
    uint8_t reg_addr = payload[0];
    uint8_t value = payload[1];
    spi_write_reg(reg_addr, value);
    hdlc_send_frame(CMD_SPI_WRITE_ACK, NULL, 0);
}