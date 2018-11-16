#ifndef __NETWORK_H
#define __NETWORK_H
#include "Sys_Init.h"	


/***********网络协议指令定义表**************/
#define CMD_REPLY 				0x00   //正负应答帧
#define CMD_READVER 			0x01   //读取版本号
#define CMD_QUERYSTATE 		0x02   //查询设备状态
#define CMD_CTRL_XZ 			0x03   //控制设备动作
#define CMD_INDVICTRL_XYZ 0x04	 //独立控制三轴
#define CMD_SPEED_STEP		0x05	 //设置步长、速度
#define CMD_STOP					0x06	 //停止以及报警复位
#define CMD_TRAYSET0			0x07	 //托盘Y坐标清零
#define Net_Id						0x6d
#define Prot_Cfg 					0x61

#define SPEED							0x00
#define Successed         0x01
#define Failed            0x00

/************短报文数据格式***************/
#define Uart_Head_1		0//帧头
#define Uart_Head_2		1//帧头
#define	Rec_Length 		2//数据长度
#define COMEND				3//指令集
#define NETNUM				4//网关地址
#define PORT_CFG			5//帧信息
#define COMDATA				6//转工位指令
#define CHECKADD			7//和校验


#define Local_Ip			0x6D

/***********03指令返回状态字节********************/
#define First_Stage	 	0x01
#define Second_Stage 	0x02
#define Third_Stage 	0x03
#define Fourth_Stage 	0x04
#define Fifth_Stage 	0x05
#define Error					0xff
#define WID_LENALM		0XF0
#define Torque_Alm		0XF1
#define Analysis_Error	0xff

/***************04指令三轴独立控制字节***************/
#define X							0X00
#define Z							0X10
#define Y							0X20

#define X_BACK				0X00
#define X_FRONT				0X01
#define Y_LEFT				0X00
#define Y_RIGHT				0X01
#define Z_BACK				0X01
#define Z_FRONT				0X00

#define Speed_Stage		0x00
#define Step_Stage		0x10

/*******************工程师模式频率档位调节*************************/
#define Casual_Fre_1						0x01
#define Casual_Fre_2						0x02
#define Casual_Fre_3						0x03
#define Casual_Fre_4						0x04
#define Casual_Fre_5						0x05
#define Casual_Fre_6						0x06
#define Casual_Fre_7						0x07

/*******************工程师模式步长档位调节*************************/
#define Casual_Ste_1						0x10
#define Casual_Ste_2						0x20
#define Casual_Ste_3						0x30
#define Casual_Ste_4						0x40
#define Casual_Ste_5						0x50
#define Casual_Ste_6						0x60
#define Casual_Ste_7						0x70
#define Casual_Ste_None					0xf0//随动步长
/*******************03转工位指令**************************/
#define Get_Left_Box			0x00
#define Get_Right_Box			0x01
#define Return_Left_Box		0x10
#define Return_Right_Box	0x11
#define Arrive						0x20
#define Reset							0x30

/**************托盘属性****************/
#define Keep							0x00
#define Left_Tray					0x01
#define Right_Tray				0x02

#define Right							0x02
#define Zero							0x01
#define Left							0x00



typedef struct
{	
	 volatile u8	RecAdd;
	 u8 Buffer[30];
	 u8 RxByte;//单个字节接收
	 u8 checkadd;

	u8 RecError;//接收错误
}struct_UART3;
extern  u8 Cache_Cnt;
extern  volatile struct_UART3 Uart3[40];
extern u8	Current_Position_Buff[8];
extern u8 test_add;
extern bool Set_flag;
extern bool Reset_Flag;
extern bool Enable_Uart3_Flg;
//extern bool XReturn_OverFlag;
//extern bool ZReturn_OverFlag;

void Uart2AskBytes(unsigned char *str,unsigned int Num); 
void send_char(char *buf,u8 set_flg);
void USART3_Init(u32 bound);
void Uart_Analysis(void);
void Uart_Send(u8 Length,u8 cmd,u8 net_num,u8 Port_cfg,u8 comdata,u8 Buf[]);
void WIFI_Init(void);
void Return_SendMesge(void);

#endif
