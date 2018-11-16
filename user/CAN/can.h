#ifndef __CAN_H_
#define __CAN_H_

//#include "stm32f10x.h"
#include "Sys_Init.h"

/************************can  �����ʶ���***********************************/
#define  baudrate_62_5K  36
#define  baudrate_125k   18
#define  baudrate_250k   9



//CAN���յ�����
typedef struct
{
	unsigned char dlc;	          //���ݳ���		  
	unsigned char DES_ID;	      //Ŀ�ĵ�ַ
	unsigned char SRC_ID;		  //Դ��ַ
	unsigned char PROT_CFG;		  //֡����
	unsigned char ATS_CMD; 		  //����
	unsigned char Cdata[8]; 	  //����
	unsigned char BottleColeData[8];//ÿ��ƿ��ú����Ϣ��8��BCD������ 
}CANREC_typedef;//CAN���յ�����

//CAN���͵�����
typedef struct
{
	unsigned char dlc;	          //���ݳ���		  
	unsigned char DES_ID;	      //Դ��ַ
	unsigned char SRC_ID;		  //Ŀ�ĵ�ַ
	unsigned char PROT_CFG;		  //֡����
	unsigned char ATS_CMD; 		  //����
	unsigned char Cdata[8]; 		  //����
}struct_CANDATA;
typedef union
{
	struct_CANDATA CanData;
	unsigned char CanSenBuf[13];
}UnionCan_TypeDef;
extern UnionCan_TypeDef CANSend;//CAN���� //CAN���͵�����
extern struct_CANDATA CANSendBuffer[40];


extern CANREC_typedef CANRead;	//CAN��������
extern u8 RoadRecheck_ID;				//����·��̽����/����ת����ID
extern u8 RecheckTimer_100ms;		//���²�ѯ�豸״̬��ʱ			
extern u16 ReSendCantime_ms;

/*************************can  ��������*******************************/

u8 canPoll(void);			         //���can �����ж�
void init_can(u8 baudrate);			 //��ʼ��can ���������can������
void SendMessage(u8 SendData[]);		 // can ���ͱ���
void SendMessage_BigData(u8 *SendData);	// can ���ͳ�����
u8 canRead(u8 n, u8 *can_id, u8 *Data);      //����can����
void set_can_ID(u8 filternumber,u32 can_Stdid,u32 can_Extid,u32 can_markid ); //can    �˲�������
//void REC_Analyze(void);
void _CAN_Init(u8 baudrate);

//=============Ӧ�ò㺯��====================//
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
