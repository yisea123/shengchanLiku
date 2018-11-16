#ifndef __LED_H
#define __LED_H	

#include "sys.h"
#include <stdbool.h>

#define LED 	PCout(2)	//ACU���İ�ָʾ��	 	PC2	
#define LED_R	PCout(0)	//ACU�װ�ָʾ��	 ��	PC0
#define LED_G	PCout(1)	//							 �� PC1

#define LEDH	LED = 1
#define LEDL	LED = 0

#define LED_RH	LED_R = 1
#define LED_RL	LED_R = 0

#define AES16F_R_H    SM_EN = 1   //AES16F����õ�����ƶ��ӵ�EN�ſ���
#define AES16F_R_L    SM_EN = 0   //AES16F����õ�����ƶ��ӵ�EN�ſ���

#define LED_GH	LED_G = 1
#define LED_GL	LED_G = 0

#define DEFAULT() {LED_RL; LED_GL;}	//Ĭ��״̬���������� 
#define NORMAL()	{LED_RL; LED_GH;}	//������״̬����������	
#define FAULT()		{LED_RH; LED_GL;}	//���ϵ�״̬������������	
#define BOTPASS()	{LED_RH; LED_GH;}	//ƿͨ��ƿ���������������һ��  	���Զ���������ƿ������	

#define LED_ON  1
#define LED_OFF 0

extern u16 Timer_LEDFlash_count;  //LED��˸���Ƽ�ʱ��
extern u16 LED_G_OnTime;          //�̵���˸����ʱ��
extern u16 LED_G_OffTime;         //�̵���˸���ʱ��
extern u8  LED_G_Ctrl;            //�̵ƿ��� 0������ 1������	2����˸ 
extern u8  LED_R_Ctrl;            //��ƿ��� 0������ 1������	2����˸ 

void LED_Init(void); //��ʼ��
void LED_Ctrl(void);
void AMS16D_LEDCTRL(void);



//void LED_Flash_Update(u8 _ledno);
		 				    
#endif /* __LED_H */
