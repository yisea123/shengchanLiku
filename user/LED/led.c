#include "led.h"
#include "timer.h"

//-- Private variable definitions --
u16 Timer_LEDFlash_count;  //LED闪烁控制计时器
u16 LED_G_OnTime;          //绿灯闪烁亮的时间
u16 LED_G_OffTime;         //绿灯闪烁灭的时间
u8  LED_G_Ctrl;            //绿灯控制 0：长灭 1：长亮	2：闪烁 
u8  LED_R_Ctrl;            //红灯控制 0：长灭 1：长亮	2：闪烁 
//LED IO初始化
void LED_Init(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能GPIOB端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	 	
	GPIO_SetBits(GPIOC,GPIO_Pin_2);
}

