#ifndef __OUTPUT_H
#define	__OUTPUT_H

#include "sys.h"

#define	P1_Out		PCout(0)//P13-1
#define	P2_Out		PCout(1)//P13-2
#define	P3_Out		PCout(3)//P13-3
#define	P4_Out		PAout(5)//P13-4

#define P26_DIR2	PEout(0)//P22-4
#define P26_PUL2	PEout(2)//P22-3
#define P27_DIR1	PEout(4)//P22-2
#define P27_PUL1	PBout(7)//P22-1

#define RFID_VCC 		PAout(15)		
#define RFID_ON()		RFID_VCC = 1
#define RFID_OFF()	RFID_VCC = 0

void GPIO_OutPut_Init(void);		//Êä³ö³õÊ¼»¯

#endif

