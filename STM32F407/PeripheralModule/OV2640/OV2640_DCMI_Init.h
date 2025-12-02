#include "stm32f4xx.h"

// 摄像头引脚与STM32引脚对应关系
// | 摄像头引脚 | STM32引脚 |
// |------------|-----------|
// | DCMI_D0    | PC6       |
// | DCMI_D1    | PC7       |
// | DCMI_D2    | PC8       |
// | DCMI_D3    | PC9       |
// | DCMI_D4    | PE4       |
// | DCMI_D5    | PB6       |
// | DCMI_D6    | PE5       |
// | DCMI_D7    | PE6       |
// | DCMI_HSYNC | PA4       |
// | DCMI_VSYNC | PB7       |
// | DCMI_PIXCLK| PA6       |
// | DCMI_PWDN  | PE7       |
// | DCMI_RST   | PE8       |


// DCMI_D0引脚配置
#define DCMI_D0_GPIO_PORT        	GPIOC
#define DCMI_D0_GPIO_CLK         	RCC_AHB1Periph_GPIOC
#define DCMI_D0_GPIO_PIN         	GPIO_Pin_6
#define DCMI_D0_PINSOURCE        	GPIO_PinSource6
#define DCMI_D0_AF			          GPIO_AF_DCMI

// DCMI_D1引脚配置
#define DCMI_D1_GPIO_PORT        	GPIOC
#define DCMI_D1_GPIO_CLK         	RCC_AHB1Periph_GPIOC
#define DCMI_D1_GPIO_PIN         	GPIO_Pin_7
#define DCMI_D1_PINSOURCE        	GPIO_PinSource7
#define DCMI_D1_AF			          GPIO_AF_DCMI

// DCMI_D2引脚配置
#define DCMI_D2_GPIO_PORT        	GPIOC
#define DCMI_D2_GPIO_CLK         	RCC_AHB1Periph_GPIOC
#define DCMI_D2_GPIO_PIN         	GPIO_Pin_8
#define DCMI_D2_PINSOURCE        	GPIO_PinSource8
#define DCMI_D2_AF			          GPIO_AF_DCMI

// DCMI_D3引脚配置
#define DCMI_D3_GPIO_PORT        	GPIOC
#define DCMI_D3_GPIO_CLK         	RCC_AHB1Periph_GPIOC
#define DCMI_D3_GPIO_PIN         	GPIO_Pin_9
#define DCMI_D3_PINSOURCE        	GPIO_PinSource9
#define DCMI_D3_AF			          GPIO_AF_DCMI

// DCMI_D4引脚配置
#define DCMI_D4_GPIO_PORT        	GPIOE
#define DCMI_D4_GPIO_CLK         	RCC_AHB1Periph_GPIOE
#define DCMI_D4_GPIO_PIN         	GPIO_Pin_4
#define DCMI_D4_PINSOURCE        	GPIO_PinSource4
#define DCMI_D4_AF			          GPIO_AF_DCMI

// DCMI_D5引脚配置
#define DCMI_D5_GPIO_PORT        	GPIOB
#define DCMI_D5_GPIO_CLK         	RCC_AHB1Periph_GPIOB
#define DCMI_D5_GPIO_PIN         	GPIO_Pin_6
#define DCMI_D5_PINSOURCE        	GPIO_PinSource6
#define DCMI_D5_AF			          GPIO_AF_DCMI

// DCMI_D6引脚配置
#define DCMI_D6_GPIO_PORT        	GPIOE
#define DCMI_D6_GPIO_CLK         	RCC_AHB1Periph_GPIOE
#define DCMI_D6_GPIO_PIN         	GPIO_Pin_5
#define DCMI_D6_PINSOURCE        	GPIO_PinSource5
#define DCMI_D6_AF			          GPIO_AF_DCMI

// DCMI_D7引脚配置
#define DCMI_D7_GPIO_PORT        	GPIOE
#define DCMI_D7_GPIO_CLK         	RCC_AHB1Periph_GPIOE
#define DCMI_D7_GPIO_PIN         	GPIO_Pin_6
#define DCMI_D7_PINSOURCE        	GPIO_PinSource6
#define DCMI_D7_AF			          GPIO_AF_DCMI

// DCMI水平同步引脚配置
#define DCMI_HSYNC_GPIO_PORT        	GPIOA
#define DCMI_HSYNC_GPIO_CLK         	RCC_AHB1Periph_GPIOA
#define DCMI_HSYNC_GPIO_PIN         	GPIO_Pin_4
#define DCMI_HSYNC_PINSOURCE        	GPIO_PinSource4
#define DCMI_HSYNC_AF			          GPIO_AF_DCMI

// DCMI垂直同步引脚配置
#define DCMI_VSYNC_GPIO_PORT        	GPIOB
#define DCMI_VSYNC_GPIO_CLK         	RCC_AHB1Periph_GPIOB
#define DCMI_VSYNC_GPIO_PIN         	GPIO_Pin_7
#define DCMI_VSYNC_PINSOURCE        	GPIO_PinSource7
#define DCMI_VSYNC_AF			          GPIO_AF_DCMI

// DCMI像素时钟引脚配置
#define DCMI_PIXCLK_GPIO_PORT        	GPIOA
#define DCMI_PIXCLK_GPIO_CLK         	RCC_AHB1Periph_GPIOA
#define DCMI_PIXCLK_GPIO_PIN         	GPIO_Pin_6
#define DCMI_PIXCLK_PINSOURCE        	GPIO_PinSource6
#define DCMI_PIXCLK_AF			          GPIO_AF_DCMI

// DCMI电源下电引脚配置
#define DCMI_PWDN_GPIO_PORT        	GPIOE
#define DCMI_PWDN_GPIO_CLK         	RCC_AHB1Periph_GPIOE
#define DCMI_PWDN_GPIO_PIN         	GPIO_Pin_7

// DCMI复位引脚配置
#define DCMI_RST_GPIO_PORT        	GPIOE
#define DCMI_RST_GPIO_CLK         	RCC_AHB1Periph_GPIOE
#define DCMI_RST_GPIO_PIN         	GPIO_Pin_8

// static void DCMI_GPIO_Init(void);
// static void DCMI_DCMI_Init(void);

void DCMI_Camera_Init(void);