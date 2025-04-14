#include "stm32f4xx.h"

/**
 * @brief 配置中断控制器（NVIC）的优先级和使能状态。
 *
 * 该函数主要用于配置USART1（串口1）和DMA2（直接内存访问控制器2）的相关中断。
 * 具体配置包括设置中断优先级和使能中断。
 */
void NVIC_Configuration(void);