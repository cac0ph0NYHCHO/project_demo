#include "stm32f10x.h"                  // Device header
#include "Timer.h"

static uint8_t Key_State = 0;      // 按键状态机
static uint32_t Key_Time = 0;      // 消抖计时
uint8_t Key_Flag = 0;              // 按键按下标志


void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

// 无阻塞按键扫描（必须放在 while(1) 里不停跑）
void Key_Scan(void)
{
	switch(Key_State)
	{
		case 0:  //等待按下
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
			{
				Key_Time = GetTick();
				Key_State = 1;	
			}
			break;
		case 1:	// 消抖确认
			if(GetTick() - Key_Time >= 20)
			{
				if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
				{
					Key_State = 2;  // 等待松开
				}
				else
				{
					Key_State = 0;  // 抖动，放弃
				}
			}
			break;
		case 2:	// 等待松开
			if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 1)
			{
				Key_Flag = 1;      // 按键有效
				Key_State = 0;    // 回到初始
			}
			break;
	}
}


uint8_t Key_GetNum(void)
{
	if(Key_Flag == 1)
	{
		Key_Flag = 0;
		return 1;
	}
	return 0;
}
