#ifndef __MOTOR_H
#define __MOTOR_H

#include "sys.h"
#include <stdbool.h>


/**********�궨��**********/
#define STEPMOTO_X  0
#define STEPMOTO_Y  1
#define STEPMOTO_Z  2
#define STEPMOTO_W  3
#define STEPMOTO_V  4

#define _FAS_DISTANCE	     110000  //�Զ�����
#define POSITION_ZERO      0      //��λ����ֵ
#define POSITION_SEND      110000 //����λ����ֵ
#define POSITION_BOTTLECAP 110010 //�����ƶ���ƿ�Ǽ��λ�Ĳ�����Ӧ����Сƿƿ�ǵĲ���
                                  //Ϊ�˱���������㵽��ֵ������Ϊ���������������ֵ
																	
#define AES16A_DISTANCE    165000  //�Զ�ȡ������վ(�л�����)�г�			

#define AES16D_DISTANCE    200000  //�Զ�ȡ������վ(�޻�����)�г�																	
#define AES16D_Suction     40000   //�Զ�ȡ������վ����������̨λ��											
																	
#define AES16F_DISTANCE    61500
#define AES16F_LAYER_1     61500
#define AES16F_LAYER_2     40800	
#define AES16F_LAYER_3     19900
#define AES16F_Suction     10000   //�Զ�ȡ������վ����������̨λ��											
#define AES16F_LAYER_SEND  0
	
	
#define DISTANCE_CAP_TO_CAPTEST 25000 //��ƿ��λ�õ�ƿ�Ǽ���Ӧ�ߵľ���
#define AMpulnum			     5600	  //ƿ���ж�����������
#define RFID_Read_Block    0x08  //RFID��ȡ������

/*********ȫ�ֱ�������**********/
extern u32 Addnum;						//����(�����ۼ�)
extern u32 Pulplus;	
extern u16 BadBottle_Reject_timer; //����ƿ�޳���ʱʱ��
extern bool First_Bootlecaptest; //ƿ�ǲ���

//===================== ��̨�������������IO�ڶ����� =============================//

#define SM_EN			PBout(13)	//���ʹ�ܶ�	PB.13
#define SM_DIR		PBout(14)	//����				PB.14
#define SM_PUL		PBout(15)	//����				PB.15

#define SM_ENH		SM_EN = 1
#define SM_ENL		SM_EN = 0

#define SM_DIRH		SM_DIR = 1	
#define SM_DIRL		SM_DIR = 0

#define SM_PULH		SM_PUL = 1
#define SM_PULL		SM_PUL = 0

#define AES16F_BRAKE_H   LED_R = 1   //AES16F ɲ���ú�ƶ˿ڿ���
#define AES16F_BRAKE_L   LED_R = 0   
//======================== Ƥ�����IO�ڶ����� ==============================//

//������L6203��������	
#define MO_EN 		GPIO_Pin_12		//PE12	ʹ��		
#define MO_SE			GPIO_Pin_12		//PC12	�������
#define MO_IN1 		GPIO_Pin_14		//PE14
#define MO_IN2 		GPIO_Pin_15		//PE15

//������ſ���  
#define MENH      GPIO_WriteBit(GPIOE, MO_EN, Bit_SET)
#define MENL      GPIO_WriteBit(GPIOE, MO_EN,	Bit_RESET)
#define MIN1H     GPIO_WriteBit(GPIOE, MO_IN1, Bit_SET)
#define MIN1L     GPIO_WriteBit(GPIOE, MO_IN1, Bit_RESET)
#define MIN2H     GPIO_WriteBit(GPIOE, MO_IN2, Bit_SET)
#define MIN2L     GPIO_WriteBit(GPIOE, MO_IN2, Bit_RESET)
#define Read_OC   GPIO_ReadInputDataBit(GPIOC, MO_SE)		//�������

//�������
#define Motor_F   {MIN1H; MIN2L; MENH;} //�����ת F
#define Motor_R   {MIN1L; MIN2H; MENH;} //�����ת R
#define Motor_S   {MIN1L; MIN2L; MENH;} //ͣת����ɲ����
#define Motor_OFF	{MIN1L; MIN2L; MENL;} //ͣת����ɲ����


//======================== ���������ٵ��IO�ڶ����� ==============================//
#define MotoGobackPul_NORMAL  4000        //����λ�����߲���1CM
#define MotoGobackPul_AES16F  400        //����λ�����߲���1CM

//#define SPEED             35
#define SPEED_AES16F      140

#define DERICT_FRONT      0
#define DERICT_BACK       1

#define ROD_PUSH          1   //�Ƹ���
#define ROD_BACK          2   //�Ƹ���

#define VALVE_CLOSE       1   //��������
#define VALVE_OPEN        2   //��������
//============================== ���������� ======================================//
void Motor_Init(void);				//���IO��ʼ��
unsigned char StepperMoto_42(unsigned long StepNum,unsigned char Derict);
void MoveToPosition(u32 Position);
u8 BadBottle_Reject(void);
u8 PushRod_Ctrl(u8 Position, u8 Speed);
u8 Valve_Ctrl(u8 Position);
u16 Motor_SpeedCtrl(u32 OverallStep,u32 NowStep);
u8 Rod_Ctrl_AES16F(u8 Position);
void Rod_Ctrl_PUSH_AES16F(void);
#endif

