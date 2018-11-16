
#include "input.h"
//#include "led.h"
//#include "can.h"

_DEVICE_OBJECT Device_obj;	//�ṹ����� ����վ����

void GPIO_InPut_Init(void)		//�������˿ڳ�ʼ��
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5; //IO		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);				//��ʼ��GPIOB
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13; //������չ��X��ǰ��λPC13
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);				//��ʼ��GPIOC
		
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7|GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_12 | GPIO_Pin_14|GPIO_Pin_15; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOE, &GPIO_InitStructure);			
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_15 | GPIO_Pin_11 | GPIO_Pin_6 | GPIO_Pin_9; //������չ�壬PE:3X�����λ��PE15:Z�Ἣ��λ��PE11:A�洫����,PE6:B�洫����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOE, &GPIO_InitStructure);	
}


void Send_Sensor_Status(void)		//������״̬�仯�ϴ�
{

}
