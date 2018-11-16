
#include "Beep.h"
#include "delay.h"

u8 Alarm_ct;		//鸣次数
u16 Alarm_Tim;	//鸣时间
bool Beep_Open;	//开蜂鸣器标记

//初始化PC0为输出口.并使能这个口的时钟		    
//蜂鸣器初始化
void Beep_Init(void)
{
	RCC->APB2ENR|=1<<4;			//使能PORTC时钟
	
	GPIOC->CRH&=0XFFFF0FFF;	//PC11 推挽输出	速度50MHz 
	GPIOC->CRH|=0X00003000; //
	
  GPIOC->ODR|=0<<11;      //PC11 输出0，关闭蜂鸣器输出
}

//蜂鸣器	延时
//UpT:高电平时间  	DwT：低电平时间    ct：次数
void Beep(u16 UpT, u16 DwT, u8 ct)
{
	u8 i;	//次数
	for(i = 0; i < ct; i++)
	{
		Buzzer = 1;
		delay_ms(UpT);
		Buzzer = 0;
		delay_ms(DwT);
	}
}

