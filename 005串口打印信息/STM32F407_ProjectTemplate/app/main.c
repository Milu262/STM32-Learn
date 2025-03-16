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
#include "board.h"
#include "bsp_uart.h"
#include <stdio.h>
#include "i2c_init.h"

int main(void)
{
	//float t = 0.0;
	//int count = 0;

	//uint8_t recive[4]={0};
	
	board_init();
	
	uart1_init(115200U);
	User_I2C_Init();
	printf("USART Init!!\r\n");
	
	while(1)
	{

		if(test_data.flag == 1)
		{			
			do_process(&test_data);		
            
		}
		}
	

}
