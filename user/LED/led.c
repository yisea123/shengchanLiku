#include "led.h"
#include "timer.h"

//-- Private variable definitions --
u16 Timer_LEDFlash_count;  //LED��˸���Ƽ�ʱ��
u16 LED_G_OnTime;          //�̵���˸����ʱ��
u16 LED_G_OffTime;         //�̵���˸���ʱ��
u8  LED_G_Ctrl;            //�̵ƿ��� 0������ 1������	2����˸ 
u8  LED_R_Ctrl;            //��ƿ��� 0������ 1������	2����˸ 
//LED IO��ʼ��
void LED_Init(void)
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //ʹ��GPIOB�˿�ʱ��

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	 	
	GPIO_SetBits(GPIOC,GPIO_Pin_2);
}

