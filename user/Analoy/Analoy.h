#ifndef __ANALOY_H
#define __ANALOY_H

#include "sys.h"
#include "Sys_Init.h"


/**************�ṹ�嶨��***************/
typedef struct
{
	u16 DelayTimer; //ģ�⶯����ʱ
	u8 Comend;     //ģ�⶯��ָ��
	u8 Result;     //ģ�⶯�����
  u8 CANId;      //ģ��Ŀ���豸��CAN����ID
  u8 Action;     //ģ��Ķ���
}struct_ANALOYCTRL;

/**************�ⲿ���ö���***************/
extern struct_ANALOYCTRL Analoy;

/**************��������***************/
void Analoy_Ctrl(void);




#endif
