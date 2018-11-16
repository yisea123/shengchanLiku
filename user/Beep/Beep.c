
#include "Beep.h"
#include "delay.h"

u8 Alarm_ct;		//������
u16 Alarm_Tim;	//��ʱ��
bool Beep_Open;	//�����������

//��ʼ��PC0Ϊ�����.��ʹ������ڵ�ʱ��		    
//��������ʼ��
void Beep_Init(void)
{
	RCC->APB2ENR|=1<<4;			//ʹ��PORTCʱ��
	
	GPIOC->CRH&=0XFFFF0FFF;	//PC11 �������	�ٶ�50MHz 
	GPIOC->CRH|=0X00003000; //
	
  GPIOC->ODR|=0<<11;      //PC11 ���0���رշ��������
}

//������	��ʱ
//UpT:�ߵ�ƽʱ��  	DwT���͵�ƽʱ��    ct������
void Beep(u16 UpT, u16 DwT, u8 ct)
{
	u8 i;	//����
	for(i = 0; i < ct; i++)
	{
		Buzzer = 1;
		delay_ms(UpT);
		Buzzer = 0;
		delay_ms(DwT);
	}
}

