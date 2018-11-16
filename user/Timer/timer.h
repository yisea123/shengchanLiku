#ifndef __TIMER_H
#define __TIMER_H

#include "sys.h"
#include "sys_init.h"
#include "stm32f10x.h"
#include "stdbool.h"

typedef struct
{
	u16 Timer_cnt;
}TYPE_ALARM;
extern u16 IC_RevTime;	//RFIC超时
extern u8  TestModeFlash_count;   //工程模式闪烁次数
extern u16 ReadSensorTime_100ms;
extern u16 Timer_TestModeFlash;   //工程模式闪烁定时器    
extern u16 MotorCtrlTime_1ms;     //电机控制时间
extern u16 MotorCtrlTime_Total;     //电机控制时间
extern unsigned int W5500_Send_Delay_Counter; //W5500发送延时计数变量(ms)
extern u8 Timer_10s_flag;
extern u8 Timer_5s_flag;
extern u8 Timer_1ms_flag;
extern u8 Timer_5ms_flag;
extern u16 DelayTimes_ms;
void TIM_delay_ms(u32 tms);						//定时器延时 ms  
void TIM3_Int_Init(u16 arr,u16 psc);	//TIM3
void TIM2_Int_Init(u16 arr,u16 psc);	//TIM2

#endif
