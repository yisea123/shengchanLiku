
#ifndef __INPUT_H
#define __INPUT_H

#include "sys.h"
#include <stdbool.h>

#define Bit_Band	//����ʹ��λ������

//=============================����������===============================//
//����վ���� �ṹ��
typedef struct
{
	u8 DEVICE_TYPE;		//����վ����  
	u8 DEVICE_ID;			//����վID
	u8 DEVICE_STATUS;	//����վ״̬		
	u8 Code_Number;		//PCB���ϲ����ַ	bit0~bit7
}_DEVICE_OBJECT;

extern _DEVICE_OBJECT Device_obj;


//==============================����������================================//
#ifdef Bit_Band
//P14����ģ��
#define P5_IN			PEin(8)  	//P14-1
#define P6_IN			PEin(10)  //P14-2
#define P7_IN			PEin(12)  //P14-3
#define P32_IN		PEin(14)  //P14-4
//P23����ģ��
#define P28_IN		PBin(5)  	//P23-1
#define P29_IN		PBin(4)  	//P23-2
#define P30_IN		PBin(3)  	//P23-3
#define P31_IN		PDin(6)  	//P23-4

#else
#define P5_IN		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8) 
#define P6_IN		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_10)
#define P7_IN		GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12) 
#define P32_IN	GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14) 

#define P28_IN	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5) 
#define P29_IN	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4) 
#define P30_IN	GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3) 
#define P31_IN	GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_6) 
#endif

//=============================����������=================================//
void GPIO_InPut_Init(void);		//����IO��ʼ��
void Send_Sensor_Status(void);//������״̬�仯�ϴ�

#endif 

