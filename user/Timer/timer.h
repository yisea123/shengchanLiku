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
extern u16 IC_RevTime;	//RFIC��ʱ
extern u8  TestModeFlash_count;   //����ģʽ��˸����
extern u16 ReadSensorTime_100ms;
extern u16 Timer_TestModeFlash;   //����ģʽ��˸��ʱ��    
extern u16 MotorCtrlTime_1ms;     //�������ʱ��
extern u16 MotorCtrlTime_Total;     //�������ʱ��
extern unsigned int W5500_Send_Delay_Counter; //W5500������ʱ��������(ms)
extern u8 Timer_10s_flag;
extern u8 Timer_5s_flag;
extern u8 Timer_1ms_flag;
extern u8 Timer_5ms_flag;
extern u16 DelayTimes_ms;
void TIM_delay_ms(u32 tms);						//��ʱ����ʱ ms  
void TIM3_Int_Init(u16 arr,u16 psc);	//TIM3
void TIM2_Int_Init(u16 arr,u16 psc);	//TIM2

#endif
