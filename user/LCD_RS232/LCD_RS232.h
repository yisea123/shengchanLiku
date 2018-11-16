#ifndef __LCDRS232_H
#define __LCDRS232_H

#include "sys.h"
#include "Sys_Init.h"

#define PAGE_POSITION 12 //跳转行列

extern u8 dataH,dataL;

//串口2接收数据CCU-TO-TCU
//设备信息
typedef struct
{
	unsigned char Star0;
	unsigned char Star1;
	unsigned char Len;
	unsigned char Comend;
	unsigned char ComData[100];
	unsigned char check;
}struct_USART2;
extern unsigned char R_right;                                  //接收报文正确标志
extern volatile struct_USART2 Uart2Buff[5];                    //在stm32f10x_it.h里定义	  //串口2接收数据CCU-TO-TCU



void USART2_Init(u32 baud); //串口2初始化
void USART2_printf_str(u8 len, u8*put_str);	//发送Card信息
void SURE_REC_UART2_Analyze(void);
void REC_UART2_Analyze(unsigned char right_num);//串口数析【SCREEN-TO-ACU】
void Set_Variable(unsigned char adressH,unsigned char adressL, 
									unsigned char data0, unsigned char data1, 
									unsigned char data2, unsigned char data3, 
									unsigned char length);
void SCREEN_Refresh(void);
void ChangePage(unsigned char page);
void PE7OUTPUT(void);
void PE7INPUT(void);	
void All_I2C_Read(void);									

typedef union
{
	u32 X_Zero_Pluse_Data;
	u8 X_Zero_Pluse_CharData[4];
}Typedef_X_Zero_Pluse;
extern Typedef_X_Zero_Pluse Change_X_Zero_Pluse;

typedef union
{
	double X_MPP_Data;
	u8 X_MPP_CharData[8];
}Typedef_X_MPP;
extern Typedef_X_MPP Change_X_MPP;

typedef union
{
	u32 First_Goods_Height_Data;
	u8 First_Goods_Height_CharData[4];
}Typedef_First_Goods_Height;
extern Typedef_First_Goods_Height Change_First_Goods_Height;

typedef union
{
	double Z_MPP_Data;
	u8 Z_MPP_CharData[8];
}Typedef_Z_MPP;
extern Typedef_Z_MPP Change_Z_MPP;

typedef union
{
	u32 Plat_Axis_Z_Data;
	u8 Plat_Axis_Z_CharData[4];
}Typedef_Plat_Axis_Z;
extern Typedef_Plat_Axis_Z Change_Plat_Axis_Z;

typedef union
{
	u32 Plat_Axis_X_Data;
	u8 Plat_Axis_X_CharData[4];
}Typedef_Plat_Axis_X;
extern Typedef_Plat_Axis_X Change_Plat_Axis_X;

typedef union
{
	u32 Y_RightZero_Pluse_Data;
	u8 Y_RightZero_Pluse_CharData[4];
}Typedef_Y_RightZero_Pluse;
extern Typedef_Y_RightZero_Pluse Change_Y_RightZero_Pluse;

typedef union
{
	u32 Y_LeftZero_Pluse_Data;
	u8 Y_LeftZero_Pluse_CharData[4];
}Typedef_Y_LeftZero_Pluse;
extern Typedef_Y_LeftZero_Pluse Change_Y_LeftZero_Pluse;

typedef union
{
	u32 Y_LeftTrayPluse_Data;
	u8 Y_LeftTrayPluse_CharData[4];
}Typedef_Y_LeftTrayPluse;
extern Typedef_Y_LeftTrayPluse Change_Y_LeftTrayPluse;

typedef union
{
	u32 Y_LeftLim_MorePluse_Data;
	u8 Y_LeftLim_MorePluse_CharData[4];
}Typedef_Y_LeftLim_MorePluse;
extern Typedef_Y_LeftLim_MorePluse Change_Y_LeftLim_MorePluse;

typedef union
{
	u32 Y_LeftLim_LessPluse_Data;
	u8 Y_LeftLim_LessPluse_CharData[4];
}Typedef_Y_LeftLim_LessPluse;
extern Typedef_Y_LeftLim_LessPluse Change_Y_LeftLim_LessPluse;

typedef union
{
	u32 Y_RightLim_MorePluse_Data;
	u8 Y_RightLim_MorePluse_CharData[4];
}Typedef_Y_RightLim_MorePluse;
extern Typedef_Y_RightLim_MorePluse Change_Y_RightLim_MorePluse;

typedef union
{
	u32 Y_RightLim_LessPluse_Data;
	u8 Y_RightLim_LessPluse_CharData[4];
}Typedef_Y_RightLim_LessPluse;
extern Typedef_Y_RightLim_LessPluse Change_Y_RightLim_LessPluse;

typedef union
{
	u32 AxisZ_UpDown_Pluse_Data;
	u8 AxisZ_UpDown_Pluse_CharData[4];
}Typedef_AxisZ_UpDown_Pluse;
extern Typedef_AxisZ_UpDown_Pluse Change_AxisZ_UpDown_Pluse;

typedef union
{
	u32 AxisZ_UpDownMore_Pluse_Data;
	u8 AxisZ_UpDownMore_Pluse_CharData[4];
}Typedef_AxisZ_UpDownMore_Pluse;
extern Typedef_AxisZ_UpDownMore_Pluse Change_AxisZ_UpDownMore_Pluse;

unsigned char HexToBcd(unsigned int Hex);

#endif
