#ifndef __TEST_CODE_H__
#define __TEST_CODE_H__
#include <stdio.h>
// 以下是测试代码

#define TEST_WRITE_ADDR 0x000000				   // 测试写入的起始地址
#define TEST_WRITE_DATA "Hello SPI Flash!"		   // 测试写入的数据
#define TEST_READ_ADDR TEST_WRITE_ADDR			   // 测试读取的地址（与写入地址相同）
#define TEST_READ_SIZE sizeof(TEST_WRITE_DATA) - 1 // 测试读取的字节数（不包括字符串结尾的空字符）

int test_SPI_Flash(void);
#endif
