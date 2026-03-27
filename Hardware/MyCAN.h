#ifndef __MYCAN_H
#define __MYCAN_H
 
extern CanRxMsg RxMessage;
extern uint8_t RxFlagStatus;
 
void MyCAN_Init(void);
void MyCAN_Transmit(uint32_t ID, uint8_t Length, uint8_t *Data);

#endif
