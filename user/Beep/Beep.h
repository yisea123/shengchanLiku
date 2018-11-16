#ifndef __BEEP_H
#define __BEEP_H

#include "stm32f10x.h"
#include "sys.h"
#include <stdbool.h>

#define Buzzer PCout(11)	//PC11
#define asdasdasd 1234
#define Buzzer_H  Buzzer = 1  
#define Buzzer_L	Buzzer = 0  


extern u8 Alarm_ct;		//鸣次数
extern u16 Alarm_Tim;	//鸣时间
extern bool Beep_Open;	//开蜂鸣器标记

void Beep_Init(void);	//初始化
void Beep(u16 UpT, u16 DwT, u8 ct); //蜂鸣器响次数
void Buzzer_Beep_Times(u16 alarm_time, u16 alarm_cycle, u8 alarm_ct);

#endif

