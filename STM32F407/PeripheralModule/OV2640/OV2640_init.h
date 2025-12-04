#ifndef __OV2640_INIT_H
#define __OV2640_INIT_H

#include <stdint.h>

#define OV2640_Device_Address 0x60
// 竖屏---------------------------
#define OV2640_IMAGE_WIDTH 240 * 4
#define OV2640_IMAGE_HEIGHT 280

// 横屏---------------------------
//  #define OV2640_IMAGE_WIDTH  280
//  #define OV2640_IMAGE_HEIGHT 240

/**
 * @brief       向OV2640摄像头模块的指定寄存器写入一个字节的数据
 * @param       WriteAddr: 要写入的寄存器地址
 * @param       pBuffer:   要写入的数据
 * @retval      返回I2C写入操作的结果，1表示成功，其他值表示失败
 */
// static uint8_t OV2640_WriteReg(uint8_t WriteAddr, uint8_t pBuffer);

/**
 * @brief       设置OV2640摄像头输出图像的尺寸
 * @param       width:  输出图像的宽度（必须是4的倍数）
 * @param       height: 输出图像的高度（必须是4的倍数）
 * @retval 返回设置结果，0表示成功，1表示宽度不是4的倍数，2表示高度不是4的倍数
 */
// static uint8_t OV2640_OutSize_Set(uint16_t width, uint16_t height);

uint8_t OV2640_Image_Config(void);

uint8_t OV2640_ImageSize_Set(uint16_t width, uint16_t height);

uint8_t OV2640_Hardware_Init(void);

/**
 * @brief       播放OV2640摄像头模块的图像
 * @retval      返回播放结果，0表示成功，其他值表示失败
 */
int Ov2640PlayVideo(void);
#endif