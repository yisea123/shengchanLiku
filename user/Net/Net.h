#ifndef _NET_H_
#define _NET_H_

#include "Sys_Init.h"
#include <stdbool.h>

typedef unsigned char SOCKET;			//�Զ���˿ں���������


void W5500_Initialization(void);
void Load_Net_Parameters(void);
void W5500_Socket_Set(void);
void Process_Socket_Data(SOCKET s);
void Net_Receive(SOCKET s);
void Net_Send(u8 CAN_id,u8 cmd_num,u8 cmd_data,u8 rec_num,u8 *rec_data);
void Net_SendBuffer(void);
void Net_Analysis(void);
void Net_Answer(u8 CAN_id,u8 Cmd);




/***********������յ������ݽṹ����**************/
#define RECNUM      		 		 10   //����¼����  ����
#define RECBUFFERNUM      	 30   //�����������ݻ�������
typedef struct
{
  unsigned char RecNum;//��¼��N  ��������Ϊ8*(N+1)
	unsigned char Comend;//����ָ��
	unsigned char NetNum;//���ص�ַ
	unsigned char PROT_CFG;//CAN���ߵ�ַ
	unsigned char ComData;//ָ������
	unsigned char RecNumBuff[RECNUM][8];//��¼����
}struct_RECEIVENET;
extern volatile struct_RECEIVENET NetRecBuff[RECBUFFERNUM];
#define SENDBUFFERNUM       10   //�����������ݻ�������
typedef struct
{
	unsigned char RecNum;//��¼��N  ��������Ϊ8*(N+1)
	unsigned char Comend;//����ָ��
	unsigned char NetNum;//���ص�ַ
	unsigned char PROT_CFG;//CAN���ߵ�ַ
	unsigned char ComData;//ָ������
	unsigned char RecNumBuff[RECNUM][8];//��¼����
}struct_SENDNET;

extern u8 NowSendNetNum;	//��ǰ���ͻ����������������


#endif

