

#include "stm32f4xx.h"
#include "motor.h"



static GPIO_TypeDef * clock_gpio[] = {GPIOC, GPIOC, GPIOA};
static uint16_t clock_pin[] = {GPIO_Pin_12, GPIO_Pin_8, GPIO_Pin_5};

static GPIO_TypeDef * dir_gpio[] = {GPIOC, GPIOC, GPIOG};
static uint16_t dir_pin[] = {GPIO_Pin_11, GPIO_Pin_3, GPIO_Pin_9};



void Motor_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    int i;





    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOG, ENABLE);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;


    for (i = 0; i < MOTOR_CNT; i++)
    {
        GPIO_InitStructure.GPIO_Pin = clock_pin[i];
        GPIO_Init(clock_gpio[i], &GPIO_InitStructure);

        GPIO_InitStructure.GPIO_Pin = dir_pin[i];
        GPIO_Init(dir_gpio[i], &GPIO_InitStructure);

    }



}


void Motor_SetDirection(int motor, int dir)
{
    GPIO_WriteBit(dir_gpio[motor], dir_pin[motor], (dir == DIR_CW) ? Bit_SET : Bit_RESET);

}





void Motor_ToggleClock(int motor)
{
    GPIO_ToggleBits(clock_gpio[motor], clock_pin[motor]);
}






