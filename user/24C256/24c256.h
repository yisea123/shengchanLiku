#ifndef __24C256_H
#define __24C256_H

#include "Sys_Init.h"
#include "stm32f10x.h"
#include "stm32f10x_it.h"
//#include "system_init.h"

#define SCL_H         GPIOB->BSRR = GPIO_Pin_8
#define SCL_L         GPIOB->BRR  = GPIO_Pin_8 
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_9
#define SDA_L         GPIOB->BRR  = GPIO_Pin_9

#define SCL_read      GPIOB->IDR  & GPIO_Pin_8
#define SDA_read      GPIOB->IDR  & GPIO_Pin_9

/**************************称重模块1的地址*********************************/
#define ADD_IP			          6  									//存储IP地址的EEPROM地址
#define IP_EEP_NUM   					2   								//2字节

#define ADD_WEIGHT_A			    ADD_IP+IP_EEP_NUM  	//存储称重传感器拟合直线的斜率
#define WEIGHT_A_EEP_NUM   		8   								//8字节

#define ADD_WEIGHT_B			    ADD_WEIGHT_A+WEIGHT_A_EEP_NUM  	//存储称重传感器拟合直线的截距
#define WEIGHT_B_EEP_NUM   		8   								//8字节
/***********************************************************/

/**************************称重模块2的地址********************************/
#define ADD_IP2			          30  									//存储IP地址的EEPROM地址
#define IP_EEP_NUM2   					2   								//2字节

#define ADD_WEIGHT_A2			    ADD_IP2+IP_EEP_NUM2  	//存储称重传感器拟合直线的斜率
#define WEIGHT_A_EEP_NUM2   		8   								//8字节

#define ADD_WEIGHT_B2			    ADD_WEIGHT_A2+WEIGHT_A_EEP_NUM2  	//存储称重传感器拟合直线的截距
#define WEIGHT_B_EEP_NUM2   		8  
/***********************************************************/
#define ADD_ICEBOXDATANUM				24								//存储冰箱数据数量地址
#define ICEBOXDATANUM_EEP_NUM		2									//2字节

#define ADD_ERRNUM							26								//故障次数
#define ERRNUM_EEP_NUM					1									//1字节

#define ADD_IDCARDDATA          28                //ID卡存放地址
#define IDCARD_EEP_NUM          40                //40字节 10张卡，每张4字节


#define ADD_ICEBOXDATA					100								//冰箱数据起始地址
#define ICEBOXDATA_EEP_NUM			4000							//冰箱数据数量

////设备信息
//typedef struct
//{
//	unsigned char StationName;	//设备名称
//	unsigned char StationName2;	//设备名称	
//	unsigned char Pipe0ID; 		  //管口0连接设备号
//	unsigned char Pipe1ID; 		  //管口1连接设备号
//	unsigned char Pipe2ID; 		  //管口2连接设备号
//	unsigned char Pipe3ID; 		  //管口3连接设备号
//	unsigned char Pipe4ID; 		  //管口4连接设备号
//	unsigned char Pipe0Time;	  //管口0传输时间

//	unsigned char OnLine;	      //在线状态		  bit7表示设备在系统表格中，应该启用
//															//              bit6表示与风机线路相连（0表示没有与风机相连）
//															//						  bit0表示设备在线
//															//              bit1表示该工作站正在发送或等待柜子取瓶出来40秒
//															//              bit2表示该工作站接收就续，如果没有就续，就不能够接收瓶
//															//              bit3成功插好入队列，等待传输
//	unsigned char Senser;
//}DEVICE;

//typedef union
//{
//	DEVICE ID[0x36];
//	unsigned char pd[DEVICE_EEP_NUM];
//}DEVICE_TypeDef ;

//extern DEVICE_TypeDef      device; 								        //设备  系统表格 地址与设备的ID号一一对应

////传输记录
//typedef struct
//{
//	unsigned char Month;			//传输记录-月
//	unsigned char Day; 				//传输记录-日  
//	unsigned char Hour; 			//传输记录-时	 
//	unsigned char Min; 				//传输记录-分
//	unsigned char Sec; 				//传输记录-秒  
//	unsigned char StarName; 	//传输记录-始发站名称  
//	unsigned char EndName;		//传输记录-目的站名称
//	unsigned char Result;	  	//传输记录-传输结果
//}RECORD;

//typedef union
//{
//	RECORD RecordNum[5];
//	unsigned char pd[40];
//}RECORD_TypeDef ;

//extern RECORD_TypeDef      Record; 								        //设备  系统表格 地址与设备的ID号一一对应









void IIC_Init(void);
extern unsigned char I2C_BufferWrite(u8* pBuffer, u16 length, u16 WriteAddress);
unsigned char I2C_ReadByte(u8* pBuffer, u16 length, u16 ReadAddress);
void I2C_PageWrite(u8* pBuffer, u16 length, u16 WriteAddress);

#endif
