#ifndef __KEY_H
#define __KEY_H

#include "sys_init.h"
#include "sys.h"
#include "stdbool.h"

#define KEY1		PDin(7)		//按键1	
#define KEY2		PDin(4)		//按键2 
#define KEY3		PDin(5)		//按键3 
#define KEY4		PDin(2)		//按键4 
#define KEY5		PDin(3)		//按键5	
#define KEY6		PDin(0)		//按键6	
//#define KEY7		PCin(8)		//按键7	预留				测试模式功能键-
//#define KEY8		PCin(9)		//按键8	预留 				测试模式滑台复位键


extern bool Bottle_Judge_Record; //大小瓶记录模式，由(KEY4 + 复位键)触发	
extern u8 TEST_MODE; 					 //测试模式标记，由(KEY4 + 复位键)触发	
extern u8 TEST_MODE_SAVE;      
extern u16 Time_PushRod_Ctrl;   

void KEY_Init(void);  //按键IO初始化 
void Key_Scan(void);		//按键获取
void Key_Deal(void);	//按键读取

#endif

