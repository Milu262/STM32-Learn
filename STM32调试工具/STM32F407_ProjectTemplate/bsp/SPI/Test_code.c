#include "Test_code.h"

// 以下是测试代码


int test_SPI_Flash(void)
{
	uint32_t flash_id;
	uint8_t read_buffer[TEST_READ_SIZE];
	uint8_t write_buffer[TEST_READ_SIZE];
	uint8_t i;

	// 1. 初始化SPI Flash（假设SPI初始化已经在board.h或spi_init.h中完成）
	// 这里可能需要调用一些初始化SPI总线和Flash芯片的函数，但具体实现依赖于硬件和驱动程序
	// 例如：SPI_Init(); // 初始化SPI总线
	// FLASH_SPI_Init(); // 初始化Flash芯片（如果需要的话）

	// 2. 读取SPI Flash的ID
	flash_id = SPI_FLASH_ReadID();
	printf("SPI Flash ID: 0x%08X\n", flash_id);

	// 检查读取的ID是否与预期的Flash芯片ID匹配
	if (flash_id != sFLASH_ID)
	{
		printf("Error: SPI Flash ID mismatch!\n");
		return -1;
	}

	// 3. 使能SPI Flash的写操作
	SPI_FLASH_WriteEnable();

	// 4. 擦除一个扇区（假设扇区大小为4KB，即0x1000字节）
	// 注意：擦除操作会清除指定扇区内的所有数据，因此请确保擦除的是非重要数据区域
	SPI_FLASH_SectorErase(TEST_WRITE_ADDR & ~(0x0FFF)); // 将地址对齐到扇区边界

	// 等待擦除操作完成（如果需要的话，这里可以添加等待函数）
	SPI_FLASH_WaitForWriteEnd(); // 假设这个函数会等待写入/擦除操作完成

	// 5. 向一个页写入数据
	// 将测试数据复制到写缓冲区中
	for (i = 0; i < TEST_READ_SIZE; i++)
	{
		write_buffer[i] = TEST_WRITE_DATA[i];
	}

	// 写入数据到Flash中
	SPI_FLASH_PageWrite(write_buffer, TEST_WRITE_ADDR, TEST_READ_SIZE);

	// 等待写入操作完成（同样，这里可以添加等待函数）
	SPI_FLASH_WaitForWriteEnd(); // 假设这个函数会等待写入/擦除操作完成

	// 6. 从SPI Flash读取数据并验证
	// 读取数据到读缓冲区中
	SPI_FLASH_BufferRead(read_buffer, TEST_READ_ADDR, TEST_READ_SIZE);

	// 验证读取的数据是否与写入的数据相同
	printf("Data read from SPI Flash: ");
	for (i = 0; i < TEST_READ_SIZE; i++)
	{
		printf("%c", read_buffer[i]);
		if (read_buffer[i] != write_buffer[i])
		{
			printf("\nError: Data mismatch at index %d!\n", i);
			return -1;
		}
	}
	printf("\n");

	// 如果所有验证都通过，则打印成功消息
	printf("SPI Flash test passed!\n");
	return 0;
}