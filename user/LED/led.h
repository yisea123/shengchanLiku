#ifndef __LED_H
#define __LED_H	

#include "sys.h"
#include <stdbool.h>

#define LED 	PCout(2)	//ACU核心板指示灯	 	PC2	
#define LED_R	PCout(0)	//ACU底板指示灯	 红	PC0
#define LED_G	PCout(1)	//							 绿 PC1

#define LEDH	LED = 1
#define LEDL	LED = 0

#define LED_RH	LED_R = 1
#define LED_RL	LED_R = 0

#define AES16F_R_H    SM_EN = 1   //AES16F红灯用电机控制端子的EN脚控制
#define AES16F_R_L    SM_EN = 0   //AES16F红灯用电机控制端子的EN脚控制

#define LED_GH	LED_G = 1
#define LED_GL	LED_G = 0

#define DEFAULT() {LED_RL; LED_GL;}	//默认状态：红灭，绿灭 
#define NORMAL()	{LED_RL; LED_GH;}	//正常灯状态：红灭，绿亮	
#define FAULT()		{LED_RH; LED_GL;}	//故障灯状态：红亮，绿灭	
#define BOTPASS()	{LED_RH; LED_GH;}	//瓶通过瓶满传感器：红灯闪一下  	半自动存样：有瓶，门锁	

#define LED_ON  1
#define LED_OFF 0

extern u16 Timer_LEDFlash_count;  //LED闪烁控制计时器
extern u16 LED_G_OnTime;          //绿灯闪烁亮的时间
extern u16 LED_G_OffTime;         //绿灯闪烁灭的时间
extern u8  LED_G_Ctrl;            //绿灯控制 0：长灭 1：长亮	2：闪烁 
extern u8  LED_R_Ctrl;            //红灯控制 0：长灭 1：长亮	2：闪烁 

void LED_Init(void); //初始化
void LED_Ctrl(void);
void AMS16D_LEDCTRL(void);



//void LED_Flash_Update(u8 _ledno);
		 				    
#endif /* __LED_H */
