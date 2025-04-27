#include "SPI_Control.h"

// 屏幕的SPI接口定义在SPI_init.h中定义

// 屏幕使用SPI2
/**屏幕使用的管脚定义
 * - SCK:  PB10
 * - MOSI: PC3
 * - CS(NSS): PE9
 * - DC:  PD2
 * - RES: PD3
 * - BLK: PD1
 */

#define LCD_SPI SPI2
#define LCD_SPI_CS_GPIO_PORT GPIOE
#define LCD_SPI_CS_PIN GPIO_Pin_9

#define LCD_BLK_GPIO_PORT GPIOD
#define LCD_BLK_PIN GPIO_Pin_1
#define LCD_BLK_GPIO_CLK RCC_AHB1Periph_GPIOD

#define LCD_DC_GPIO_PORT GPIOD
#define LCD_DC_PIN GPIO_Pin_2
#define LCD_DC_GPIO_CLK RCC_AHB1Periph_GPIOD

#define LCD_RES_GPIO_PORT GPIOD
#define LCD_RES_PIN GPIO_Pin_3
#define LCD_RES_GPIO_CLK RCC_AHB1Periph_GPIOD

#define LCD_WIDTH 240
#define LCD_HIDTH 280

#define COLOR_WHITE 0xFFFF

#define USE_HORIZONTAL 0 // 设置屏幕方向，0或1为竖屏，2或3为横屏

#define LCD_SPI_CS_ON(x) GPIO_WriteBit(LCD_SPI_CS_GPIO_PORT, LCD_SPI_CS_PIN, x ? Bit_SET : Bit_RESET)

// 屏幕的背光控制
#define LCD_BLK_ON(x) GPIO_WriteBit(LCD_BLK_GPIO_PORT, LCD_BLK_PIN, x ? Bit_SET : Bit_RESET)
// 屏幕的指令控制
#define LCD_DC_ON(x) GPIO_WriteBit(LCD_DC_GPIO_PORT, LCD_DC_PIN, x ? Bit_SET : Bit_RESET)

// 屏幕的复位控制
#define LCD_RES_ON(x) GPIO_WriteBit(LCD_RES_GPIO_PORT, LCD_RES_PIN, x ? Bit_SET : Bit_RESET)

static void LCD_GPIO_Init(void);
static uint8_t LCD_WR_REG(uint8_t reg);
static uint8_t LCD_WR_DATA8(uint8_t data);
static void LCD_WR_DATA(uint16_t dat);
static uint8_t LCD_Writ_Bus(uint8_t dat);
void LCD_Screen_Init(void);

/**
 * @brief 设置显示区域的起始和结束地址
 *
 * @param x1 列的起始地址
 * @param y1 行的起始地址
 * @param x2 列的结束地址
 * @param y2 行的结束地址
 * @return void 无返回值
 */
void LCD_Address_Set(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

/**
 * @brief 填充指定区域的颜色
 *
 * @param xsta 列的起始地址
 * @param ysta 行的起始地址
 * @param xend 列的结束地址
 * @param yend 行的结束地址
 * @param color 填充颜色值
 * @return void 无返回值
 */
void LCD_Fill(uint16_t xsta, uint16_t ysta, uint16_t xend, uint16_t yend, uint16_t color);