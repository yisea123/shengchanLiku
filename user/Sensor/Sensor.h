
#ifndef __SENSOR_H
#define __SENSOR_H

#include "sys.h"
#include <stdbool.h>

//=============================����������===============================//
//RFID ����
extern u8 Status;			//����ƿ��ǩ���(ƿ��ǩ��ã���ID��Ч)		
extern u8 ATQ[2];
extern u8 SAK;
extern u8 Len;
extern u8 Bottle_id[4];
extern u8 RFID_FF_ct;	//��RFID�������ζ���ff ff ff ff��RFID����IO��λһ�Σ�ZLG522S���³�ʼ��һ��
extern bool Bottle_NoRFID; //ƿ����RFID��ǩ���
extern bool NO_RFID_Bottle;
extern bool Bottle_Exist;

//����վ���� �ṹ��
typedef struct
{
	u8 DEVICE_TYPE;		//����վ����
	u8 DEVICE_ID;			//����վID
	u8 DEVICE_STATUS;	//����վ״̬	
}_DEVICE_PROPERTY;

extern _DEVICE_PROPERTY Device_Prop;
extern _DEVICE_PROPERTY Device[12];

//����վ����
typedef enum
{
	_ADS16 = 0x01,//�Զ�����
	
	_AMS16,				//���Զ�����
	_AMS16B,		  //�������գ��뿪Ԫ�Խӹ���վ��
	_AMS16D,      //�°��˹���������
	
	_AES16,				//�Զ�ȡ��
	_AES16B,			//��ƿ����
	_AES16D,      //�Զ�ȡ�� �޻������汾
	_AES16F       //�Զ�ȡ�� �޻������汾���޻���汾
}_STATION_TYPE;

//******ȫ�Զ�ȡ��վѡ��*****************
#define STATION_AES16A 0  //�л��������л���汾
#define STATION_AES16D 1  //�޻��������л���汾��������
#define STATION_AES16F 0  //�޻��������޻���汾�����Ƹ�

//******���Զ���ȡ��վѡ��***************
#define STATION_AMS16A 0  
#define STATION_AMS16D 1

//==============================����������================================//
//===============================RFID����=================================//
#define RFID_SE 	PCout(10)
#define RFID_SEH	RFID_SE = 1
#define RFID_SEL	RFID_SE = 0

//============================ADS16==============================//
#define Sensor_Bottlefull    PCin(11)   //ƿ��������    -P9
#define Sensor_HeightTest    PDin(0)    //ƿ�߼�⴫����-P10
#define Sensor_BottlecapTest PDin(2)	  //ƿ�Ǽ�⴫����-P11
#define Sensor_MotorZero     PEin(3)    //�����λ������-P14
#define Sensor_SendPosition  PEin(4)    //����λ������  -P15

//============================AES16D==============================//
#define Sensor_Rod_Push			 PAin(5)    //�Ƹ��ƹ�λ    -P7#1
#define Sensor_Rod_Back			 PAin(6)    //�Ƹ��չ�λ    -P7#2
#define Sensor_Bottlefull    PCin(11)   //ƿ��������    -P9
#define Sensor_Pipe			     PDin(0)    //�ܵ��ڴ�����  -P10
#define Sensor_MotorDown     PDin(2)    //����¹�λ������-P11 - �¹�λ
#define Sensor_MotorZero     PEin(3)    //�����λ������-P14 - �Ϲ�λ
#define Sensor_Lock				   PEin(4)    //�����������  -P15   0���� 1����

//============================AES16A==============================//
#define Sensor_Valve_Close	 PAin(5)    //�������ع�λ  -P7#1
#define Sensor_Valve_Open		 PAin(6)    //����������λ  -P7#2
#define Sensor_Bottlefull    PCin(11)   //ƿ��������    -P9
#define Sensor_Pipe			     PDin(0)    //�ܵ��ڴ�����  -P10
#define Sensor_MotorZero     PEin(3)    //�����λ������-P14 - ��λ
#define Sensor_Lock				   PEin(4)    //�����������  -P15   0���� 1����

//============================AMS16D==============================//
#define Sensor_ShortBottle   PDin(0)    //��ƿ������    -P10
#define Sensor_Key           PDin(2)    //�󰴼�����    -P11 
#define Sensor_HighBottle    PEin(3)    //��ƿ������    -P14 
#define Sensor_Lock				   PEin(4)    //�����������  -P15   0���� 1����

//============================AES16F==============================//
#define Sensor_Rod_Push			 PAin(5)    //�Ƹ��ƹ�λ    -P7#1
#define Sensor_Rod_Back			 PAin(6)    //�Ƹ��չ�λ    -P7#2
#define Sensor_Bottlefull    PCin(11)   //ƿ��������    -P9
#define Sensor_Pipe			     PDin(0)    //�ܵ��ڴ�����  -P10
#define Sensor_MotorZero     PEin(3)    //�����λ������-P14 - ��λ
#define Sensor_Lock				   PEin(4)    //�����������  -P15   0���� 1����

//============================AMS16B==============================//
#define Sensor_Pipe			     PDin(0)    //��ƿ�ź�      -P10
#define Sensor_KY 				   PEin(4)    //��Ԫæ�ź�    -P15   0���� 1����


//=============================����������=================================//

void Sensor_Init(void);				//�������˿ڳ�ʼ��
void Read_Device_ID(void);		//��ȡ�豸ID
u8 Station_Type_Judge(void);	//����վ�����ж�
u8 Station_Sensor_Read(void);	//����վ״̬��ȡ
void Send_Sensor_Status(void);//������״̬�仯�ϴ�

void BottleID_CoalInfo_Read(void);		//ƿID��ú����Ϣ��ȡ
void FAS_HAS_Bottle_ID_Send(void);		//�Զ������Զ���������վƿID����   

#endif /*__SENSOR_H */

