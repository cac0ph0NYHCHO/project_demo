#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdio.h>

void Serial_Init(void);
void Serial_SendByte(uint16_t Byte);
void Serial_SendString(char *String);


#endif
