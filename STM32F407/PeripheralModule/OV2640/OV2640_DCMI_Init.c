#include "OV2640_DCMI_Init.h"

static void DCMI_GPIO_Init(void)
{
    // 启用DCMI相关引脚的时钟
    RCC_AHB1PeriphClockCmd(DCMI_RST_GPIO_CLK | DCMI_PWDN_GPIO_CLK | DCMI_VSYNC_GPIO_CLK | DCMI_HSYNC_GPIO_CLK | DCMI_PIXCLK_GPIO_CLK |
                               DCMI_D0_GPIO_CLK | DCMI_D1_GPIO_CLK | DCMI_D2_GPIO_CLK | DCMI_D3_GPIO_CLK |
                               DCMI_D4_GPIO_CLK | DCMI_D5_GPIO_CLK | DCMI_D6_GPIO_CLK | DCMI_D7_GPIO_CLK | DCMI_RST_GPIO_CLK,
                           ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    // 配置DCMI垂直同步引脚
    GPIO_InitStructure.GPIO_Pin = DCMI_VSYNC_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(DCMI_VSYNC_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(DCMI_VSYNC_GPIO_PORT, DCMI_VSYNC_PINSOURCE, DCMI_VSYNC_AF);

    // 配置DCMI水平同步引脚
    GPIO_InitStructure.GPIO_Pin = DCMI_HSYNC_GPIO_PIN;
    GPIO_Init(DCMI_HSYNC_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(DCMI_HSYNC_GPIO_PORT, DCMI_HSYNC_PINSOURCE, DCMI_HSYNC_AF);

    // 配置DCMI像素时钟引脚
    GPIO_InitStructure.GPIO_Pin = DCMI_PIXCLK_GPIO_PIN;
    GPIO_Init(DCMI_PIXCLK_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(DCMI_PIXCLK_GPIO_PORT, DCMI_PIXCLK_PINSOURCE, DCMI_PIXCLK_AF);

    // 配置DCMI数据引脚
    GPIO_InitStructure.GPIO_Pin = DCMI_D0_GPIO_PIN;
    GPIO_Init(DCMI_D0_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(DCMI_D0_GPIO_PORT, DCMI_D0_PINSOURCE, DCMI_D0_AF);

    GPIO_InitStructure.GPIO_Pin = DCMI_D1_GPIO_PIN;
    GPIO_Init(DCMI_D1_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(DCMI_D1_GPIO_PORT, DCMI_D1_PINSOURCE, DCMI_D1_AF);

    GPIO_InitStructure.GPIO_Pin = DCMI_D2_GPIO_PIN;
    GPIO_Init(DCMI_D2_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(DCMI_D2_GPIO_PORT, DCMI_D2_PINSOURCE, DCMI_D2_AF);

    GPIO_InitStructure.GPIO_Pin = DCMI_D3_GPIO_PIN;
    GPIO_Init(DCMI_D3_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(DCMI_D3_GPIO_PORT, DCMI_D3_PINSOURCE, DCMI_D3_AF);

    GPIO_InitStructure.GPIO_Pin = DCMI_D4_GPIO_PIN;
    GPIO_Init(DCMI_D4_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(DCMI_D4_GPIO_PORT, DCMI_D4_PINSOURCE, DCMI_D4_AF);

    GPIO_InitStructure.GPIO_Pin = DCMI_D5_GPIO_PIN;
    GPIO_Init(DCMI_D5_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(DCMI_D5_GPIO_PORT, DCMI_D5_PINSOURCE, DCMI_D5_AF);

    GPIO_InitStructure.GPIO_Pin = DCMI_D6_GPIO_PIN;
    GPIO_Init(DCMI_D6_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(DCMI_D6_GPIO_PORT, DCMI_D6_PINSOURCE, DCMI_D6_AF);

    GPIO_InitStructure.GPIO_Pin = DCMI_D7_GPIO_PIN;
    GPIO_Init(DCMI_D7_GPIO_PORT, &GPIO_InitStructure);
    GPIO_PinAFConfig(DCMI_D7_GPIO_PORT, DCMI_D7_PINSOURCE, DCMI_D7_AF);
    
    // 配置DCMI电源下电引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = DCMI_PWDN_GPIO_PIN;
    GPIO_Init(DCMI_PWDN_GPIO_PORT, &GPIO_InitStructure);

    // 配置DCMI复位引脚
    GPIO_InitStructure.GPIO_Pin = DCMI_RST_GPIO_PIN;
    GPIO_Init(DCMI_RST_GPIO_PORT, &GPIO_InitStructure);

    // 控制DCMI电源下电引脚
    GPIO_SetBits(DCMI_PWDN_GPIO_PORT, DCMI_PWDN_GPIO_PIN);
    GPIO_ResetBits(DCMI_PWDN_GPIO_PORT, DCMI_PWDN_GPIO_PIN);

    // 控制DCMI复位引脚
    GPIO_SetBits(DCMI_RST_GPIO_PORT, DCMI_RST_GPIO_PIN);
}
static void DCMI_DCMI_Init(void)
{
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_DCMI, ENABLE);

    DCMI_InitTypeDef DCMI_InitStructure;
    DCMI_DeInit();//清除原来的配置
    DCMI_InitStructure.DCMI_CaptureMode = DCMI_CaptureMode_Continuous;
    DCMI_InitStructure.DCMI_SynchroMode = DCMI_SynchroMode_Hardware;
    DCMI_InitStructure.DCMI_CaptureRate = DCMI_CaptureRate_All_Frame;
    DCMI_InitStructure.DCMI_ExtendedDataMode = DCMI_ExtendedDataMode_8b;
    DCMI_InitStructure.DCMI_PCKPolarity = DCMI_PCKPolarity_Rising;
    DCMI_InitStructure.DCMI_VSPolarity = DCMI_VSPolarity_Low;
    DCMI_InitStructure.DCMI_HSPolarity = DCMI_HSPolarity_Low;
    DCMI_Init(&DCMI_InitStructure);
    DCMI_Cmd(ENABLE);
    // 使能DCMI中断，帧中断
    // DCMI_ITConfig(DCMI_IT_FRAME, ENABLE);
}
void DCMI_Camera_Init(void)
{
    DCMI_GPIO_Init();
    DCMI_DCMI_Init();
}