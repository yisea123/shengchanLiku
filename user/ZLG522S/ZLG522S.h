
#ifndef __ZLG522S_H
#define __ZLG522S_H

#include "sys.h"
#include <stdbool.h>

#define EN_USART1_RX 			1		//Ê¹ÄÜ£¨1£©/½ûÖ¹£¨0£©´®¿Ú1½ÓÊÕ
//=================================================================================================
////============= º¯Êı¼ô²Ã ==========================================================================
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

////============= ³£Á¿¶¨Òå ==========================================================================
////¶¨ÒåAĞÍ¿¨ÃüÁî
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

// ¶¨Òå´®ĞĞÊı¾İ»º³åÇø¸÷×Ö·ûµ¥ÔªË÷ÒıÖµ
#define FRAMELEN				0				// Ö¡³¤¶È
#define CMDTYPE					1				// °üºÅ
#define COMMAND					2				// ÃüÁî
#define STATUS					2				// ÏìÓ¦×´Ì¬
#define LENGTH      		3				// Êı¾İ³¤¶È
#define DATA        		4				// Êı¾İÆğÊ¼Î»ÖÃ
#define PRTCLEN					6				// Ğ­Òé×Ö·û³¤¶È

// Í¨ĞÅĞ­Òé³£Á¿
#define STX     				0x20
#define ETX     				0x03
#define ACK						  0x06
#define NAK							0x15

// ¶¨Òå·µ»Ø´íÎó´úÂë
#define OK						  0
#define COMMERR					1
#define SEQERR					2
#define BCCERR					4

// ¶¨Òå»º³åÇø´óĞ¡
#define MAXSERSIZE      64+2+6
extern u8 SerBfr[MAXSERSIZE];// ´®ĞĞÊı¾İ»º³åÇ
extern u8 Coal_SampleID[8];
extern u8 Coal_ID[16];
extern u8 Status1;
extern u8 Status2;
extern bool RFID_Hand_OK; 	//¶Á¿¨Æ÷³õÊ¼»¯Óë´®¿ÚÎÕÊÖ³É¹¦±ê¼Ç

extern u8 BottleRfidId[4];//Æ¿RFID
extern u8 ColRfidId[8];//ÃºÑùĞÅÏ¢


//============= º¯ÊıÉùÃ÷ =================================================================
/**************************************************************************************************
** º¯ÊıÔ­ĞÍ:    unsigned char ZLG522S_UARTInit(void)
** º¯Êı¹¦ÄÜ:    ZLG522S³õÊ¼»¯
** ÊäÈë²ÎÊı:    -		
** Êä³ö²ÎÊı:    
** ·µ »Ø Öµ:    ÃüÁîÖ´ĞĞ½á¹û
** Ëµ    Ã÷:		¸Ãº¯Êı
**************************************************************************************************/
void USART3_Init(u32 bound);		//´®¿Ú3³õÊ¼»¯
u8 ZLG522S_UARTInit(void);			//ZLG522S³õÊ¼»¯
void RFIDdelay_us(u16 time);
/**************************************************************************************************
** º¯ÊıÔ­ĞÍ:    unsigned char ReadColRfid(BottleRfidId,ColRfidId)
** º¯Êı¹¦ÄÜ:    ¶ÁRFIDºÍÃºÑùĞÅÏ¢
** ÊäÈë²ÎÊı:    RFID ºÍ ÃºÑùĞÅÏ¢  »º³åÇø		
** Êä³ö²ÎÊı:    
** ·µ »Ø Öµ:    ÃüÁîÖ´ĞĞ½á¹û 0³É¹¦  1Ê§°Ü
** Ëµ    Ã÷:	¸Ãº¯Êı
							1¡¢A¿¨¼¤»î MÖ¸Áî  ·µ»ØRFID
							2¡¢Ö±½ÓÃÜÂëÑéÖ¤ FÖ¸Áî
							3¡¢¶Á»º³åÇø8 GÖ¸Áî ·µ»ØÃºÑùĞÅÏ¢
**************************************************************************************************/
unsigned char ReadColRfid(unsigned char BottleRfidId_temp[],unsigned char ColRfidId_temp[]);

//¶Á¿¨º¯Êı
/**************************************************************************************************
** º¯ÊıÔ­ĞÍ:    unsigned char PiccActiveTypeA(	unsigned char BaudRate,unsigned char Req_Code,
**												unsigned char *Atq,unsigned char *Ats,
**												unsigned char *PiccSnrLen,unsigned char *PiccSnr)
** º¯Êı¹¦ÄÜ:    Ä£¿éĞÅÏ¢ÅäÖÃ
** ÊäÈë²ÎÊı:    unsigned char BaudRate			// ±£Áô²ÎÊı£¬ÉèÖÃÎª0
**							unsigned char Req_Code			// ÇëÇóÂë	
** Êä³ö²ÎÊı:    unsigned char *Atq					// ÇëÇó»ØÓ¦
**							unsigned char *Ats					// ×îºóÒ»¼¶Ñ¡ÔñÓ¦´ğ
**							unsigned char *Piunsigned char PiccAutoDetect(unsigned char OutMode,  unsigned char TxMode, unsigned char Req_Code, 
					 unsigned char AuthMode, unsigned char KEYAB,  unsigned char *Key, unsigned char Block)ccSnrLen		// ¿¨ºÅ³¤¶È
**							unsigned char *PiccSnr			// ¿¨ºÅ
** ·µ »Ø Öµ:    ÃüÁîÖ´ĞĞ½á¹û
**************************************************************************************************/
extern u8 PiccActiveTypeA(u8 BaudRate, u8 Req_Code, u8 *Atq, u8 *Ats, u8 *PiccSnrLen, u8 *PiccSnr);
u8 PiccAutoDetect(u8 OutMode, u8 TxMode, u8 Req_Code, u8 AuthMode, u8 KEYAB,  u8 *Key, u8 Block);
char IC_PiccAutKey(u8 *Key);		//Ö±½ÓÃÜÂëÑéÖ¤
char ActiveCardKey(void);				//AĞÍ¿¨¼¤»îºÍÃÜÂëÑéÖ¤
unsigned char IC_ReadBlockData(u8 Block);	  //¶Á¿¨
unsigned char IC_WriteBlockData(u8 Block);  	//Ğ´¿¨
int IC_SearchBrush(void);
u32 BottleID_ByteTOString(u8 B1, u8 B2, u8 B3, u8 B4); //Æ¿/¿¨IDÓÉ×Ö½Ú×ªÎª4¸öÁ¬Ğø×Ö·û´®

#endif				// __ZLG522S_H__

