
#ifndef __SENSOR_H
#define __SENSOR_H

#include "sys.h"
#include <stdbool.h>

//=============================变量定义区===============================//
//RFID 数组
extern u8 Status;			//读到瓶标签标记(瓶标签完好，且ID有效)		
extern u8 ATQ[2];
extern u8 SAK;
extern u8 Len;
extern u8 Bottle_id[4];
extern u8 RFID_FF_ct;	//若RFID连续三次读到ff ff ff ff，RFID供电IO复位一次，ZLG522S重新初始化一次
extern bool Bottle_NoRFID; //瓶子无RFID标签标记
extern bool NO_RFID_Bottle;
extern bool Bottle_Exist;

//工作站属性 结构体
typedef struct
{
	u8 DEVICE_TYPE;		//工作站类型
	u8 DEVICE_ID;			//工作站ID
	u8 DEVICE_STATUS;	//工作站状态	
}_DEVICE_PROPERTY;

extern _DEVICE_PROPERTY Device_Prop;
extern _DEVICE_PROPERTY Device[12];

//工作站类型
typedef enum
{
	_ADS16 = 0x01,//自动存样
	
	_AMS16,				//半自动存样
	_AMS16B,		  //单发单收（与开元对接工作站）
	_AMS16D,      //新版人工单发单收
	
	_AES16,				//自动取样
	_AES16B,			//空瓶回收
	_AES16D,      //自动取样 无滑动阀版本
	_AES16F       //自动取样 无滑动阀版本，无滑轨版本
}_STATION_TYPE;

//******全自动取样站选择*****************
#define STATION_AES16A 0  //有滑动阀，有滑轨版本
#define STATION_AES16D 1  //无滑动阀，有滑轨版本，带拨杆
#define STATION_AES16F 0  //无滑动阀，无滑轨版本，带推杆

//******半自动存取样站选择***************
#define STATION_AMS16A 0  
#define STATION_AMS16D 1

//==============================传感器定义================================//
//===============================RFID供电=================================//
#define RFID_SE 	PCout(10)
#define RFID_SEH	RFID_SE = 1
#define RFID_SEL	RFID_SE = 0

//============================ADS16==============================//
#define Sensor_Bottlefull    PCin(11)   //瓶满传感器    -P9
#define Sensor_HeightTest    PDin(0)    //瓶高检测传感器-P10
#define Sensor_BottlecapTest PDin(2)	  //瓶盖检测传感器-P11
#define Sensor_MotorZero     PEin(3)    //电机零位传感器-P14
#define Sensor_SendPosition  PEin(4)    //发送位传感器  -P15

//============================AES16D==============================//
#define Sensor_Rod_Push			 PAin(5)    //推杆推工位    -P7#1
#define Sensor_Rod_Back			 PAin(6)    //推杆收工位    -P7#2
#define Sensor_Bottlefull    PCin(11)   //瓶满传感器    -P9
#define Sensor_Pipe			     PDin(0)    //管道口传感器  -P10
#define Sensor_MotorDown     PDin(2)    //电机下工位传感器-P11 - 下工位
#define Sensor_MotorZero     PEin(3)    //电机零位传感器-P14 - 上工位
#define Sensor_Lock				   PEin(4)    //电控锁传感器  -P15   0：锁 1：开

//============================AES16A==============================//
#define Sensor_Valve_Close	 PAin(5)    //滑动阀关工位  -P7#1
#define Sensor_Valve_Open		 PAin(6)    //滑动阀开工位  -P7#2
#define Sensor_Bottlefull    PCin(11)   //瓶满传感器    -P9
#define Sensor_Pipe			     PDin(0)    //管道口传感器  -P10
#define Sensor_MotorZero     PEin(3)    //电机零位传感器-P14 - 零位
#define Sensor_Lock				   PEin(4)    //电控锁传感器  -P15   0：锁 1：开

//============================AMS16D==============================//
#define Sensor_ShortBottle   PDin(0)    //矮瓶传感器    -P10
#define Sensor_Key           PDin(2)    //大按键输入    -P11 
#define Sensor_HighBottle    PEin(3)    //高瓶传感器    -P14 
#define Sensor_Lock				   PEin(4)    //电控锁传感器  -P15   0：锁 1：开

//============================AES16F==============================//
#define Sensor_Rod_Push			 PAin(5)    //推杆推工位    -P7#1
#define Sensor_Rod_Back			 PAin(6)    //推杆收工位    -P7#2
#define Sensor_Bottlefull    PCin(11)   //瓶满传感器    -P9
#define Sensor_Pipe			     PDin(0)    //管道口传感器  -P10
#define Sensor_MotorZero     PEin(3)    //电机零位传感器-P14 - 零位
#define Sensor_Lock				   PEin(4)    //电控锁传感器  -P15   0：锁 1：开

//============================AMS16B==============================//
#define Sensor_Pipe			     PDin(0)    //有瓶信号      -P10
#define Sensor_KY 				   PEin(4)    //开元忙信号    -P15   0：锁 1：开


//=============================函数定义区=================================//

void Sensor_Init(void);				//传感器端口初始化
void Read_Device_ID(void);		//读取设备ID
u8 Station_Type_Judge(void);	//工作站类型判断
u8 Station_Sensor_Read(void);	//工作站状态读取
void Send_Sensor_Status(void);//传感器状态变化上传

void BottleID_CoalInfo_Read(void);		//瓶ID、煤样信息获取
void FAS_HAS_Bottle_ID_Send(void);		//自动、半自动存样工作站瓶ID发送   

#endif /*__SENSOR_H */

