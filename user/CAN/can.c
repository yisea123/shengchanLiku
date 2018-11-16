//#define LCD_REC_TEST   //测试用 

#include "can.h"


UnionCan_TypeDef CANSend;//CAN数据 //CAN发送的数据

#define  bassage_type   ((SendData[0]>>6)&0x01)==1? CAN_RTR_REMOTE:CAN_RTR_DATA  // 1为远程帧  0为数据帧
#define  bassage_format ((SendData[0]>>7)&0x01)==1? CAN_ID_EXT:CAN_ID_STD 	   //  1用扩展标示符	  0用标准标示符
#define  bassage_DLC  	 SendData[0]&0xf;

CANREC_typedef CANRead;	//CAN接收数据
struct_CANDATA CANSendBuffer[40];//CAN发送数据缓存
u8 NowSendCANNum;
u8 RoadRecheck_ID;			//待查路径探测器/四向转换器ID
u8 RecheckTimer_100ms;	//重新查询设备状态计时			
u16 ReSendCantime_ms;
u8 Manual_BottleMsg[15];
//u8 Manual_BottleRFID[4];
//u8 Manual_BottleColID[8];
/*****************************************************************************/
//   ASCII TO INT
/****************************************************************************/
unsigned int ASCIITOInt(unsigned char ASCII1,unsigned char ASCII2,unsigned char ASCII3,unsigned char ASCII4)
{
	unsigned int ReturnValue;
	ReturnValue = (ASCII1<<24)+(ASCII2<<16)+(ASCII3<<8)+ASCII4;
	return ReturnValue;
}
/*****************************************************************************/
//   HEX TO BCD
/****************************************************************************/
unsigned char HexToBcd(unsigned int Hex)
{
	Hex&=0xff;
	return (Hex/10*16)+(Hex%10);
}
/*****************************************************************************/
//       CAN 初始化函数
//       输入参数 baudrate 设置CAN波特率
//       返回参数  无
/****************************************************************************/
void init_can(u8 baudrate)
{
//    u8 rxfneid[4]={0x00,0x01,0x00,0x00};			     //can  接收id

    GPIO_InitTypeDef       GPIO_InitStructure;	 //GPIO Init structure definition  
    CAN_InitTypeDef        CAN_InitStructure;	 //CAN init structure definition
		CAN_FilterInitTypeDef  CAN_FilterInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);   //使能can外设时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
    //=====================GPIO================================//

    /* Configure can Tx (PA.12) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure CAN Rx (PA.11) as input IPU */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);


     //=====================CAN控制器初始化==========================//

    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN_InitStructure);

    CAN_InitStructure.CAN_TTCM=DISABLE;          //失能时间触发
    CAN_InitStructure.CAN_ABOM=DISABLE;          //失能自动离线管理
    CAN_InitStructure.CAN_AWUM=DISABLE; 	     //失能自动唤醒
    CAN_InitStructure.CAN_NART=DISABLE;          //失能报文失败一直重传，直到成功
    CAN_InitStructure.CAN_RFLM=DISABLE;          //失能FIFO锁定功能
    CAN_InitStructure.CAN_TXFP=DISABLE;          //失能fifo发送优先级
		CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;  //can工作在正常模式 CAN_Mode_Normal   CAN_Mode_LoopBack
//	CAN_InitStructure.CAN_Mode=CAN_Mode_LoopBack;
//	CAN_InitStructure.CAN_Mode=CAN_Mode_Silent_LoopBack; 
    CAN_InitStructure.CAN_SJW =CAN_SJW_1tq;		 //重新跳跃宽度1个时间单位
    CAN_InitStructure.CAN_BS1 =CAN_BS1_8tq;		 //时间段1为3个时间单位
    CAN_InitStructure.CAN_BS2 =CAN_BS2_7tq;		 //时间段2为2个时间单位
    CAN_InitStructure.CAN_Prescaler =baudrate;   //设置一个时间单位的长度	 波特率=PCLK1/(1+8+7)/36=62.5K
    CAN_Init(CAN1,&CAN_InitStructure);


	  /* CAN filter init */
		CAN_FilterInitStructure.CAN_FilterNumber=0;
		CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
		CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
		CAN_FilterInitStructure.CAN_FilterIdHigh=0x0;
		CAN_FilterInitStructure.CAN_FilterIdLow=0x0;
		CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0;
		CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0;  
		CAN_FilterInitStructure.CAN_FilterFIFOAssignment=0;
		CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
		CAN_FilterInit(&CAN_FilterInitStructure);

//    rxfneid[0] = Devi;
//    set_scu_can_ID(rxfneid);                     //初始化过滤器

    CAN_ITConfig(CAN1,CAN_IT_FMP0, DISABLE);		     //失能消息挂号中断
    //CAN_ITConfig(CAN1,CAN_IT_TME , ENABLE);	 //使能发送邮箱空中断
}

//***************************************************************************/
//  can  发送任意长度报文		 注：报文数据长度<=8
//  输入参数  无
//  返回参数  无
//**************************************************************************/
void SendMessage(u8 *SendData)
{
	u8 i,j;
//  u8 sendbuff[32];
	unsigned char transmit_mailbox;
	CanTxMsg TxMessage;
//	u8 mailbox;

	TxMessage.RTR=bassage_type; 		  //设置报文类型
	TxMessage.IDE=bassage_format;		  //帧类型   扩展帧
	TxMessage.DLC=bassage_DLC;			  //设置报文长度									
	TxMessage.StdId=((SendData[1]<<3)|(SendData[2]>>5))& 0x7ff;			    //标准标示符
	TxMessage.ExtId=(SendData[4]>>3)|(SendData[3]<<5)|(SendData[2]<<13);    //扩展标示符
	for(i=0;i<(SendData[0]&0x0f);i++)
	{
		TxMessage.Data[i]=SendData[i+5];
	}
//	for(i=0;i<(SendData[0]&0x0f);i++)
//	{
//		TxMessage.Data[i]=SendData[i+3];
//	}

//	mailbox=CAN_Transmit(CAN1, &TxMessage);			 //返回发送邮箱号   若无空邮箱返回CAN_NO_MB
	transmit_mailbox=CAN_Transmit(CAN1, &TxMessage);			 //返回发送邮箱号   若无空邮箱返回CAN_NO_MB
	if(transmit_mailbox==CAN_TxStatus_NoMailBox)//发送邮箱满了
	{
//		sendbuff[0]=0xaa;
//		sendbuff[1]=0x55;					
//		sendbuff[2]=2;
//		sendbuff[3]=0xc2;
//		sendbuff[4]=0x01;
//		sendbuff[5]=0xff;
//		sendbuff[6]=0xff;
//		sendbuff[8]=0xcc;
//		sendbuff[9]=0xcc;
//		sendbuff[10]=0xcc;
//		sendbuff[11]=0xcc;
//		sendbuff[12]=sendbuff[13]=sendbuff[14]=sendbuff[15]=0;				
//		sendbuff[16]=0xcc;
//		sendbuff[17]=0xcc;
//		sendbuff[18]=0xcc;
//		sendbuff[19]=0xcc;
//		sendbuff[20]=0xcc;
//		sendbuff[21]=0xcc;
//		sendbuff[22]=0xff;
//		sendbuff[23]=0;

//		sendbuff[7]=0;
//		for(i=2;i<24;i++)
//		{
//			if(i!=7)sendbuff[7]+=sendbuff[i];
//		}
//		Uart1AskBytes(sendbuff,24);
		
		init_can(baudrate_62_5K);         //CAN初始化
		
		return;
	}
	i = 0;
  // 用于检查消息传输是否正常
  while((CAN_TransmitStatus(CAN1, transmit_mailbox) != CANTXOK) && (i != 0xFF))
  {
    j = CAN_TransmitStatus(CAN1, transmit_mailbox);
   i++;
  }
  i = 0;
  // 检查返回的挂号的信息数目
  while((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (i != 0xFF))
  {
   i++;
  }
}
//***************************************************************************/
//  can  发送任意长度报文		 注：报文数据长度<=8  大数据排队模式
//  输入参数  无
//  返回参数  无
//**************************************************************************/
void SendMessage_BigData(u8 *SendData)
{
	u8 i;
	unsigned char transmit_mailbox;
	CanTxMsg TxMessage;

	TxMessage.RTR=bassage_type; 		  //设置报文类型
	TxMessage.IDE=bassage_format;		  //帧类型   扩展帧
	TxMessage.DLC=bassage_DLC;			  //设置报文长度									
	TxMessage.StdId=((SendData[1]<<3)|(SendData[2]>>5))& 0x7ff;			    //标准标示符
	TxMessage.ExtId=(SendData[4]>>3)|(SendData[3]<<5)|(SendData[2]<<13);    //扩展标示符
	for(i=0;i<(SendData[0]&0x0f);i++)
	{
		TxMessage.Data[i]=SendData[i+5];
	}

	transmit_mailbox = CAN_Transmit(CAN1, &TxMessage);
	while(transmit_mailbox)
	{
		transmit_mailbox = CAN_Transmit(CAN1, &TxMessage);
	}
	
	i = 0;
  // 用于检查消息传输是否正常
  while((CAN_TransmitStatus(CAN1, transmit_mailbox) != CANTXOK) && (i != 0xFF))
  {
   i++;
  }
  i = 0;
  // 检查返回的挂号的信息数目
  while((CAN_MessagePending(CAN1, CAN_FIFO0) < 1) && (i != 0xFF))
  {
   i++;
  }
}

///***********************************************************************************
//								查询是否收到数据
//	返回值:如果没有数据，则返回-1，
//			否则，返回收到数据的缓冲区号
//	Note: 如果两个缓冲区都收到数据，则返回第一个缓冲区
//***********************************************************************************/
u8 canPoll(void)
{
  u8 i=0;
  i=(u8)(CAN1->RF0R&(u32)0x03);  //报文数目
  switch(i)
  {
   	 case 1:  return 1;
	 case 2:  return 2;
	 case 3:  return 3;
	 default: return 0;
  }	
}
//
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Name:Can_Read(int n,unsigned long *can_id,unsigned char *data, unsigned char *dlc,int *rxRTR,int *IsExt)
Function Description:	CAN总线读取数据
                     n=0表示接收缓冲器0，n=1表示接收缓冲器1；；can_id表示ID值；*data表示数据；
                     dlc表示data length code；rxRTR表示是否为远程帧；IsExt为1表示写入扩展ID
Returns:		N/A
-------------------------------------------------------------------*/
u8 canRead(u8 n, u8 *can_id, u8 *Data)
{
   CanRxMsg RxMessage;
   u8 i,dlc;		   //接收数据长度
   u32 std_id,ext_id;

   dlc=(u8)0x0F & CAN1->sFIFOMailBox[0].RDTR;    //读取接收数据长度
   std_id = (u32)0x000007FF & (CAN1->sFIFOMailBox[0].RIR >> 21);			//标准标示符  11位
   ext_id= (u32)0x3FFFF & (CAN1->sFIFOMailBox[0].RIR>>3);				   // 扩展标示符  18位

   can_id[0]=(std_id>>3)&0xff;
   can_id[1]=((std_id&0x07)<<5)|((u32)0x1f&(ext_id>>13));
   can_id[2]= (u32)0xff&(ext_id>>5);
   can_id[3]=((u32)0x1f&ext_id)<<3;


   CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);

   for(i=0;i<dlc;i++)
   {
   	   Data[i]=RxMessage.Data[i];
   }
	return (dlc);
}



//===========================================================================
//
//                       通信应用层CAN函数
//
//===========================================================================
/***********************************************
* 函数名称：void REC_Analyze(void)
* 功能描述：解析CAN接收到的数据
* 输入参数：无
* 调用函数：无
* 返回结果：无
***********************************************/
void REC_Analyze(void)
{							
  u8 Sendbuff[32];
	u8 sendbuff[1][8];
  u8 i;
  u8 dlc;
  u8 recnum;
  if(CANRead.DES_ID==0x01)//目的地址是TCU
  {
    if(CANRead.dlc > 0x80)//
    {
      if((CANRead.PROT_CFG & 0x70) == 0x50)////帧信息  
      {
//				Beep(50,50,1);
				if(CANRead.ATS_CMD == 0x00)//应答信息
				{
					ReSendCantime_ms = 3;
				}
				for(i=0;i<CANRead.dlc-0x80;i++)
				{
					sendbuff[0][i] = CANRead.Cdata[i+1];					
				}
        for(i=CANRead.dlc-0x80;i<8;i++)
        {
          sendbuff[0][i] = 0;
        }
        if(CANRead.dlc > 0x81)
        {
          recnum = 1;
        }
        else
        {
          recnum = 0;
        }
				Net_Send(CANRead.SRC_ID,CANRead.ATS_CMD>>3,CANRead.Cdata[0],recnum,sendbuff[0]);
      }
    }
  }	
}
/***********************************************
* 函数名称：void SURE_REC_Analyze(void)
* 功能描述：判断是否要解析串口接收到的数据
* 输入参数：无
* 调用函数：无
* 返回结果：无
***********************************************/
void SURE_REC_Analyze(void)
{
	unsigned char recbit;
	unsigned char id_temp[4];
	
//	CANREC_typedef *CANR;
//	CANR=&CANRead;
	
	recbit=canPoll();
	
	if(recbit)                              //等待接收
  {		
		CANRead.dlc=canRead(recbit, id_temp, CANRead.Cdata)+0x80;//CAN总线读取数据
    CANRead.DES_ID=id_temp[0]; //目的地址
		CANRead.SRC_ID=id_temp[1];//源地址
		CANRead.PROT_CFG=id_temp[2]; //帧类型
		CANRead.ATS_CMD=id_temp[3]; //命令

		REC_Analyze();
	}
}

/***********正应答，每接收到指令都先返回一条正应答***********/
//输入：待应答指令
//void CommondAnswer(u8 CMD)
//{
//  CANSend.CanData.dlc=0x81;
//  CANSend.CanData.DES_ID=0x01;//目地地址
//  CANSend.CanData.SRC_ID=Device_Prop.DEVICE_ID;//源地址
//  CANSend.CanData.PROT_CFG=0x51;//帧信息
//  CANSend.CanData.ATS_CMD=0x00<<3;//指令	
//  CANSend.CanData.Cdata[0] = CMD;
//  SendMessage(CANSend.CanSenBuf);			
//}

void CAN_SendData(u8 Dlc, u8 Des_ID,u8 Cmd, u8 *Data)
{
  u8 i;
  CANSend.CanData.dlc=Dlc;
  CANSend.CanData.DES_ID=Des_ID;//目地地址
  CANSend.CanData.SRC_ID=Device_Prop.DEVICE_ID;//源地址
  CANSend.CanData.PROT_CFG=0x41;//帧信息
  CANSend.CanData.ATS_CMD=Cmd<<3;//指令	
  for(i=0;i<Dlc-0x80;i++)
  {
    CANSend.CanData.Cdata[i] = Data[i];
  }
  SendMessage(CANSend.CanSenBuf);			
}

void CAN_SendDataToBuff(u8 *SendData)
{
	u8 i;
	CANSendBuffer[NowSendCANNum].dlc = SendData[0];
	CANSendBuffer[NowSendCANNum].DES_ID = SendData[1];
	CANSendBuffer[NowSendCANNum].SRC_ID = SendData[2];
	CANSendBuffer[NowSendCANNum].PROT_CFG = SendData[3];
	CANSendBuffer[NowSendCANNum].ATS_CMD = SendData[4];
	for(i=0;i<CANSendBuffer[NowSendCANNum].dlc-0x80;i++)
	{
		CANSendBuffer[NowSendCANNum].Cdata[i] = SendData[5+i];		
	}	
	NowSendCANNum++;
}
void CAN_SendBuffer_1ms()
{
	u8 i,j;
	if(NowSendCANNum)
	{
		//当前没有CAN数据在发送，则发送第一条CAN数据
		if(ReSendCantime_ms == 0)
		{
			ReSendCantime_ms = 1000;
			CANSend.CanData.dlc=CANSendBuffer[0].dlc;
			CANSend.CanData.DES_ID=CANSendBuffer[0].DES_ID;//目地地址
			CANSend.CanData.SRC_ID=CANSendBuffer[0].SRC_ID;//源地址
			CANSend.CanData.PROT_CFG=CANSendBuffer[0].PROT_CFG;//帧信息
			CANSend.CanData.ATS_CMD=CANSendBuffer[0].ATS_CMD;//指令	
			for(i=0;i<CANSend.CanData.dlc-0x80;i++)
			{
				CANSend.CanData.Cdata[i] = CANSendBuffer[0].Cdata[i];
			}
			SendMessage(CANSend.CanSenBuf);			
		}
		else 
		{
			//判断反馈
			if(ReSendCantime_ms == 3)//接收到应答
			{
				ReSendCantime_ms = 0;
				NowSendCANNum--;
				//队列前移
				for(i=0;i<NowSendCANNum;i++)
				{
					CANSendBuffer[i].dlc 					= CANSendBuffer[i+1].dlc;
					CANSendBuffer[i].ATS_CMD 			= CANSendBuffer[i+1].ATS_CMD;
					CANSendBuffer[i].DES_ID 			= CANSendBuffer[i+1].DES_ID;
					CANSendBuffer[i].PROT_CFG 		= CANSendBuffer[i+1].PROT_CFG;
					CANSendBuffer[i].SRC_ID 			= CANSendBuffer[i+1].SRC_ID;
					for(j=0;j<CANSendBuffer[i].dlc-0x80;j++)
					{
						CANSendBuffer[i].Cdata[j] = CANSendBuffer[i+1].Cdata[j];
					}
				}
			}
			//判断超时
			else if(ReSendCantime_ms == 5)//超时
			{
				/*-*///超时处理
				
				ReSendCantime_ms = 0;
				//任务前移
				NowSendCANNum--;
				//队列前移
				for(i=0;i<NowSendCANNum;i++)
				{
					CANSendBuffer[i].dlc 					= CANSendBuffer[i+1].dlc;
					CANSendBuffer[i].ATS_CMD 			= CANSendBuffer[i].ATS_CMD;
					CANSendBuffer[i].DES_ID 			= CANSendBuffer[i].DES_ID;
					CANSendBuffer[i].PROT_CFG 		= CANSendBuffer[i].PROT_CFG;
					CANSendBuffer[i].SRC_ID 			= CANSendBuffer[i].SRC_ID;
					for(j=0;j<CANSendBuffer[i].dlc-0x80;j++)
					{
						CANSendBuffer[i].Cdata[j] = CANSendBuffer[i+1].Cdata[j];
					}
				}
			}
			//400ms重发
			else if(ReSendCantime_ms == 600 || ReSendCantime_ms == 200)//重发
			{
				ReSendCantime_ms = 1000;
				CANSend.CanData.dlc=CANSendBuffer[0].dlc;
				CANSend.CanData.DES_ID=CANSendBuffer[0].DES_ID;//目地地址
				CANSend.CanData.SRC_ID=CANSendBuffer[0].SRC_ID;//源地址
				CANSend.CanData.PROT_CFG=CANSendBuffer[0].PROT_CFG;//帧信息
				CANSend.CanData.ATS_CMD=CANSendBuffer[0].ATS_CMD;//指令	
				for(i=0;i<CANSend.CanData.dlc-0x80;i++)
				{
					CANSend.CanData.Cdata[i] = CANSendBuffer[0].Cdata[i];
				}
				SendMessage(CANSend.CanSenBuf);			
			}
		}
	}
}




