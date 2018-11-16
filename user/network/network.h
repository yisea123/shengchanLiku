#ifndef __NETWORK_H
#define __NETWORK_H
#include "Sys_Init.h"	


/***********����Э��ָ����**************/
#define CMD_REPLY 				0x00   //����Ӧ��֡
#define CMD_READVER 			0x01   //��ȡ�汾��
#define CMD_QUERYSTATE 		0x02   //��ѯ�豸״̬
#define CMD_CTRL_XZ 			0x03   //�����豸����
#define CMD_INDVICTRL_XYZ 0x04	 //������������
#define CMD_SPEED_STEP		0x05	 //���ò������ٶ�
#define CMD_STOP					0x06	 //ֹͣ�Լ�������λ
#define CMD_TRAYSET0			0x07	 //����Y��������
#define Net_Id						0x6d
#define Prot_Cfg 					0x61

#define SPEED							0x00
#define Successed         0x01
#define Failed            0x00

/************�̱������ݸ�ʽ***************/
#define Uart_Head_1		0//֡ͷ
#define Uart_Head_2		1//֡ͷ
#define	Rec_Length 		2//���ݳ���
#define COMEND				3//ָ�
#define NETNUM				4//���ص�ַ
#define PORT_CFG			5//֡��Ϣ
#define COMDATA				6//ת��λָ��
#define CHECKADD			7//��У��


#define Local_Ip			0x6D

/***********03ָ���״̬�ֽ�********************/
#define First_Stage	 	0x01
#define Second_Stage 	0x02
#define Third_Stage 	0x03
#define Fourth_Stage 	0x04
#define Fifth_Stage 	0x05
#define Error					0xff
#define WID_LENALM		0XF0
#define Torque_Alm		0XF1
#define Analysis_Error	0xff

/***************04ָ��������������ֽ�***************/
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

/*******************����ʦģʽƵ�ʵ�λ����*************************/
#define Casual_Fre_1						0x01
#define Casual_Fre_2						0x02
#define Casual_Fre_3						0x03
#define Casual_Fre_4						0x04
#define Casual_Fre_5						0x05
#define Casual_Fre_6						0x06
#define Casual_Fre_7						0x07

/*******************����ʦģʽ������λ����*************************/
#define Casual_Ste_1						0x10
#define Casual_Ste_2						0x20
#define Casual_Ste_3						0x30
#define Casual_Ste_4						0x40
#define Casual_Ste_5						0x50
#define Casual_Ste_6						0x60
#define Casual_Ste_7						0x70
#define Casual_Ste_None					0xf0//�涯����
/*******************03ת��λָ��**************************/
#define Get_Left_Box			0x00
#define Get_Right_Box			0x01
#define Return_Left_Box		0x10
#define Return_Right_Box	0x11
#define Arrive						0x20
#define Reset							0x30

/**************��������****************/
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
	 u8 RxByte;//�����ֽڽ���
	 u8 checkadd;

	u8 RecError;//���մ���
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
