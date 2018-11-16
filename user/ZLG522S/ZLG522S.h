
#ifndef __ZLG522S_H
#define __ZLG522S_H

#include "sys.h"
#include <stdbool.h>

#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
//=================================================================================================
////============= �������� ==========================================================================
//#define GET_DVC_INFO_EN				1
//#define PCD_CONFIG_EN					0
//#define PCD_CLOSE_EN					0
//#define PCDLoadKey_en					0
//#define PCDWriteReg_en				0
//#define PCDReadReg_en					0

//#define PICC_REQUEST_EN 			0
//#define PiccAnticoll_en				0
//#define PiccSelect_en					0
//#define PiccHalt_en						0
//#define PiccAuthE2_en					0
//#define PiccAuthKey_en				0
//#define PiccRead_en						0
//#define PiccWrite_en					0
//#define PiccULWrite_en				0
//#define PiccValue_en					0
//#define PiccReset_en					0			
#define PICC_ACTIVE_TYPEA_EN	1
#define PiccAutoDetect_en			1
//#define PiccADRcv_en					0
//#define PiccReadADDT_en				0

////============= �������� ==========================================================================
////����A�Ϳ�����
#define REQSTD		0x26	
#define REQALL		0x52
#define SELCODE1	0x93
#define SELCODE2	0x95
#define SELCODE3	0x97
#define KEYA			0x60
#define KEYB			0x61
#define DECREMENT	0xc0
#define INCREMENT	0xc1
#define RESTORE		0xc2
#define ISO14443A	0x00
#define ISO14443B	0x80
#define INT8U unsigned char

// ���崮�����ݻ��������ַ���Ԫ����ֵ
#define FRAMELEN				0				// ֡����
#define CMDTYPE					1				// ����
#define COMMAND					2				// ����
#define STATUS					2				// ��Ӧ״̬
#define LENGTH      		3				// ���ݳ���
#define DATA        		4				// ������ʼλ��
#define PRTCLEN					6				// Э���ַ�����

// ͨ��Э�鳣��
#define STX     				0x20
#define ETX     				0x03
#define ACK						  0x06
#define NAK							0x15

// ���巵�ش������
#define OK						  0
#define COMMERR					1
#define SEQERR					2
#define BCCERR					4

// ���建������С
#define MAXSERSIZE      64+2+6
extern u8 SerBfr[MAXSERSIZE];// �������ݻ����
extern u8 Coal_SampleID[8];
extern u8 Coal_ID[16];
extern u8 Status1;
extern u8 Status2;
extern bool RFID_Hand_OK; 	//��������ʼ���봮�����ֳɹ����

extern u8 BottleRfidId[4];//ƿRFID
extern u8 ColRfidId[8];//ú����Ϣ


//============= �������� =================================================================
/**************************************************************************************************
** ����ԭ��:    unsigned char ZLG522S_UARTInit(void)
** ��������:    ZLG522S��ʼ��
** �������:    -		
** �������:    
** �� �� ֵ:    ����ִ�н��
** ˵    ��:		�ú���
**************************************************************************************************/
void USART3_Init(u32 bound);		//����3��ʼ��
u8 ZLG522S_UARTInit(void);			//ZLG522S��ʼ��
void RFIDdelay_us(u16 time);
/**************************************************************************************************
** ����ԭ��:    unsigned char ReadColRfid(BottleRfidId,ColRfidId)
** ��������:    ��RFID��ú����Ϣ
** �������:    RFID �� ú����Ϣ  ������		
** �������:    
** �� �� ֵ:    ����ִ�н�� 0�ɹ�  1ʧ��
** ˵    ��:	�ú���
							1��A������ Mָ��  ����RFID
							2��ֱ��������֤ Fָ��
							3����������8 Gָ�� ����ú����Ϣ
**************************************************************************************************/
unsigned char ReadColRfid(unsigned char BottleRfidId_temp[],unsigned char ColRfidId_temp[]);

//��������
/**************************************************************************************************
** ����ԭ��:    unsigned char PiccActiveTypeA(	unsigned char BaudRate,unsigned char Req_Code,
**												unsigned char *Atq,unsigned char *Ats,
**												unsigned char *PiccSnrLen,unsigned char *PiccSnr)
** ��������:    ģ����Ϣ����
** �������:    unsigned char BaudRate			// ��������������Ϊ0
**							unsigned char Req_Code			// ������	
** �������:    unsigned char *Atq					// �����Ӧ
**							unsigned char *Ats					// ���һ��ѡ��Ӧ��
**							unsigned char *Piunsigned char PiccAutoDetect(unsigned char OutMode,  unsigned char TxMode, unsigned char Req_Code, 
					 unsigned char AuthMode, unsigned char KEYAB,  unsigned char *Key, unsigned char Block)ccSnrLen		// ���ų���
**							unsigned char *PiccSnr			// ����
** �� �� ֵ:    ����ִ�н��
**************************************************************************************************/
extern u8 PiccActiveTypeA(u8 BaudRate, u8 Req_Code, u8 *Atq, u8 *Ats, u8 *PiccSnrLen, u8 *PiccSnr);
u8 PiccAutoDetect(u8 OutMode, u8 TxMode, u8 Req_Code, u8 AuthMode, u8 KEYAB,  u8 *Key, u8 Block);
char IC_PiccAutKey(u8 *Key);		//ֱ��������֤
char ActiveCardKey(void);				//A�Ϳ������������֤
unsigned char IC_ReadBlockData(u8 Block);	  //����
unsigned char IC_WriteBlockData(u8 Block);  	//д��
int IC_SearchBrush(void);
u32 BottleID_ByteTOString(u8 B1, u8 B2, u8 B3, u8 B4); //ƿ/��ID���ֽ�תΪ4�������ַ���

#endif				// __ZLG522S_H__

