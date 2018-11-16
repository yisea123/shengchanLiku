
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
	RCC->APB2ENR|=1<<4;			//ʹ��PORTCʱ��
	
	GPIOC->CRH&=0XFF0FFFFF;	//PC13 �������	�ٶ�10MHz 
	GPIOC->CRH|=0X00100000;
		
	GPIOC->ODR|=1<<13;			//PC13 ����ߣ��̵����Ͽ�	
}

void Open_FAR_Door(void) 
{
	if(Sensor_Lock == 0)  //������ڹ���״̬
	{
		Relay_L;				//���� 
		delay_ms(150);
		Relay_H;
	}
}
