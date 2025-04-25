/*
 * ������������Ӳ�������������չ����Ӳ�����Ϲ���ȫ����Դ
 * �����������www.lckfb.com
 * ����֧�ֳ�פ��̳���κμ������⻶ӭ��ʱ����ѧϰ
 * ������̳��club.szlcsc.com
 * ��עbilibili�˺ţ������������塿���������ǵ����¶�̬��
 * ��������׬Ǯ���������й�����ʦΪ����
 *

 Change Logs:
 * Date           Author       Notes
 * 2024-03-07     LCKFB-LP    first version
 */
#ifndef __BOARD_H__
#define __BOARD_H__

#include "stm32f4xx.h"

typedef struct
{
    uint8_t errorCode;        // 错误代码，用于唯一标识一个错误
    const char *errorMessage; // 错误信息，是一个指向字符数组的指针，用于存储错误的描述文本
} Error_Code;                 // 定义一个结构体Error_Code，用于存储错误的代码和对应的错误信息

void board_init(void);
void delay_us(uint32_t _us);
void delay_ms(uint32_t _ms);
void delay_1ms(uint32_t ms);
void delay_1us(uint32_t us);

#endif
