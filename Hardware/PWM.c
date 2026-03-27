#include "stm32f10x.h"                  // Device header

void PWM_Init(void)
{	//开启TIM时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	//初始化GPIO PA0
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;					//复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//选择RCC内部时钟
	TIM_InternalClockConfig(TIM2);
	//配置时基单元
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;		//时钟源到时基单元中间滤波器的一个参数，无影响
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	TIM_TimeBaseInitStruct.TIM_Period = 100 - 1;					//ARR的值
	TIM_TimeBaseInitStruct.TIM_Prescaler = 720 - 1;					//PSC的值
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter = 0;				//高级计时器才有的
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	//使能TIM
	TIM_Cmd(TIM2, ENABLE);
	//配置输出比较单元OC1
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_OCStructInit(&TIM_OCInitStruct);						//给结构体赋默认值
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;				//PWM1模式1
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;		//极性不反转
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;	//使能
	TIM_OCInitStruct.TIM_Pulse = 50;							//CCR的值
	TIM_OC1Init(TIM2, &TIM_OCInitStruct);
}

void PWM_SetCCR(uint16_t Compare)
{
	TIM_SetCompare1(TIM2, Compare);
}

