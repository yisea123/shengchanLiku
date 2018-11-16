
#include <string.h>
#include "ZLG522S.h"
#include "stm32f10x_usart.h"
#include "timer.h"
#include "delay.h"
#include "Sensor.h"
#include "Motor.h"

u8 SerBfr[MAXSERSIZE];		//串行数据缓冲区
u8 IC_Buffer[MAXSERSIZE];	//与IC卡的存取数据缓冲区
u8 SerBfrREC[MAXSERSIZE];	//串行数据缓冲区
u8 IC_CardNO[4];					//卡号
u8 Coal_SampleID[8];
u8 Coal_ID[16];
u8 Status1;
u8 Status2;
bool RFID_Hand_OK; 	//读卡器初始化与串口握手成功标记

//////////////////////////////////////
u8 BottleRfidId[4];//瓶RFID
u8 ColRfidId[8];//煤样信息

u8 ComRfidRecBuff[72];	//RFID串口接收数据
u8 ComRfidRecAdd;				//RFID串口接收数据的地址，下标
u8 ComRfidSuccess;			//RFID串口接收数据成功标志
u16 DelayTimes_ms;

void USART3_Init(u32 bound)		//USART3 连接RFIC
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure; 
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//USART3时钟设置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		
	
	USART_DeInit(USART3);  //复位串口3
	//USART3_TX   PB.10
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);					//初始化PB10
	//USART3_RX   PB.11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;			   
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);								//初始化PB11
		
	//USART3初始化
	USART_InitStructure.USART_BaudRate = bound;	//一般为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//1位停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART3, &USART_InitStructure); //初始化串口
	RFID_ON();
	
	/* Enable the USART1 */

	/* CPU的小缺陷：串口配置好，如果直接Send，则第1个字节发送不出去
	如下语句解决第1个字节无法正确发送出去的问题 */
	USART_ClearFlag(USART3, USART_FLAG_TXE);     /* 清发送完成标志，Transmission Complete flag */
	USART_Cmd(USART3, ENABLE);	
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断

//#if EN_USART1_RX		  //如果使能了接收  
  //Usart3 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

//#endif
}

void RFIDdelay_us(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=10;  //尹工用的17 
      while(i--) ;    
   }
}

/**************************************************************************************************
** 函数原型:    unsigned char ZLG522S_UARTInit(void)
** 函数功能:    ZLG522S初始化
** 输入参数:    -		
** 输出参数:    
** 返 回 值:    命令执行结果
** 说    明:		该函数
**************************************************************************************************/
unsigned char ZLG522S_UARTInit(void)
{
	unsigned char temp=0,SetCunt=3;
	unsigned int rectime;
			
	while(SetCunt)
	{
		USART_SendData(USART3,0x20);

		RFIDdelay_us(40);
		USART_SendData(USART3,0x20);
		
		rectime = 30;
		while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET && rectime>0)
		{
			RFIDdelay_us(100);
			rectime--;
    }
		if(rectime == 0)
		{
			SetCunt--;
			continue;	   //一次失败
		}	
		else
		{
			temp=USART_ReceiveData(USART3);
			if(temp == 0x06)
			{
				USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);                    //使能接收中断
				return 1;   //TRUE;
			}
			else
			{
				SetCunt--;
				continue;	   //一次失败
			}		
		}
	}
	return 0;
}

/**************************************************************************************************
** 函数原型:    unsigned char ReadComRfidData(void)
** 函数功能:    读RFID的串口数据
** 输入参数:    无
** 输出参数:    
** 返 回 值:    1成功  0失败
**************************************************************************************************/
unsigned char ReadComRfidData(void)
{
	unsigned char i,BCC;
	
	if(ComRfidSuccess)// 已接收一帧数据,进行数据校验
	{
		if(ComRfidRecBuff[ComRfidRecBuff[0]-1]==0x03 && ComRfidRecBuff[0]==ComRfidRecBuff[3]+6)
		{
			// 正确接收到ETX,及帧长度等于数据长度加协议长度,则数据有效
						BCC = 0;
			for(i=0; i<(ComRfidRecBuff[0]-1); i++)
			{
				BCC ^= ComRfidRecBuff[i];
			}
			if(BCC == 0xff)
			{
				ComRfidSuccess = 0;				// BCC校验通过
				return 1;				
			}
		}
		ComRfidSuccess=0;
	}
	return 0;		
}

/**************************************************************************************************
** 函数原型:    unsigned char ReadColRfid(BottleRfidId,ColRfidId)
** 函数功能:    读RFID和煤样信息
** 输入参数:    RFID 和 煤样信息  缓冲区		
** 输出参数:    
** 返 回 值:    0成功  1(激活错  读瓶ID M指令)  2(密码验证错 F指令)  3(读煤样信息错 G指令)
** 说    明:	该函数
							1、A卡激活 M指令  返回RFID
							2、直接密码验证 F指令
							3、读缓冲区8 G指令 返回煤样信息
							4、串口数据错
**************************************************************************************************/
unsigned char ReadColRfid(unsigned char BottleRfidId_temp[],unsigned char ColRfidId_temp[])
{
	unsigned char ICSendBuffer[20];
//	unsigned char ICReadBuffer[20];
	unsigned int temp, i;
//	unsigned char readnum;
//	unsigned int rectime;
	
	ICSendBuffer[0] = 0x08;
	ICSendBuffer[1] = 0x02;
	ICSendBuffer[2] = 77;//M
	ICSendBuffer[3] = 0x02;
	ICSendBuffer[4] = 0x00;
	ICSendBuffer[5] = 0x52;
	
	//校验
	temp=ICSendBuffer[0];
	ICSendBuffer[temp - 2] = 0;   //BCC清0
	for (i = 0; i < temp - 2; i++)	   //计算BCC
	{
		ICSendBuffer[temp - 2] ^= ICSendBuffer[i];
	}
	ICSendBuffer[temp - 2] = ~ICSendBuffer[temp - 2];
	ICSendBuffer[temp - 1] = 0x03;
	//发送
	for(i=0;i<ICSendBuffer[0];i++)
	{
		while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);  
		//等待发送完成。   检测 USART_FLAG_TC 是否置1；    //见库函数 P359 介绍
		USART_SendData(USART3,ICSendBuffer[i]);
	}
	//读返回值  接收串口数据包
	DelayTimes_ms=500;
	while(DelayTimes_ms)
	{
		if(ReadComRfidData())break;
	}
	if(DelayTimes_ms==0)//串口接收错误 
	{
		return 4;
	}
	//判断读出来的状态
	if(ComRfidRecBuff[2]==0)
	{
		BottleRfidId_temp[0]=ComRfidRecBuff[11];//读取瓶ID码
		BottleRfidId_temp[1]=ComRfidRecBuff[10];
		BottleRfidId_temp[2]=ComRfidRecBuff[9];
		BottleRfidId_temp[3]=ComRfidRecBuff[8];
		//读取煤样信息
		RFIDdelay_us(10000);
//		delay_ms(10);
		
		ICSendBuffer[0] = 0x12;
		ICSendBuffer[1] = 0x02;
		ICSendBuffer[2] = 70;// 'F';
		ICSendBuffer[3] = 0x0C;
		ICSendBuffer[4] = 0X60;           //密钥A
		ICSendBuffer[5] = BottleRfidId_temp[3];
		ICSendBuffer[6] = BottleRfidId_temp[2];
		ICSendBuffer[7] = BottleRfidId_temp[1];
		ICSendBuffer[8] = BottleRfidId_temp[0];
		ICSendBuffer[9] = 0xff;
		ICSendBuffer[10] = 0xff;
		ICSendBuffer[11] = 0xff;
		ICSendBuffer[12] = 0xff;
		ICSendBuffer[13] = 0xff;
		ICSendBuffer[14] = 0xff;
		ICSendBuffer[15] = RFID_Read_Block; //
		//校验
		temp = ICSendBuffer[0];
		ICSendBuffer[temp - 2] = 0;   //BCC清0
		for (i = 0; i < temp - 2; i++)	   //计算BCC
		{
			ICSendBuffer[temp - 2] ^= ICSendBuffer[i];
		}
		ICSendBuffer[temp - 2] = (~ICSendBuffer[temp - 2]);
		ICSendBuffer[temp - 1] = 0x03;
		//发送
		for(i=0;i<ICSendBuffer[0];i++)
		{
			while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);  
			//等待发送完成。   检测 USART_FLAG_TC 是否置1；    //见库函数 P359 介绍
			USART_SendData(USART3,ICSendBuffer[i]);
		}
		//读返回值  接收串口数据包
		DelayTimes_ms=500;
		while(DelayTimes_ms)
		{
			if(ReadComRfidData())break;
		}
		if(DelayTimes_ms==0)//串口接收错误 
		{
			return 4;
		}	
		//判断读出来的状态
		if(ComRfidRecBuff[2]==0)
		{
			//读卡中的煤样信息
			ICSendBuffer[0] = 0x07;
			ICSendBuffer[1] = 0x02;
			ICSendBuffer[2] = 0x47;// 'G';
			ICSendBuffer[3] = 0x01;
			ICSendBuffer[4] = RFID_Read_Block;//8块数据  XiangYang5
			//校验
			temp = ICSendBuffer[0];
			ICSendBuffer[temp - 2] = 0;   //BCC清0
			for (i = 0; i < temp - 2; i++)	   //计算BCC
			{
					ICSendBuffer[temp - 2] ^= ICSendBuffer[i];
			}
			ICSendBuffer[temp - 2] = (~ICSendBuffer[temp - 2]);
			ICSendBuffer[temp - 1] = 0x03;
			//发送
			for(i=0;i<ICSendBuffer[0];i++)
			{
				while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);  
				//等待发送完成。   检测 USART_FLAG_TC 是否置1；    //见库函数 P359 介绍
				USART_SendData(USART3,ICSendBuffer[i]);
			}
			//读返回值  接收串口数据包
			DelayTimes_ms=500;
			while(DelayTimes_ms)
			{
				if(ReadComRfidData())break;
			}
			if(DelayTimes_ms==0)//串口接收错误 
			{
				return 4;
			}			

			if(ComRfidRecBuff[2]==0)
			{
				for (i = 0; i < 8; i++)//读煤样信息
				{
						ColRfidId_temp[i] = ((ComRfidRecBuff[4+i*2]-'0')<<4) | (ComRfidRecBuff[4+i*2+1]-'0');
				}
				return 0; //0				
			}
			else 
			{
				return 3;//读煤样信息错 G指令 3
			}
		}//end  if(ICReadBuffer[2]==0)
		else 
		{
			return 2;//密码验证错 F指令 2
		}
	}
	else 
	{
		return 1;//激活错  读瓶ID M指令 1
	}
}




/////////////////////////////////////////////////////////////////////////////////////////////////////
void IC_RevBuffer(void)	    //接收串口数据包
{
	u8 i,n=3;	
	while(n)                  //三次循环
	{
		IC_RevTime=200;
		while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET && IC_RevTime>0);
		USART_ClearFlag(USART3,USART_FLAG_RXNE);	
		
		SerBfrREC[0]=USART_ReceiveData(USART3);
		if(IC_RevTime==0)
		{
			n--;
			continue;
		}
		else
		{
			for(i=0; i<SerBfrREC[0]; i++)
			{
				IC_RevTime=20;
				while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET && IC_RevTime>0);
				USART_ClearFlag(USART3,USART_FLAG_RXNE); 
				SerBfrREC[i+1]=USART_ReceiveData(USART3);
				if(IC_RevTime==0)
				{
					n--;
					continue;	
				}
			}
		}
	}
}

void IC_AddBufferCheck(void)                           //数据包的结束及CRC计算
{
	u8 temp,i;

	temp = IC_Buffer[0];
	IC_Buffer[temp - 2] = 0;   //BCC清0

	for(i=0; i<temp-2; i++)	   //计算BCC
	{
		IC_Buffer[temp - 2] ^= IC_Buffer[i];
	}
	IC_Buffer[temp - 2] = ~IC_Buffer[temp - 2];
	
	IC_Buffer[temp - 1] = ETX; 
}

void SendArr_UART(u8 *PtrSend, u8 Count)  //串口发送数据包
{
	u8 i;
	for(i=0; i<Count; i++)
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);  
		USART_SendData(USART3,PtrSend[i]);
	}
}

/**************************************************************************************************
** 函数原型:    unsigned char PiccActiveTypeA(	unsigned char BaudRate,unsigned char Req_Code,
**												unsigned char *Atq,unsigned char *Ats,
**												unsigned char *PiccSnrLen,unsigned char *PiccSnr)
** 函数功能:    模块信息配置
** 输入参数:    unsigned char BaudRate			// 保留参数，设置为0
**							unsigned char Req_Code			// 请求码	
** 输出参数:    unsigned char *Atq					// 请求回应
**							unsigned char *Ats					// 最后一级选择应答
**							unsigned char *PiccSnrLen		// 卡号长度
**							unsigned char *PiccSnr			// 卡号
** 返 回 值:    命令执行结果
**************************************************************************************************/
#if PICC_ACTIVE_TYPEA_EN
u8 PiccActiveTypeA(u8 BaudRate, u8 Req_Code, u8 *Atq, u8 *Ats, u8 *PiccSnrLen, u8 *PiccSnr)
{
	IC_Buffer[0] 	= 0x08;
	IC_Buffer[1] 	= 0x02;
	IC_Buffer[2]	= 'M';
	IC_Buffer[3] 	= 0x02;
	IC_Buffer[4] 	= 0x00;
	IC_Buffer[5]	= 0x52;
	IC_AddBufferCheck();			

	SendArr_UART(IC_Buffer,8);
	
	IC_RevBuffer();

	if(SerBfrREC[2] == 0)
	{
		PiccSnr[0]=IC_CardNO[0] = SerBfrREC[11];
		PiccSnr[1]=IC_CardNO[1] = SerBfrREC[10];
		PiccSnr[2]=IC_CardNO[2] = SerBfrREC[9];
		PiccSnr[3]=IC_CardNO[3] = SerBfrREC[8];      //得到S50卡的序列号				
		return 1;
	}
	else
	{
		return 0;
	}
}
#endif

char IC_PiccAutKey(u8 *Key)	     //直接密码验证
{
	int i;
	for(i=0;i<20;i++)
	{
		SerBfrREC[i]=0;
	}
        
	IC_Buffer[0]= 0x12;
	IC_Buffer[1]= 0x02;
	IC_Buffer[2]= 'F';
	IC_Buffer[3]= 0x0C;
	IC_Buffer[4]= 0X60;           //密钥A
	IC_Buffer[5]= Bottle_id[3];
	IC_Buffer[6]= Bottle_id[2];
	IC_Buffer[7]= Bottle_id[1];
	IC_Buffer[8]= Bottle_id[0];

	IC_Buffer[9] =Key[0];
	IC_Buffer[10]=Key[1];
	IC_Buffer[11]=Key[2];
	IC_Buffer[12]=Key[3];
	IC_Buffer[13]=Key[4];
	IC_Buffer[14]=Key[5];

	IC_Buffer[15]= 0x08;

	IC_AddBufferCheck();	
	SendArr_UART(IC_Buffer,IC_Buffer[0]);
	IC_RevBuffer();
	for(i=0;i<15;i++)
	{
		IC_Buffer[i]=0;
	}
	if(SerBfrREC[2] == 0)  //无CRC验证
	{
		return 1;              
	}
	else
	{
		return 0;               
	}       
}

char ActiveCardKey(void)  //A型卡激活和密码验证
{
	u8 n=3;
	u8 KeyA[3][6]={
            {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
            {0xA1,0xB2,0xC3,0xD4,0xE5,0xF6},    
            {0x00,0x00,0x00,0x00,0x00,0x00}};
       	
	while(n)
	{
    if(!PiccActiveTypeA(0,REQALL,ATQ,&SAK,&Len,Bottle_id)) return 0;
		if(!IC_PiccAutKey(KeyA[3-n]))             
		{
			n--;
			if(n==0) return 0;
			delay_ms(5);
			continue;
		} 
		else
		{ 
			return 1;
		}
	}
	return FALSE;
} 

unsigned char IC_ReadBlockData(u8 Block)	  //读卡
{
	IC_Buffer[0] 	= 0x07;
	IC_Buffer[1]  = 0x02;
	IC_Buffer[2]	= 'G';
	IC_Buffer[3] 	= 0x01;
	IC_Buffer[4] 	= Block;

	IC_AddBufferCheck();	
	SendArr_UART(IC_Buffer,IC_Buffer[0]);
	IC_RevBuffer();

	if(SerBfrREC[2] == 0)
	{
		Coal_ID[0]=SerBfrREC[4]-0x30;
		Coal_ID[1]=SerBfrREC[5]-0x30;
		Coal_ID[2]=SerBfrREC[6]-0x30;
		Coal_ID[3]=SerBfrREC[7]-0x30;
		Coal_ID[4]=SerBfrREC[8]-0x30;
		Coal_ID[5]=SerBfrREC[9]-0x30;
		Coal_ID[6]=SerBfrREC[10]-0x30;
		Coal_ID[7]=SerBfrREC[11]-0x30;
		Coal_ID[8]=SerBfrREC[12]-0x30;
		Coal_ID[9]=SerBfrREC[13]-0x30;
		Coal_ID[10]=SerBfrREC[14]-0x30;
		Coal_ID[11]=SerBfrREC[15]-0x30;
		Coal_ID[12]=SerBfrREC[16]-0x30;
		Coal_ID[13]=SerBfrREC[17]-0x30;
		Coal_ID[14]=SerBfrREC[18]-0x30;
		Coal_ID[15]=SerBfrREC[19]-0x30;
		Coal_SampleID[0]=Coal_ID[0]<<4 | Coal_ID[1];
		Coal_SampleID[1]=Coal_ID[2]<<4 | Coal_ID[3];
		Coal_SampleID[2]=Coal_ID[4]<<4 | Coal_ID[5];
		Coal_SampleID[3]=Coal_ID[6]<<4 | Coal_ID[7];
		Coal_SampleID[4]=Coal_ID[8]<<4 | Coal_ID[9];
		Coal_SampleID[5]=Coal_ID[10]<<4 | Coal_ID[11];
		Coal_SampleID[6]=Coal_ID[12]<<4 | Coal_ID[13];
		Coal_SampleID[7]=Coal_ID[14]<<4 | Coal_ID[15];
		return 1;  
	}
	else
	{
		return 0;
	}	
}

unsigned char IC_WriteBlockData(u8 Block)  //写卡
{
  IC_Buffer[0] 	= 0x17;
	IC_Buffer[1]  = 0x02;
	IC_Buffer[2]	= 'H';
	IC_Buffer[3] 	= 0x11;
	IC_Buffer[4] 	= Block;
        
  IC_Buffer[5]= 0x05;
	IC_Buffer[6]= 0x05;
	IC_Buffer[7]= 0x05;
	IC_Buffer[8]= 0x05;
	IC_Buffer[9] =0x05;
	IC_Buffer[10]=0x05;
	IC_Buffer[11]=0x05;
	IC_Buffer[12]=0x05;
        
	IC_Buffer[13]=0x05;
	IC_Buffer[14]=0x05;
  IC_Buffer[15]=0x05;
	IC_Buffer[16]=0x05;
  IC_Buffer[17]=0x05;
	IC_Buffer[18]=0x05;
  IC_Buffer[19]=0x05;
	IC_Buffer[20]=0x05;
        
	IC_AddBufferCheck();	
	SendArr_UART(IC_Buffer,IC_Buffer[0]);
	IC_RevBuffer();

	if(SerBfrREC[2] == 0)
	{                
		return 1;  
	}
	else
	{
		return 0;
	}
}

int IC_SearchBrush(void)
{

	if(!ActiveCardKey()) return 0;
	 
	delay_ms(10);

	if(!IC_ReadBlockData(8))  return 0; //读取2扇区8段BCD码

//	if(!IC_WriteBlockData(8))  return 0; //往2扇区8段写数据
	return 1;	
}

//#if PiccAutoDetect_en
//unsigned char PiccAutoDetect(unsigned char OutMode,  unsigned char TxMode, unsigned char Req_Code, 
//					 unsigned char AuthMode, unsigned char KEYAB,  unsigned char *Key, unsigned char Block)
//{
//	unsigned char Status;
//	
//	SerBfr[CMDTYPE] = 2;
//	SerBfr[COMMAND] = 'N';

//	if(AuthMode == 0)
//		SerBfr[LENGTH] = 4;
//	else
//	{	SerBfr[DATA+4] = KEYAB;
//		if(AuthMode == 'E')
//		{
//			SerBfr[DATA+5] = *Key;
//			SerBfr[DATA+6] = Block;
//			SerBfr[LENGTH] = 7;
//		}
//		else if(AuthMode == 'F')
//		{
//			memcpy(&SerBfr[DATA+5],Key,6);
//			SerBfr[DATA+11] = Block;
//			SerBfr[LENGTH] = 12;
//		}
//		else
//			return COMMERR;
//	}

//	SerBfr[DATA] = OutMode;
//	SerBfr[DATA+1] = TxMode;
//	SerBfr[DATA+2] = Req_Code;
//	SerBfr[DATA+3] = AuthMode;
//	
//	Status = ZLG522S_Cmd();
//	
//	if(Status == OK)
//	{
//		Status = SerBfr[STATUS];
//	}
//	return Status;
//}
//#endif


u32 BottleID_ByteTOString(u8 B1, u8 B2, u8 B3, u8 B4) //瓶/卡ID由字节转为4个连续字符串
{
	u8 temp_1, temp_2, temp_3, temp_4;
	u32 temp_u;
	
	temp_1 = B1;
	temp_2 = B2;
	temp_3 = B3;
	temp_4 = B4;
	
	temp_u = (temp_4<<24)|(temp_3<<16)|(temp_2<<8)|temp_1;
	
	return temp_u;
}





