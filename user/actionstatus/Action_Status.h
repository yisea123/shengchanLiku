#ifndef __ACTIONSTATUS_H
#define __ACTIONSTATUS_H
#include "sys.h"
#include "Sys_Init.h"


//#define Plat_Axis_Z									212//�м�Z�����굥λMm
//#define Plat_Axis_X									10//�м�X�����굥λMm
extern u32 Plat_Axis_Z;
extern u32 Plat_Axis_X;
#define AxisZ_Float_Disance				15//���ܸ������룬��λmm
#define AxisZ_PutBoxFloat_Disance 50		//ȡ�����ʱ������λ���м��Ϸ��ľ��뵥λMm
//#define AxisZ_UpDown_Pluse				110//���������½�������
//#define AxisZ_UpDownMore_Pluse		190//���������½�������
extern u32 AxisZ_UpDown_Pluse;
extern u32 AxisZ_UpDownMore_Pluse;
#define Tray		1//���̵�ǰ�����мܴ�
#define Goods		0//���̵�ǰ���ڻ��ﴦ	

#define Tray_AutoReset_Pulse			40
#define ActionDelay		500//ÿ��������������ʱʱ��

//#define First_Goods_Height 30//��һ�������������̵ĸ߶ȣ���λMm
extern u32 First_Goods_Height;
#define Z_Difference     (270-First_Goods_Height)
#define AB_HIGHTLENGTH_SENSOR			PEin(12)

#define A_HIN PEin(1)
#define A_LIN PEin(13)
#define B_HIN PEin(9)
#define B_LIN PEin(7)

#define		A_SENSOR PEin(11)	
#define 	B_SENSOR PEin(6)
/***********��ɫ��***************/
#define 	LED_RED  PCout(0)
#define 	LED_GRE  PCout(1)
#define 	LED_YEL  PCout(3)
#define 	ON	1
#define		OFF 0
typedef struct 
{
	bool XYZRET_Status_Change;
	u8 XYZ_Ret_Stage;
	bool Over_Flag;
	bool Start_Flag;
	bool X_Return_Flag;
	bool Tray_Reset_Flag;
	bool Z_Return_Flag;
}TYPEDEF_BACKZERO;
extern TYPEDEF_BACKZERO XYZ_To_Zero;
void Get_Box(u16 AXIS_X_Mm,u16 AXIS_Z_Mm);
void Put_Box(u16 AXIS_X_Mm,u16 AXIS_Z_Mm);
void Axis_Move_Alone(u8 Axis ,bool Dir);
void Tray_Put_Box(bool Axis_Y_Dir,u32 UpDown_Pluse,bool Tray_Position);
void XYZ_BackZero(void);
void Action_Stop(void);

typedef void(*pick_one)(u8 tray_position,u8 Command_Data,u32 x_run,u32 Z_Run);
void Get_PutBox_Analysis(pick_one open,u8 tray_position,u8 command_data,u32 x_run, u32 Z_Run);
void Put_Box_Analysis(u8 tray_position,u8 command_data,u32 x_run, u32 Z_Run);
void Get_Box_Analysis(u8 tray_position,u8 command_data,u32 x_run, u32 Z_Run);
void Arrive_Analysis(u8 tray_position,u8 command_data,u32 x_run, u32 Z_Run);
bool Length_High_LimCtl(void);
void Auto_Rest(void);
void XZ_Run(u16 AXIS_X_Mm,u16 AXIS_Z_Mm);
char Axis_X_Demarcate(u8 X_X,u8 Z_Z,u8 Tray_Position);
#endif
