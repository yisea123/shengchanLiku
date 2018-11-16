
#include "Sensor.h"
#include "led.h"

//RFID 数组
u8 Status;		//读到瓶标签标记(瓶标签完好，且ID有效)		
u8 ATQ[2];
u8 SAK;
u8 Len;
u8 Bottle_id[4];
u8 RFID_FF_ct = 0;	//若RFID连续三次读到ff ff ff ff，RFID供电IO复位一次，ZLG522S重新初始化一次
bool Bottle_NoRFID; //瓶子无RFID标签标记
bool NO_RFID_Bottle;
bool Bottle_Exist;
//#define MAN_CONTROL 1

_DEVICE_PROPERTY Device_Prop;	//结构体变量 工作站属性
_DEVICE_PROPERTY Device[12];
void Sensor_Init(void)		//传感器端口初始化
{
	RCC->APB2ENR|=1<<2;			//使能PORTA时钟
	RCC->APB2ENR|=1<<4;			//使能PORTC时钟
	RCC->APB2ENR|=1<<5;			//使能PORTD时钟
	RCC->APB2ENR|=1<<6;			//使能PORTE时钟
	
	//设备地址 
	GPIOD->CRH&=0X00000000;	//PD8~15  上拉输入
	GPIOD->CRH|=0X88888888;
	
	//滑动阀3工位
	GPIOA->CRL&=0X000FFFFF;	//PA5~7   上拉输入(硬件外设)
	GPIOA->CRL|=0X88800000;
	
	//瓶检测
	GPIOC->CRH&=0XFFFF0FFF;	//PC11 	  上拉输入
	GPIOC->CRH|=0X00008000;	
	GPIOD->CRL&=0XFFFFF0F0;	//PD0,PD2 上拉输入
	GPIOD->CRL|=0X00000808;
	
//	//微动开关	金属接近开关		
//	GPIOE->CRL&=0XFFF00FFF;	//PE3,PE4 上拉输入
//	GPIOE->CRL|=0X00088000;
	
	//RFID供电
	GPIOC->CRH&=0XFFFFF0FF;	//PC10 	推挽输出 速度50MHz
	GPIOC->CRH|=0X00000300;
	GPIOC->ODR|=1<<10;			//PC10 	输出高	RFID_SE通电
	delay_ms(500);						
}

/******************************************************************************
函数名称：Read_Device_ID()
功能描述：设备地址读取
*******************************************************************************/
void Read_Device_ID(void)
{
	Device_Prop.DEVICE_ID = ~(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_15) | GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_14)<<1 | GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_13)<<2 
													|GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_12)<<3| GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_11)<<4 | GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_10)<<5
													|GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_9)<<6	| GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_8)<<7);
}
/***************************************************************
================================================================
@ 工作站类型						工作站地址
----------------------------------------------------------------
@	网关       				  0x01		      
@	动力轨道    				0x10-0x1f		      
@	工作站      				0x20-0x2f		      
================================================================			
****************************************************************/
u8 Station_Type_Judge(void)		//工作站类型定义
{	
//	if(Device_Prop.DEVICE_ID)		//工作站ID不为0
//	{
//			   if(Device_Prop.DEVICE_ID >= 0xA0 && Device_Prop.DEVICE_ID <= 0xAA)	return _ADS16;	//自动存样 110 160   ADS16
//		else if(Device_Prop.DEVICE_ID >= 0xAB && Device_Prop.DEVICE_ID <= 0xAF)
//		{ 
//			#if STATION_AMS16A
//			return _AMS16;	//半自动存、取样 160 AMS16
//			#endif
//			
//			#if STATION_AMS16D
//			return _AMS16D; //半自动存取样，无滑台版本
//			#endif
//		}			
//		else if(Device_Prop.DEVICE_ID >= 0xB0 && Device_Prop.DEVICE_ID <= 0xBA)
//		{
//			#if STATION_AES16A
//			return _AES16;	//自动取样 110 160   AES16		
//			#endif
//			
//			#if STATION_AES16D
//			return _AES16D;	//自动取样	
//			#endif
//			
//			#if STATION_AES16F
//			return _AES16F;	//自动取样 
//			#endif
//		}			
//		else if(Device_Prop.DEVICE_ID >= 0xBB && Device_Prop.DEVICE_ID <= 0xBF)	return _AMS16B;	
//		else if(Device_Prop.DEVICE_ID >= 0xC0 && Device_Prop.DEVICE_ID <= 0xCF)	return _AES16B;
//	}
//	else	//工作站ID为0	
//	{		
//		return 0;
//	}
	return 0;
}

/******************************************************************************
================================读传感器状态===================================
===============================================================================
						|	  7	  |   6   |	  5    |	  4	  |   3   |   2   |   1   |   0   |
-------------------------------------------------------------------------------
						| 滑台零|	滑台发|滑动阀开|滑动阀关|	门锁. |	瓶满1/|	瓶存在|	托盘	|
						|	工位	|	送工位|	 工位2 |	工位1	|				|			  |	滑动阀| 瓶存在|
===============================================================================
自动存样	：|		*		|		*		|				 |				|				|		*		|		*		|		*		|
===============================================================================

*******************************************************************************/
u8 Station_Sensor_Read(void)		//工作站传感器状态读取(带工作站类型)
{
	/*****模拟****/
	
	return 0;
}

