#ifndef __OV2640_INIT_H__
#define __OV2640_INIT_H__

#include "stdint.h"
#define OV2640_Device_Address 0x60
// 竖屏---------------------------
#define OV2640_IMAGE_WIDTH 240 * 4
#define OV2640_IMAGE_HEIGHT 280

// 横屏---------------------------
//  #define OV2640_IMAGE_WIDTH  280
//  #define OV2640_IMAGE_HEIGHT 240

/**
  * @brief       配置OV2640摄像头的图像参数
  * @retval      返回配置结果，0表示成功，其他表示失败
  */
uint8_t OV2640_Image_Config(void);

/**
  * @brief       设置OV2640摄像头输出图像的尺寸
  * @param       width:  输出图像的宽度（必须是4的倍数）
  * @param       height: 输出图像的高度（必须是4的倍数）
  * @retval      返回设置结果，0表示成功，1表示宽度不是4的倍数，2表示高度不是4的倍数
  */
uint8_t OV2640_ImageSize_Set(uint16_t width, uint16_t height);

/**
  * @brief       初始化OV2640摄像头模块的硬件接口
  * @retval      返回初始化结果，0表示成功，其他表示失败
  */
uint8_t OV2640_Hardware_Init(void);
#endif