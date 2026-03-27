#include "stm32f10x.h"                  // Device header
#include "Timer.h"

// 红灯状态定义
typedef enum {
	RED_OFF,      	// 熄灭
	RED_BLINK_SLOW,  // 慢闪（发送数据）
	RED_BLINK_FAST   // 快闪（异常/紧急）
} RedLED_State;

extern RedLED_State red_state;
extern uint32_t send_keep_timer;

uint8_t IR_State = 0;

void IRSensor_Init(void)
{
	//开启GPIO和AFIO时钟，注意EXTI不需要开启，而NVIC在内核RCC更管不到了
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	//GPIO初始化，配置成上拉输入
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	//AFIO-数据选择器初始化
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);
	//EXTI初始化
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line = EXTI_Line14;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
	//NVIC初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
}

void EXTI15_10_IRQHandler(void) 
{
	if(EXTI_GetITStatus(EXTI_Line14) == SET){
		send_keep_timer = GetTick();
		red_state = RED_BLINK_FAST;
		IR_State = 1;
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
}
