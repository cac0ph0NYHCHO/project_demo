#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Key.h"
#include "MyCAN.h"
#include "LED.h"
#include "Timer.h"
#include "IRSensor.h"
#include "Serial.h"


/*===========枚举定义==========*/
typedef enum {
	RED_OFF,      						 // 熄灭
	RED_BLINK_SLOW,  					 // 慢闪（发送数据）
	RED_BLINK_FAST   					 // 快闪（异常/紧急）
} RedLED_State;


/*===========全局变量==========*/
// 呼吸灯相关：蓝色LED
static uint16_t pwm_val = 0;			 // 当前亮度
static uint8_t dir = 0;					 // 0=变亮 1=变暗
static uint32_t last_tick = 0;			 // 记录时间

// 红灯状态机
RedLED_State red_state = RED_OFF; 		// 初始熄灭
uint32_t red_timer = 0;
uint32_t send_keep_timer = 0; 	 		// 发送完成后保持闪烁的时间

/*===========函数声明==========*/
static void RGB_Breathing_Loop(void);
static void Red_LED_State_Machine(void);

/*===========主函数==========*/
int main(void)
{
	/* 模块初始化 */
	OLED_Init();
	Key_Init();
	MyCAN_Init();
	LED_Init();
	Timer_Init();
	IRSensor_Init();
	Serial_Init();
	
	/* 定义按键键码 */
	uint32_t KeyNum;
	
	/* CAN 发送参数 */
	uint32_t Tx_ID = 0x555;
	uint8_t Tx_Length = 4;
	uint8_t Tx_Data[8] = {0x11, 0x22, 0x33, 0x44};
	
	/* CAN 接收参数 */
	uint32_t Rx_ID;
	uint8_t Rx_Length;
	uint8_t Rx_Data[8]; 
	
	/* OLED 固定显示 */
	OLED_ShowString(1,1,"TxID:");
	OLED_ShowHexNum(1,6,Tx_ID,3);
	OLED_ShowString(2,1,"RxID:");
	OLED_ShowString(3,1,"Leng:");
	OLED_ShowString(4,1,"Data:");
	
	/* 初始化成功，打印日志到串口 */
	printf("Init Complete!\r\n");
	
	
	/*===========主循环==========*/
	while (1)
	{	
		Key_Scan();                         // 按键扫描
        RGB_Breathing_Loop();               // 蓝色呼吸灯
        Red_LED_State_Machine();            // 红灯状态机

		
		/* 按键发送CAN */
		KeyNum = Key_GetNum();
		if(KeyNum == 1)
		{
			red_state = RED_BLINK_SLOW;   	// 开始慢闪
            send_keep_timer = GetTick();  	// 记录开始时间
			Tx_Data[0]++;
			Tx_Data[1]++;
			Tx_Data[2]++;
			Tx_Data[3]++;
			MyCAN_Transmit(Tx_ID, Tx_Length, Tx_Data);
		}
		
		
		/* CAN接收处理 */
		if(RxFlagStatus == 1)
		{
			printf("CAN_Send Success!\r\n");
			RxFlagStatus = 0;
			
			// 解析ID
			if(RxMessage.IDE == CAN_Id_Standard)
			{
				Rx_ID = RxMessage.StdId;
			}
			else
			{
				Rx_ID = RxMessage.ExtId;
			}
			// 解析数据
			if(RxMessage.RTR == CAN_RTR_Data)
			{
				Rx_Length = RxMessage.DLC;
				for(uint8_t i = 0; i<Rx_Length; i++)
				{
					Rx_Data[i] = RxMessage.Data[i];
				}
			}
			else
			{
				//......
			}		
			
			// OLED显示
			OLED_ShowHexNum(2,6,Rx_ID,3);
			OLED_ShowHexNum(3,6,Rx_Length,1);
			OLED_ShowHexNum(4,6,Rx_Data[0],2);
			OLED_ShowHexNum(4,9,Rx_Data[1],2);
			OLED_ShowHexNum(4,12,Rx_Data[2],2);
			OLED_ShowHexNum(4,15,Rx_Data[3],2);
		}
		
		
		/* 超时自动关闭红灯 */
		if ((red_state == RED_BLINK_SLOW || red_state == RED_BLINK_FAST) && 
			(GetTick() - send_keep_timer >= 600))
		{
			red_state = RED_OFF;
			RedLED_OFF();
			red_timer = 0;
		}
	}
}


/* =============================================================
 *  @函数名：RGB_Breathing_Loop
 *  @功能：非阻塞蓝色呼吸灯（仅待机状态）
 * =========================================================== */
static void RGB_Breathing_Loop(void)
{
    if(red_state != RED_OFF) return;

    if(GetTick() - last_tick >= 10)
    {
        last_tick = GetTick();

        if(dir == 0)
        {
            pwm_val++;
            if(pwm_val >= 100) dir = 1;
        }
        else
        {
            pwm_val--;
            if(pwm_val <= 0) dir = 0;
        }
        PWM_SetCCR(pwm_val);
    }
}


/* =============================================================
 *  @函数名：Red_LED_State_Machine
 *  @功能：红灯状态机（慢闪 / 快闪）
 * =========================================================== */
static void Red_LED_State_Machine(void)
{
    if(red_state == RED_OFF) return;

    // 紧急日志输出
    if(IR_State == 1)
    {
        printf("Emergency!!!\r\n");
        IR_State = 0;
    }

    // 慢闪：200ms
    if(red_state == RED_BLINK_SLOW && (GetTick() - red_timer >= 200))
    {
        red_timer = GetTick();
        RedLED_Toggle();
    }

    // 快闪：70ms
    if(red_state == RED_BLINK_FAST && (GetTick() - red_timer >= 70))
    {
        red_timer = GetTick();
        RedLED_Toggle();
    }
}
