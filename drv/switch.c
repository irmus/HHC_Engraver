
#include "stm32f4xx.h"
#include "switch.h"



static uint32_t SW_RCC[NO_KEY] = {
    RCC_AHB1Periph_GPIOE, 
    RCC_AHB1Periph_GPIOE, 
    RCC_AHB1Periph_GPIOE,
    RCC_AHB1Periph_GPIOE, 
    RCC_AHB1Periph_GPIOE, 
    RCC_AHB1Periph_GPIOD, 
    RCC_AHB1Periph_GPIOG, 
    RCC_AHB1Periph_GPIOG, 
    RCC_AHB1Periph_GPIOF, 
    RCC_AHB1Periph_GPIOB, 
    RCC_AHB1Periph_GPIOB, 
    RCC_AHB1Periph_GPIOD,
    RCC_AHB1Periph_GPIOD,
    RCC_AHB1Periph_GPIOD,
    RCC_AHB1Periph_GPIOC
};

static uint32_t SW_Pin[NO_KEY] = {
    GPIO_Pin_2,
    GPIO_Pin_3,
    GPIO_Pin_4,
    GPIO_Pin_5,
    GPIO_Pin_6,
    GPIO_Pin_7,
    GPIO_Pin_3,
    GPIO_Pin_2,
    GPIO_Pin_6,
    GPIO_Pin_7,
    GPIO_Pin_4,
    GPIO_Pin_5,
    GPIO_Pin_4,
    GPIO_Pin_2,
    GPIO_Pin_13
};

static GPIO_TypeDef * GPIO[NO_KEY] = {
    GPIOE,
    GPIOE,
    GPIOE,
    GPIOE,
    GPIOE,
    GPIOD,
    GPIOG,
    GPIOG,
    GPIOF,
    GPIOB,
    GPIOB,
    GPIOD,
    GPIOD,
    GPIOD,
    GPIOC
};



void Init_SW(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    uint8_t i;
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    
    for(i=0; i<NO_KEY; i++) {
        RCC_AHB1PeriphClockCmd(SW_RCC[i], ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = SW_Pin[i];
        GPIO_Init(GPIO[i], &GPIO_InitStructure);
    }
}


uint8_t Read_SW(uint8_t ch)
{
    if (ch < NO_KEY) {
        return GPIO_ReadInputDataBit(GPIO[ch], SW_Pin[ch]);
    }
    
    // Active High
    return 1;
}