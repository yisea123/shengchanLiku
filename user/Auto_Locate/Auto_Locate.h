#ifndef __AUTO_LOCATE
#define __AUTO_LOCATE
#include "sys.h"
#include "Sys_Init.h"

#define LasBufLen  20
typedef struct
{
	bool Auto_Locate_Status_Change;	
	bool Over_Flag;
	bool Start_Flag;	
	u8 Auto_Locate_Stage;
	u8 X_Cnt;
	u8 Y_Cnt;
	u8 Z_Cnt;

}TYPEDEF_AUTOLOCATE;


typedef struct{
	int Distance_Arrary[16];
	int Aver_Distance;
}TYPEDEF_ARRARY;

typedef struct
{
	u8 Buf[7];
	u8 distance_data[3];
	u8 Data_Len;
}TYPEDEF_UART2;

extern u8 Laser_Rright;
extern u8 Laser_Length;
extern TYPEDEF_UART2 Laser_Rec[LasBufLen];//Ω” ‹ª∫¥Ê
void asdasd(void);
#endif
