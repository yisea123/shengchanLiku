#ifndef __KEY_H
#define __KEY_H

#include "sys_init.h"
#include "sys.h"
#include "stdbool.h"

#define KEY1		PDin(7)		//����1	
#define KEY2		PDin(4)		//����2 
#define KEY3		PDin(5)		//����3 
#define KEY4		PDin(2)		//����4 
#define KEY5		PDin(3)		//����5	
#define KEY6		PDin(0)		//����6	
//#define KEY7		PCin(8)		//����7	Ԥ��				����ģʽ���ܼ�-
//#define KEY8		PCin(9)		//����8	Ԥ�� 				����ģʽ��̨��λ��


extern bool Bottle_Judge_Record; //��Сƿ��¼ģʽ����(KEY4 + ��λ��)����	
extern u8 TEST_MODE; 					 //����ģʽ��ǣ���(KEY4 + ��λ��)����	
extern u8 TEST_MODE_SAVE;      
extern u16 Time_PushRod_Ctrl;   

void KEY_Init(void);  //����IO��ʼ�� 
void Key_Scan(void);		//������ȡ
void Key_Deal(void);	//������ȡ

#endif

