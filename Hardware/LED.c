#include "stm32f10x.h"                  // Device header
#include "LED.h"
#include "PWM.h"

static void BlueLED_Init(void)
{
	PWM_Init();
}

static void RedLED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;					
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}

void LED_Init(void)
{
	BlueLED_Init();
	RedLED_Init();
}

//红灯翻转
void RedLED_Toggle(void)
{
	if(GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1) == SET)
    {
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);
    }
    else
    {
        GPIO_SetBits(GPIOA, GPIO_Pin_1);   
    }
}

//红灯熄灭
void RedLED_OFF(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_1);
}