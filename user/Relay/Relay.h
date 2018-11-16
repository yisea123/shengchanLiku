#ifndef __RELAY_H
#define	__RELAY_H

#include "sys.h"

#define Relay PCout(13) 	//继电器IO定义

#define Relay_H		Relay = 1
#define Relay_L		Relay = 0	
 
void Relay_Act(void);		//继电器动作
u8 Relay_Return(void);
void Relay_Ana(void);
void Relay_Act_PLC(void); //继电器信号告知PLC可以传送瓶子

#endif
