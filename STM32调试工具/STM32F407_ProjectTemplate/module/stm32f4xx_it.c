/**
 ******************************************************************************
 * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c
 * @author  MCD Application Team
 * @version V1.8.1
 * @date    27-January-2022
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "bsp_uart.h"
#include "DMA_Init.h"
#include "response.h"
#include <stdint.h>

/** @addtogroup Template_Project
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void USART1_IRQHandler(void)
{

  // if (USART_GetITStatus(BSP_USART, USART_IT_RXNE) != RESET)//触发接收中断
  // {
  //   test_data.data[test_data.len] = USART_ReceiveData(BSP_USART);

  //   if (++test_data.len > data_size - 2)
  //   {
  //     test_data.len = 0;
  //   }
  // }

  // if (USART_GetITStatus(BSP_USART, USART_IT_IDLE) != RESET)
  // {

  //   USART_ReceiveData(BSP_USART);
  //   test_data.data[test_data.len] = '\0';
  //   test_data.flag = 1;
  // }

  // 以下是DMA的方式
  if (USART_GetITStatus(BSP_USART, USART_IT_IDLE) != RESET) // 触发空闲中断
  {
    test_data.flag = 1;                                                                 // 接收完成标志
    DMA_Cmd(DEBUG_USART1_RX_DMA_STREAM, DISABLE);                                       // 暂时关闭接收串口的DMA，数据待处理
    usart1_rx_len = USART_MAX_LEN - DMA_GetCurrDataCounter(DEBUG_USART1_RX_DMA_STREAM); // 计算接收到的数据长度
    DMA_ClearFlag(DEBUG_USART1_RX_DMA_STREAM, DMA_FLAG_TCIF5);                          // 清除接收完成标志
    // 做自己的事,例如将接收到的数据回显
    // usart_send_String_DMA(DMA_USART1_RX_BUF, usart1_rx_len); // 回显
    if (usart1_rx_len > data_size - 2)
    {
      test_data.err = 1;
    }
    else
    {
      memcpy(test_data.data, DMA_USART1_RX_BUF, usart1_rx_len); // 将接收到的数据复制到test_data.data
      test_data.data[usart1_rx_len] = '\0';
    }

    DMA_SetCurrDataCounter(DEBUG_USART1_RX_DMA_STREAM, USART_MAX_LEN); // 重新设置DMA的缓冲区大小
    DMA_Cmd(DEBUG_USART1_RX_DMA_STREAM, ENABLE);                       // 重新开启接收串口的DMA
    USART_ReceiveData(BSP_USART);                                      // 清除接收中断标志
  }

  if (USART_GetFlagStatus(BSP_USART, USART_IT_TXE) == RESET) // 串口发送寄存器为空
  {
    USART_ITConfig(USART1, USART_IT_TC, DISABLE); // 关闭发送完成中断
    usart1_rx_len = 0;                            // 清空接收数据长度
    // printf("send ok\r\n");
  }
}

void DMA2_Stream7_IRQHandler(void)
{
  if (DMA_GetITStatus(DEBUG_USART1_TX_DMA_STREAM, DMA_IT_TCIF7) != RESET) // 等待DMA发送完成中断
  {
    DMA_ClearITPendingBit(DEBUG_USART1_TX_DMA_STREAM, DMA_IT_TCIF7); // 清除DMA发送完成中断标志
    DMA_Cmd(DEBUG_USART1_TX_DMA_STREAM, DISABLE);                    // 关闭DMA发送
    USART_ITConfig(USART1, USART_IT_TC, ENABLE);                     // 使能串口发送完成中断
  }
}

void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
{
}*/

/**




  * @}
  */
