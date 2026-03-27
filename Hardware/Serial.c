#include "stm32f10x.h"                  // Device header
#include <stdio.h>

void Serial_Init(void)
{
	//开启GPIO和USART时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//初始化GPIO PA9，发数据
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//初始化USART1
	USART_InitTypeDef USART_InitStruct;
	USART_InitStruct.USART_BaudRate = 9600;											//波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制
	USART_InitStruct.USART_Mode = USART_Mode_Tx;									//模式：发送数据
	USART_InitStruct.USART_Parity = USART_Parity_No;								//无校验位
	USART_InitStruct.USART_StopBits = USART_StopBits_1;								//1位停止位
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;						//8位数据位
	USART_Init(USART1, &USART_InitStruct);
	//使能USART
	USART_Cmd(USART1, ENABLE);
}

//函数：发送一个字节
//参数：要发送的字节（数字形式）
//返回值：无
void Serial_SendByte(uint16_t Byte)
{
	USART_SendData(USART1, Byte);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);					//TXE:发送数据寄存器空 (Transmit data register empty)
																					//确保数据已经从 发送数据寄存器 转移到 发送移位寄存器
}

//函数：发送一个字符串
//参数：字符串（传入的是第一个字符的首地址）
//返回值：无
void Serial_SendString(char *String)
{
	uint8_t i;
	for(i = 0; String[i] != '\0'; i++){
		Serial_SendByte(String[i]);
	}
}

//printf重定向
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}
