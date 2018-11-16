#ifndef __ANALOY_H
#define __ANALOY_H

#include "sys.h"
#include "Sys_Init.h"


/**************结构体定义***************/
typedef struct
{
	u16 DelayTimer; //模拟动作延时
	u8 Comend;     //模拟动作指令
	u8 Result;     //模拟动作结果
  u8 CANId;      //模拟目标设备的CAN总线ID
  u8 Action;     //模拟的动作
}struct_ANALOYCTRL;

/**************外部调用定义***************/
extern struct_ANALOYCTRL Analoy;

/**************函数声明***************/
void Analoy_Ctrl(void);




#endif
