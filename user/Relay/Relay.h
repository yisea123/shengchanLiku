#ifndef __RELAY_H
#define	__RELAY_H

#include "sys.h"

#define Relay PCout(13) 	//�̵���IO����

#define Relay_H		Relay = 1
#define Relay_L		Relay = 0	
 
void Relay_Act(void);		//�̵�������
u8 Relay_Return(void);
void Relay_Ana(void);
void Relay_Act_PLC(void); //�̵����źŸ�֪PLC���Դ���ƿ��

#endif
