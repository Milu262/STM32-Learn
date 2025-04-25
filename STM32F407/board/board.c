
#include <board.h>

static __IO uint32_t g_system_tick = 0;

/**
 * This function will initial stm32 board.
 */
void board_init(void)
{
    /* NVIC Configuration */
#define NVIC_VTOR_MASK 0x3FFFFF80
#ifdef VECT_TAB_RAM
    /* Set the Vector Table base location at 0x10000000 */
    SCB->VTOR = (0x10000000 & NVIC_VTOR_MASK);
#else /* VECT_TAB_FLASH  */
    /* Set the Vector Table base location at 0x08000000 */
    SCB->VTOR = (0x08000000 & NVIC_VTOR_MASK);
#endif

    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick->LOAD = 0xFFFF;                   // ��ռ�����
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // ��ʼ����

    //	RCC_ClocksTypeDef rcc;
    //	RCC_GetClocksFreq(&rcc);//��ȡϵͳʱ��Ƶ��
}

void delay_us(uint32_t _us)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;

    ticks = _us * (SystemCoreClock / 1000000);

    told = SysTick->VAL;

    while (1)
    {
        tnow = SysTick->VAL;

        if (tnow != told)
        {
            if (tnow < told)
                tcnt += told - tnow;
            else
                tcnt += SysTick->LOAD - tnow + told;

            told = tnow;

            if (tcnt >= ticks)
                break;
        }
    }
}


void delay_ms(uint32_t _ms) { delay_us(_ms * 1000); }

void delay_1ms(uint32_t ms) { delay_us(ms * 1000); }

void delay_1us(uint32_t us) { delay_us(us); }
