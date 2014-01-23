
#include "stm32f4xx.h"
#include "led.h"

#define NO_LED     4

static uint32_t LED_RCC[NO_LED] = {
    RCC_AHB1Periph_GPIOG, 
    RCC_AHB1Periph_GPIOG, 
    RCC_AHB1Periph_GPIOC, 
    RCC_AHB1Periph_GPIOB
};

static uint32_t GPIO_Pin[NO_LED] = {
    GPIO_Pin_13,
    GPIO_Pin_14,
    GPIO_Pin_5,
    GPIO_Pin_13
};

static GPIO_TypeDef * GPIO[NO_LED] = {
    GPIOG,
    GPIOG,
    GPIOC,
    GPIOB
};






void Init_LED(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    uint8_t i;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
    for(i=0; i<NO_LED; i++) {
        RCC_AHB1PeriphClockCmd(LED_RCC[i], ENABLE);
        
        
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin[i];
        GPIO_Init(GPIO[i], &GPIO_InitStructure);
    }
}

void LED_On(uint8_t ch)
{
    if(ch < NO_LED) {
        if(ch == 2) {
            GPIO[ch]->BSRRH = GPIO_Pin[ch];
        }
        else {
            GPIO[ch]->BSRRL = GPIO_Pin[ch];
        }
    }
}


void LED_Off(uint8_t ch)
{
    if(ch < NO_LED) {
        if(ch == 2) {
            GPIO[ch]->BSRRL = GPIO_Pin[ch];
        }
        else {
            GPIO[ch]->BSRRH = GPIO_Pin[ch];
        }
    }
}


void LED_Off_All(void)
{
    uint8_t i;
    
    for(i=0; i<NO_LED; i++) {
        LED_Off(i);
    }
}


void LED_On_All(void)
{
    uint8_t i;
    
    for(i=0; i<NO_LED; i++) {
        LED_On(i);
    }
}