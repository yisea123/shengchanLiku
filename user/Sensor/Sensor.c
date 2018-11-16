
#include "Sensor.h"
#include "led.h"

//RFID ����
u8 Status;		//����ƿ��ǩ���(ƿ��ǩ��ã���ID��Ч)		
u8 ATQ[2];
u8 SAK;
u8 Len;
u8 Bottle_id[4];
u8 RFID_FF_ct = 0;	//��RFID�������ζ���ff ff ff ff��RFID����IO��λһ�Σ�ZLG522S���³�ʼ��һ��
bool Bottle_NoRFID; //ƿ����RFID��ǩ���
bool NO_RFID_Bottle;
bool Bottle_Exist;
//#define MAN_CONTROL 1

_DEVICE_PROPERTY Device_Prop;	//�ṹ����� ����վ����
_DEVICE_PROPERTY Device[12];
void Sensor_Init(void)		//�������˿ڳ�ʼ��
{
	RCC->APB2ENR|=1<<2;			//ʹ��PORTAʱ��
	RCC->APB2ENR|=1<<4;			//ʹ��PORTCʱ��
	RCC->APB2ENR|=1<<5;			//ʹ��PORTDʱ��
	RCC->APB2ENR|=1<<6;			//ʹ��PORTEʱ��
	
	//�豸��ַ 
	GPIOD->CRH&=0X00000000;	//PD8~15  ��������
	GPIOD->CRH|=0X88888888;
	
	//������3��λ
	GPIOA->CRL&=0X000FFFFF;	//PA5~7   ��������(Ӳ������)
	GPIOA->CRL|=0X88800000;
	
	//ƿ���
	GPIOC->CRH&=0XFFFF0FFF;	//PC11 	  ��������
	GPIOC->CRH|=0X00008000;	
	GPIOD->CRL&=0XFFFFF0F0;	//PD0,PD2 ��������
	GPIOD->CRL|=0X00000808;
	
//	//΢������	�����ӽ�����		
//	GPIOE->CRL&=0XFFF00FFF;	//PE3,PE4 ��������
//	GPIOE->CRL|=0X00088000;
	
	//RFID����
	GPIOC->CRH&=0XFFFFF0FF;	//PC10 	������� �ٶ�50MHz
	GPIOC->CRH|=0X00000300;
	GPIOC->ODR|=1<<10;			//PC10 	�����	RFID_SEͨ��
	delay_ms(500);						
}

/******************************************************************************
�������ƣ�Read_Device_ID()
�����������豸��ַ��ȡ
*******************************************************************************/
void Read_Device_ID(void)
{
	Device_Prop.DEVICE_ID = ~(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_15) | GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14)<<1 | GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13)<<2 
													|GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12)<<3| GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_11)<<4 | GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_10)<<5
													|GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_9)<<6	| GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_8)<<7);
}
/***************************************************************
================================================================
@ ����վ����						����վ��ַ
----------------------------------------------------------------
@	����       				  0x01		      
@	�������    				0x10-0x1f		      
@	����վ      				0x20-0x2f		      
================================================================			
****************************************************************/
u8 Station_Type_Judge(void)		//����վ���Ͷ���
{	
//	if(Device_Prop.DEVICE_ID)		//����վID��Ϊ0
//	{
//			   if(Device_Prop.DEVICE_ID >= 0xA0 && Device_Prop.DEVICE_ID <= 0xAA)	return _ADS16;	//�Զ����� 110 160   ADS16
//		else if(Device_Prop.DEVICE_ID >= 0xAB && Device_Prop.DEVICE_ID <= 0xAF)
//		{ 
//			#if STATION_AMS16A
//			return _AMS16;	//���Զ��桢ȡ�� 160 AMS16
//			#endif
//			
//			#if STATION_AMS16D
//			return _AMS16D; //���Զ���ȡ�����޻�̨�汾
//			#endif
//		}			
//		else if(Device_Prop.DEVICE_ID >= 0xB0 && Device_Prop.DEVICE_ID <= 0xBA)
//		{
//			#if STATION_AES16A
//			return _AES16;	//�Զ�ȡ�� 110 160   AES16		
//			#endif
//			
//			#if STATION_AES16D
//			return _AES16D;	//�Զ�ȡ��	
//			#endif
//			
//			#if STATION_AES16F
//			return _AES16F;	//�Զ�ȡ�� 
//			#endif
//		}			
//		else if(Device_Prop.DEVICE_ID >= 0xBB && Device_Prop.DEVICE_ID <= 0xBF)	return _AMS16B;	
//		else if(Device_Prop.DEVICE_ID >= 0xC0 && Device_Prop.DEVICE_ID <= 0xCF)	return _AES16B;
//	}
//	else	//����վIDΪ0	
//	{		
//		return 0;
//	}
	return 0;
}

/******************************************************************************
================================��������״̬===================================
===============================================================================
						|	  7	  |   6   |	  5    |	  4	  |   3   |   2   |   1   |   0   |
-------------------------------------------------------------------------------
						| ��̨��|	��̨��|��������|��������|	����. |	ƿ��1/|	ƿ����|	����	|
						|	��λ	|	�͹�λ|	 ��λ2 |	��λ1	|				|			  |	������| ƿ����|
===============================================================================
�Զ�����	��|		*		|		*		|				 |				|				|		*		|		*		|		*		|
===============================================================================

*******************************************************************************/
u8 Station_Sensor_Read(void)		//����վ������״̬��ȡ(������վ����)
{
	/*****ģ��****/
	
	return 0;
}

