#include "jtag_swd.h"

void JTAG_Set(u8 mode) 
{ 
u32 temp; 
temp=mode; 
temp<<=25; 
RCC->APB2ENR|=1<<0;     //��������ʱ�ӱ�(AFIO)    
AFIO->MAPR&=0XF8FFFFFF; //���MAPR��[26:24] 
AFIO->MAPR|=temp;       //����jtagģʽ
}

void Free_JTAG(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			//��������ʱ��
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);	//�ͷ�JTAG����

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;			
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  				//����Ϊ�������
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}
