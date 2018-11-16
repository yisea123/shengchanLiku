/*************************************************************
*	2010-2020 ���Ż�˶�Ƽ����޹�˾	JESOO 
*	All rights reserved
*	��Ŀ���ƣ�JSQD01_SCU-DCU V0.1
*	�ļ����ƣ�main.c
*	��Ҫ������	
*	��ǰ�汾��0.1
*	����/�޸��ߣ��ܸ�ǿ
*	���ڣ�2020.01.20

*************************************************************/
#ifndef         __I2C_H__ 
#define         __I2C_H__


//#include "main.H"
#include "sys_init.h"
#include "sys.h"
#include <stdbool.h>
#define uchar  unsigned char
#define schar  signed char
#define uint   unsigned int
#define sint   signed int
#define slong  signed long
#define ulong  unsigned long

#define LOW 0
#define HIGH 1
#define SCL PEout(7)
#define SDA PEin(9)
#define SDA_out PEout(9)
extern void Start_I2C(void);
extern void Stop_I2C(void);
// extern void Init_I2C(void);
bool Bit_I2C_Clock(void);

extern unsigned char bytRec_Byte(unsigned char ack);
bool bolSend_Byte(unsigned char Data_Sended);



#endif



