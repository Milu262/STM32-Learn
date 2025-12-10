/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.h 
  * @author  MCD Application Team
  * @version V1.8.1
  * @date    27-January-2022
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F4xx_IT_H
#define __STM32F4xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

/**
 * @brief 处理USART2全局中断请求。
 * 当USART2接收到数据或发生错误时，会触发此中断。
 * 该函数检查中断状态，并在接收到空闲中断时处理接收的数据。
 */
void USART2_IRQHandler(void);

/**
 * @brief  处理DMA2 Stream7全局中断请求。
 *
 * 当DMA2 Stream7传输完成时，会触发此中断。
 * 该函数清除中断标志并在传输完成后禁用DMA流。
 */
void DMA2_Stream7_IRQHandler(void);

/**
 * @brief  处理DMA1 Stream6全局中断请求。
 *
 * 当DMA1 Stream6传输完成时，会触发此中断。
 * 该函数清除中断标志。
 */
void DMA1_Stream6_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_IT_H */

