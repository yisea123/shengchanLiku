#ifndef __CAN_H_
#define __CAN_H_

//#include "stm32f10x.h"
#include "Sys_Init.h"

/************************can  波特率定义***********************************/
#define  baudrate_62_5K  36
#define  baudrate_125k   18
#define  baudrate_250k   9



//CAN接收的数据
typedef struct
{
	unsigned char dlc;	          //数据长度		  
	unsigned char DES_ID;	      //目的地址
	unsigned char SRC_ID;		  //源地址
	unsigned char PROT_CFG;		  //帧类型
	unsigned char ATS_CMD; 		  //命令
	unsigned char Cdata[8]; 	  //数据
	unsigned char BottleColeData[8];//每个瓶的煤样信息有8个BCD码数据 
}CANREC_typedef;//CAN接收的数据

//CAN发送的数据
typedef struct
{
	unsigned char dlc;	          //数据长度		  
	unsigned char DES_ID;	      //源地址
	unsigned char SRC_ID;		  //目的地址
	unsigned char PROT_CFG;		  //帧类型
	unsigned char ATS_CMD; 		  //命令
	unsigned char Cdata[8]; 		  //数据
}struct_CANDATA;
typedef union
{
	struct_CANDATA CanData;
	unsigned char CanSenBuf[13];
}UnionCan_TypeDef;
extern UnionCan_TypeDef CANSend;//CAN数据 //CAN发送的数据
extern struct_CANDATA CANSendBuffer[40];


extern CANREC_typedef CANRead;	//CAN接收数据
extern u8 RoadRecheck_ID;				//待查路径探测器/四向转换器ID
extern u8 RecheckTimer_100ms;		//重新查询设备状态计时			
extern u16 ReSendCantime_ms;

/*************************can  函数声明*******************************/

u8 canPoll(void);			         //检测can 接收中断
void init_can(u8 baudrate);			 //初始化can 输入参数：can波特率
void SendMessage(u8 SendData[]);		 // can 发送报文
void SendMessage_BigData(u8 *SendData);	// can 发送长报文
u8 canRead(u8 n, u8 *can_id, u8 *Data);      //接收can报文
void set_can_ID(u8 filternumber,u32 can_Stdid,u32 can_Extid,u32 can_markid ); //can    滤波器设置
//void REC_Analyze(void);
void _CAN_Init(u8 baudrate);

//=============应用层函数====================//
void SURE_REC_Analyze(void);
/*****************************************************************************/
//   HEX TO BCD
/****************************************************************************/
unsigned char HexToBcd(unsigned int Hex);

/*****************************************************************************/
//   ASCII TO INT
/****************************************************************************/
unsigned int ASCIITOInt(unsigned char ASCII1,unsigned char ASCII2,unsigned char ASCII3,unsigned char ASCII4);
void CAN_SendData(u8 Dlc, u8 Des_ID,u8 Cmd, u8 *Data);
void CAN_SendBuffer_1ms(void);
void CAN_SendDataToBuff(u8 *SendData);



#endif
