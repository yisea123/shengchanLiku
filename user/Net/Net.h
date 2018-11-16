#ifndef _NET_H_
#define _NET_H_

#include "Sys_Init.h"
#include <stdbool.h>

typedef unsigned char SOCKET;			//自定义端口号数据类型


void W5500_Initialization(void);
void Load_Net_Parameters(void);
void W5500_Socket_Set(void);
void Process_Socket_Data(SOCKET s);
void Net_Receive(SOCKET s);
void Net_Send(u8 CAN_id,u8 cmd_num,u8 cmd_data,u8 rec_num,u8 *rec_data);
void Net_SendBuffer(void);
void Net_Analysis(void);
void Net_Answer(u8 CAN_id,u8 Cmd);




/***********网络接收到的数据结构定义**************/
#define RECNUM      		 		 10   //最大记录数量  行数
#define RECBUFFERNUM      	 30   //接收网络数据缓存条数
typedef struct
{
  unsigned char RecNum;//记录数N  总数据量为8*(N+1)
	unsigned char Comend;//功能指令
	unsigned char NetNum;//网关地址
	unsigned char PROT_CFG;//CAN总线地址
	unsigned char ComData;//指令数据
	unsigned char RecNumBuff[RECNUM][8];//记录数据
}struct_RECEIVENET;
extern volatile struct_RECEIVENET NetRecBuff[RECBUFFERNUM];
#define SENDBUFFERNUM       10   //发送网络数据缓存条数
typedef struct
{
	unsigned char RecNum;//记录数N  总数据量为8*(N+1)
	unsigned char Comend;//功能指令
	unsigned char NetNum;//网关地址
	unsigned char PROT_CFG;//CAN总线地址
	unsigned char ComData;//指令数据
	unsigned char RecNumBuff[RECNUM][8];//记录数据
}struct_SENDNET;

extern u8 NowSendNetNum;	//当前发送缓存的网络数据条数


#endif

