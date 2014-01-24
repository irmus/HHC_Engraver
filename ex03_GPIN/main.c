
/*
 * ex03_GPIN
 * 조이스틱 입력 테스트.
 * 100ms 주기로 GPIO를 통해 조이스틱과 버튼 입력을 입력받는다.
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

