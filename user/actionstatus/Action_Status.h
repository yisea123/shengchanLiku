#ifndef __ACTIONSTATUS_H
#define __ACTIONSTATUS_H
#include "sys.h"
#include "Sys_Init.h"


//#define Plat_Axis_Z									212//托架Z轴坐标单位Mm
//#define Plat_Axis_X									10//托架X轴坐标单位Mm
extern u32 Plat_Axis_Z;
extern u32 Plat_Axis_X;
#define AxisZ_Float_Disance				20//货架浮动距离，单位mm
#define AxisZ_PutBoxFloat_Disance 50		//取箱回来时，托盘位于托架上方的距离单位Mm
//#define AxisZ_UpDown_Pluse				110//托盘上升下降脉冲数
//#define AxisZ_UpDownMore_Pluse		190//托盘上升下降脉冲数
extern u32 AxisZ_UpDown_Pluse;
extern u32 AxisZ_UpDownMore_Pluse;
#define AxisZ_UpDownMore_Dis 55
#define Addtion_Pluse_On_Tray     80//货物属性与托架属性不一致时，额外行走脉冲数
#define Tray		1//托盘当前处于托架处
#define Goods		0//托盘当前处于货物处	

#define Tray_AutoReset_Pulse			40
#define ActionDelay		200//每个动作结束的延时时间

//#define First_Goods_Height 30//第一层货架相对于托盘的高度，单位Mm
extern u32 First_Goods_Height;
#define Z_Difference     (270-First_Goods_Height)
#define AB_HIGHTLENGTH_SENSOR			PEin(12)

#define A_HIN PEin(1)
#define A_LIN PEin(13)
#define B_HIN PEin(9)
#define B_LIN PEin(7)

#define		A_SENSOR PEin(11)	
#define 	B_SENSOR PEin(6)
/***********三色灯***************/
#define 	LED_RED  PCout(1)
#define 	LED_GRE  PCout(0)
#define 	LED_YEL  PCout(3)
#define 	ON	1
#define		OFF 0

/********************************/
#define Aid_Z_Sensor PEin(1)
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

typedef struct
{
	bool Start_Flag;
	bool Status_Change;
	bool Trigger;
	u8 Stage;
	u32 Coordinate;
}TYPEDEF_AID_Z;

typedef union
{
	u32 Coordinate;
	u8 CoordinateCharData[4];
}Typedef_AIDZ;
extern Typedef_AIDZ AidZ_I2c;
extern TYPEDEF_AID_Z Aid_Z;
extern u32 temp_Z_Coordinate;
extern TYPEDEF_BACKZERO XYZ_To_Zero;
extern u32 ZLim_Coordinate;
extern bool Z_ToZeroFlag;
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
void Axis_Z_Aid_Coordinate(void);
void AidZLocatEmg(void);
#endif
