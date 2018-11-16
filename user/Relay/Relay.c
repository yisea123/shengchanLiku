
#include "Relay.h"
#include "Sensor.h"
#include "led.h"
#include "delay.h"
#include "can.h"

typedef enum
{
	_FAS_RELAY_1 = 1,
	_FAS_RELAY_2,
	_HAS_RELAY_3,
	_HAS_RELAY_4
	
}RELAY_ACT;

void Relay_Init(void)
{	
	RCC->APB2ENR|=1<<4;			//使能PORTC时钟
	
	GPIOC->CRH&=0XFF0FFFFF;	//PC13 推挽输出	速度10MHz 
	GPIOC->CRH|=0X00100000;
		
	GPIOC->ODR|=1<<13;			//PC13 输出高，继电器断开	
}

void Open_FAR_Door(void) 
{
	if(Sensor_Lock == 0)  //如果处于关门状态
	{
		Relay_L;				//开门 
		delay_ms(150);
		Relay_H;
	}
}
