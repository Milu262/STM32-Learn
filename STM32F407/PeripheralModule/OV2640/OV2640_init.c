#include "OV2640_init.h"
#include "OV2640_DCMI_Init.h"
#include "OV2640_DMA_Init.h"
#include "OV2640_reg.h"
#include "i2c_handle.h"
#include <stdio.h>

/**
 * @brief  OV2640_WriteReg
 * @param  WriteAddr: OV2640寄存器地址
 * @param  pBuffer: 数据
 * @retval 1 if the write operation was successful, 0 otherwise.
 */
static uint8_t OV2640_WriteReg(uint8_t WriteAddr, uint8_t pBuffer) {
  return I2C_ByteWrite(OV2640_Device_Address, WriteAddr, pBuffer);
}

/**
 * @brief  OV2640_OutSize_Set
 * @param  width: 输出宽度
 * @param  height: 输出高度
 * @retval None
 */
static uint8_t OV2640_OutSize_Set(uint16_t width, uint16_t height) {
  uint16_t outh;
  uint16_t outw;
  uint8_t temp;
  if (width % 4)
    return 1;
  if (height % 4)
    return 2;
  outw = width / 4;
  outh = height / 4;
  OV2640_WriteReg(0XFF, 0X00);
  OV2640_WriteReg(0XE0, 0X04);
  OV2640_WriteReg(0X50, outw & 0X00); // 设置v divider hdivider
  OV2640_WriteReg(0X5A, outw & 0XFF); // 设置OUTW的低八位
  OV2640_WriteReg(0X5B, outh & 0XFF); // 设置OUTH的低八位
  temp = (outw >> 8) & 0X03;
  temp |= (outh >> 6) & 0X04;
  OV2640_WriteReg(0X5C, temp); // 设置OUTH/OUTH的高位
  OV2640_WriteReg(0XE0, 0X00);
  return 0;
}

uint8_t OV2640_ImageSize_Set(uint16_t width, uint16_t height) {
  uint8_t temp;
  OV2640_WriteReg(0XFF, 0X00);
  OV2640_WriteReg(0XE0, 0X04);
  OV2640_WriteReg(0XC0, (width) >> 3 & 0XFF);
  OV2640_WriteReg(0XC1, (height) >> 3 & 0XFF);
  temp = (width & 0X07) << 3;
  temp |= height & 0X07;
  temp |= (width >> 4) & 0X80;
  OV2640_WriteReg(0X8C, temp);
  OV2640_WriteReg(0XE0, 0X00);
  return 0;
}

u8 OV2640_ImageWin_Set(u16 offx, u16 offy, u16 width, u16 height) {
  u16 hsize;
  u16 vsize;
  u8 temp;
  if (width % 4)
    return 1;
  if (height % 4)
    return 2;
  hsize = width / 4;
  vsize = height / 4;
  OV2640_WriteReg(0XFF, 0X00);
  OV2640_WriteReg(0XE0, 0X04);
  OV2640_WriteReg(0X51, hsize & 0XFF);
  OV2640_WriteReg(0X52, vsize & 0XFF);
  OV2640_WriteReg(0X53, offx & 0XFF);
  OV2640_WriteReg(0X54, offy & 0XFF);
  temp = (vsize >> 1) & 0X80;
  temp |= (offy >> 4) & 0X70;
  temp |= (hsize >> 5) & 0X08;
  temp |= (offx >> 8) & 0X07;
  OV2640_WriteReg(0X55, temp);
  OV2640_WriteReg(0X57, (hsize >> 2) & 0X80);
  OV2640_WriteReg(0XE0, 0X00);
  return 0;
}

uint8_t OV2640_Image_Config(void) {
  uint32_t i;
  uint8_t err;
  // 控制DCMI复位引脚
  //  复位
  // GPIO_ResetBits(DCMI_RST_GPIO_PORT, DCMI_RST_GPIO_PIN);
  // delay_ms(1000);
  GPIO_SetBits(DCMI_RST_GPIO_PORT, DCMI_RST_GPIO_PIN);

  for (i = 0; i < (sizeof(OV2640_UXGA) / 2); i++) {
    err = OV2640_WriteReg(OV2640_UXGA[i][0], OV2640_UXGA[i][1]);
    // delay_ms(1);
    if (err==0) {
      GPIO_SetBits(GPIOB, GPIO_Pin_2);

      //   printf("OV2640 Image Config error!!\r\n");
      return 0;
    }
  }

  // printf("OV2640 Image Config OK!!\r\n");

  // OV2640_ImageSize_Set(280, 240);

  // OV2640_ImageWin_Set(4, 0, OV2640_IMAGE_WIDTH, OV2640_IMAGE_HEIGHT);

  err = OV2640_OutSize_Set(OV2640_IMAGE_WIDTH, OV2640_IMAGE_HEIGHT);
  if (err) {
    printf("OV2640 OutSize Set error!!\r\n");
    return 0;
  }

  return 1;
}

uint8_t OV2640_Hardware_Init(void) {
  // uint8_t err;
  DCMI_Camera_Init();
  DMA_DCMI_Init_Config();
  return 1;
}
