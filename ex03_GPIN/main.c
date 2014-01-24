
/*
 * ex03_GPIN
 * ���̽�ƽ �Է� �׽�Ʈ.
 * 100ms �ֱ�� GPIO�� ���� ���̽�ƽ�� ��ư �Է��� �Է¹޴´�.
 */

#include "stm32f4xx.h"
#include "util.h"
#include "led.h"
#include "switch.h"


#define COUNTER_PER_SEC		1000

int main()
{
    int i;

    Init_LED();
    Init_SW();

    if (SysTick_Config(SystemCoreClock / COUNTER_PER_SEC)) {
        /* Capture error */
        while (1);
    }

    while(1) {
        for(i=KEY_LEFT; i<NO_KEY; i++) {
            if ( Read_SW(i) == 0) {
                LED_On(i);
            }
            else {
                LED_Off(i);
            }
        }

        Delay(100);
    }
}



void SysTick_Handler(void)
{
    TimingDelay_Decrement();
}

