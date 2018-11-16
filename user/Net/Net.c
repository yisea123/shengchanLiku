
#include "Net.h"
#include <string.h>

/***************变量定义***************/

volatile struct_RECEIVENET NetRecBuff[RECBUFFERNUM];
volatile struct_SENDNET NetSendBuff[SENDBUFFERNUM];
u8 NowRecNetNum=0;	//当前接收缓存的网络数据条数
u8 NowSendNetNum=0;	//当前发送缓存的网络数据条数
static u8 NetSendingFlag;		//当前正在发送数据的标志
u8 sendbuff[8][8];

/*******************************************************************************
* 函数名  : W5500_Initialization
* 描述    : W5500初始配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 无
*******************************************************************************/
void W5500_Initialization(void)
{
	W5500_Init();		//初始化W5500寄存器函数
	Detect_Gateway();	//检查网关服务器 
	Socket_Init(0);		//指定Socket(0~7)初始化,初始化端口0
}

/*******************************************************************************
* 函数名  : Load_Net_Parameters
* 描述    : 装载网络参数
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 网关、掩码、物理地址、本机IP地址、端口号、目的IP地址、目的端口号、端口工作模式
*******************************************************************************/
void Load_Net_Parameters(void)
{
	Gateway_IP[0] = 192;//加载网关参数
	Gateway_IP[1] = 168;
	Gateway_IP[2] = 0;
	Gateway_IP[3] = 1;

	Sub_Mask[0]=255;//加载子网掩码
	Sub_Mask[1]=255;
	Sub_Mask[2]=255;
	Sub_Mask[3]=0;

	Phy_Addr[0]=0x0c;//加载物理地址
	Phy_Addr[1]=0x29;
	Phy_Addr[2]=0xab;
	Phy_Addr[3]=0x7c;
	Phy_Addr[4]=0x00;
	Phy_Addr[5]=0x01;

	IP_Addr[0]=192;//加载本机IP地址
	IP_Addr[1]=168;
	IP_Addr[2]=0;
	IP_Addr[3]=109;

	S0_Port[0] = 0x13;//加载端口0的端口号5000 1388
	S0_Port[1] = 0x88;

	S0_DIP[0]=192;//加载端口0的目的IP地址
	S0_DIP[1]=168;
	S0_DIP[2]=0;
	S0_DIP[3]=114;//189
	
	S0_DPort[0] = 0x17;//加载端口0的目的端口号6000
	S0_DPort[1] = 0x70;

	S0_Mode=TCP_CLIENT;//加载端口0的工作模式,TCP客户端模式
}

/*******************************************************************************
* 函数名  : W5500_Socket_Set
* 描述    : W5500端口初始化配置
* 输入    : 无
* 输出    : 无
* 返回值  : 无
* 说明    : 分别设置4个端口,根据端口工作模式,将端口置于TCP服务器、TCP客户端或UDP模式.
*			从端口状态字节Socket_State可以判断端口的工作情况
*******************************************************************************/
void W5500_Socket_Set(void)
{
	if(S0_State==0)//端口0初始化配置
	{
		if(S0_Mode==TCP_SERVER)//TCP服务器模式 
		{
			if(Socket_Listen(0)==TRUE)
				S0_State=S_INIT;
			else
				S0_State=0;
		}
		else if(S0_Mode==TCP_CLIENT)//TCP客户端模式 
		{
			if(Socket_Connect(0)==TRUE)
				S0_State=S_INIT;
			else
				S0_State=0;
		}
		else//UDP模式 
		{
			if(Socket_UDP(0)==TRUE)
				S0_State=S_INIT|S_CONN;
			else
				S0_State=0;
		}
	}
}
/*******************************************************************************
* 函数名  : Process_Socket_Data
* 描述    : W5500接收并发送接收到的数据
* 输入    : s:端口号
* 输出    : 无
* 返回值  : 无
* 说明    : 本过程先调用S_rx_process()从W5500的端口接收数据缓冲区读取数据,
*			然后将读取的数据从Rx_Buffer拷贝到Temp_Buffer缓冲区进行处理。
*			处理完毕，将数据从Temp_Buffer拷贝到Tx_Buffer缓冲区。调用S_tx_process()
*			发送数据。
*******************************************************************************/
void Process_Socket_Data(SOCKET s)
{
	unsigned short size;
	size=Read_SOCK_Data_Buffer(s, Rx_Buffer);
	memcpy(Tx_Buffer, Rx_Buffer, size);			
	Write_SOCK_Data_Buffer(s, Tx_Buffer, size);
}

/*******************************************************************************
* 函数名  : Net_Send(u8 cmd_num,u8 cmd_data,u8 rec_num,u8 size)
* 描述    : 发送数据到缓存
* 输入    : cmd_num：指令
						cmd_data：指令数据
						rec_num：记录数
						size:指令字节数
* 输出    : 无
* 返回值  : 无
* 说明    : 
*******************************************************************************/
void Net_Send(u8 PROT_CFG,u8 cmd_num,u8 cmd_data,u8 rec_num,u8 *rec_data)
{
	u8 i,j;
	NowSendNetNum++;
	NetSendBuff[NowSendNetNum].PROT_CFG = PROT_CFG;
	NetSendBuff[NowSendNetNum].Comend = cmd_num;
	NetSendBuff[NowSendNetNum].ComData = cmd_data;
	NetSendBuff[NowSendNetNum].RecNum = rec_num;
	if(rec_num)
	{
    for(i=0;i<8;i++)
    {
      NetSendBuff[NowSendNetNum].RecNumBuff[0][i]=rec_data[i];
    }
	}
	Net_SendBuffer();
}

/*******************************************************************************
* 函数名  : Net_SendBuffer()
* 描述    : 发送缓存数据
* 输入    : 
* 输出    : 无
* 返回值  : 无
* 说明    : 
*******************************************************************************/
void Net_SendBuffer()
{
	static u16 ReSendTimer;
	static u8 size;
	u8 i,j;
	/*******无数据发送，发送缓存区数据*******/
	if(NowSendNetNum)
	{
		/**********将数据放到发送缓存区**********/
		Tx_Buffer[0] = 0xaa;//帧头
		Tx_Buffer[1] = 0x55;
		Tx_Buffer[2] = NetSendBuff[1].RecNum;//记录数	
		Tx_Buffer[3] = NetSendBuff[1].Comend;//指令
		Tx_Buffer[4] = Net_Id;//网关ID
		Tx_Buffer[5] = NetSendBuff[1].PROT_CFG;//CAN设备ID
		Tx_Buffer[6] = NetSendBuff[1].ComData;
		if(NetSendBuff[1].RecNum)//记录数据
		{
			for(j=0;j<NetSendBuff[1].RecNum;j++)
			{
				for(i=0;i<8;i++)
				{
					Tx_Buffer[((j+1)*8)+i]=NetSendBuff[1].RecNumBuff[j][i];
				}
			}
		}		
		Tx_Buffer[7] = 0;
		size = (NetSendBuff[1].RecNum+1)*8+2;
		for(i=2;i<size;i++)
		{
			if( (i==7) || (i==(size-1)) || (i==(size-2)) )//i从2开始加，表示帧头不加入校验，i==7为校验位 i==size-1为帧尾
			{
				continue;
			}
			Tx_Buffer[7]+=Tx_Buffer[i];
		}
		Tx_Buffer[size-2] = 0xa0;
		Tx_Buffer[size-1] = 0x5f;
		/**********发送数据**********/
		Write_SOCK_Data_Buffer(0, Tx_Buffer, size);//指定Socket(0~7)发送数据处理,端口0发送size字节数据

		/**********发送队列前移**********/
		for(i=0;i<(NowSendNetNum-1);i++)
		{
			NetSendBuff[i+1].RecNum=NetSendBuff[i+2].RecNum;
			NetSendBuff[i+1].Comend=NetSendBuff[i+2].Comend;
			NetSendBuff[i+1].PROT_CFG=NetSendBuff[i+2].PROT_CFG;
			NetSendBuff[i+1].ComData=NetSendBuff[i+2].ComData;
			if(NetSendBuff[i+1].RecNum)
			{
				for(j=0;j<NetSendBuff[i+1].RecNum;j++)
				{
					for(i=0;i<8;i++)
					{
						NetSendBuff[i+1].RecNumBuff[j][i]=NetSendBuff[i+2].RecNumBuff[j][i];
					}
				}
			}
		}	
		NowSendNetNum--;
	}
}


/*******************************************************************************
* 函数名  : Net_Receive()
* 描述    : 接收W5500的数据到NetRecBuff
* 输入    : s:端口号
* 输出    : 无
* 返回值  : 无
* 说明    : 先取出网络W5500接收到的数据存入NetRecBuff里面，并进行解析
*******************************************************************************/
void Net_Receive(SOCKET s)
{

	u16 size;	
	u16 i,j,k;
	u8  CheckAdd;
	u8	Rx_BufferState[32];
	u8	Size_FirstData;
//	memcpy(Tx_Buffer, Rx_Buffer, size);			
//	Write_SOCK_Data_Buffer(s, Tx_Buffer, size);


	if((S0_Data & S_RECEIVE) == S_RECEIVE)//如果Socket0接收到数据
	{
		S0_Data &= ~S_RECEIVE;//清零接收标志
		/*********取出数据********/
		size=Read_SOCK_Data_Buffer(s, Rx_Buffer);//取出数据到Rx_Buffer并返回数据字节数
		while(size > 0)
		{
			CheckAdd = 0;//清零校验和
			for(i=2;i<Rx_Buffer[2]*8+10;i++)
			{
				if( (i==7) || (i==(Rx_Buffer[2]*8+9)) || (i==(Rx_Buffer[2]*8+8)) )//i从2开始加，表示帧头不加入校验，i==7为校验位 i==size-1为帧尾
				{
					continue;
				}
				CheckAdd += Rx_Buffer[i];
			}
			if((CheckAdd == Rx_Buffer[7]) //校验正确
				&& (Rx_Buffer[0]==0xaa && Rx_Buffer[1]==0x55)//帧头正确
				&& (Rx_Buffer[8+Rx_Buffer[2]*8]==0xa0 && Rx_Buffer[9+Rx_Buffer[2]*8]==0x5f)//帧尾正确
				&& (Rx_Buffer[4] == IP_Addr[3])		//目的网关号为本网关
				)
			{
				NowRecNetNum++;
				NetRecBuff[NowRecNetNum].RecNum = Rx_Buffer[2];//记录数
				NetRecBuff[NowRecNetNum].Comend = Rx_Buffer[3];//指令类型
				NetRecBuff[NowRecNetNum].NetNum = Rx_Buffer[4];//网关地址
				NetRecBuff[NowRecNetNum].PROT_CFG = Rx_Buffer[5];//帧信息
				NetRecBuff[NowRecNetNum].ComData = Rx_Buffer[6];//指令数据
				if(NetRecBuff[NowRecNetNum].RecNum > 0)
				{
					for(j=0;j<NetRecBuff[NowRecNetNum].RecNum;j++)
					{
						for(i=0;i<8;i++)
						{
							NetRecBuff[NowRecNetNum].RecNumBuff[j][i] = Rx_Buffer[((j+1)*8)+i]; 
						}
					}
				}
			}
			else
			{
				
			}
			Size_FirstData = Rx_Buffer[2]*8+10;
			for(k=0;k<size-Size_FirstData;k++)
			{
				Rx_Buffer[k] = Rx_Buffer[Size_FirstData+k];
			}
			if(size > Size_FirstData)
			{
				size-=Size_FirstData;				
			}
			else
			{
				size = 0;
			}
		}		
	}
}

/*******************************************************************************
* 函数名  : Net_Answer()
* 描述    : 应答
* 输入    : 指令，CAN号，
* 输出    : 无
* 返回值  : 无
* 说明    : 先取出网络W5500接收到的数据存入NetRecBuff里面，并进行解析
*******************************************************************************/
void Net_Answer(u8 CAN_id,u8 Cmd)
{
	Net_Send(CAN_id,0x00,Cmd,0,0);
}

/*******************************************************************************
* 函数名  : Net_Analysis()
* 描述    : 解析W5500接收到的数据
* 输入    : s:端口号
* 输出    : 无
* 返回值  : 无
* 说明    : 先取出网络W5500接收到的数据存入NetRecBuff里面，并进行解析
*******************************************************************************/
void Net_Analysis()
{
	u8 i,j,k;

	u16 NowAxis_X;
	u16 NowAxis_Y;
	u16 NowAxis_Z;
	u16	Different_X;
	u16 Different_Y;
	u16 Different_Z;
	u8 Current_Position_Buff[8];
	u16 temp_test;
	if(NowRecNetNum > 0)
	{ 

		switch(NetRecBuff[1].Comend)
		{
			case CMD_REPLY://正负应答帧 00
			{
				if(NetRecBuff[1].ComData == Tx_Buffer[3])//如果应答回来的指令数据为当前发送缓存的指令，则视为应答成功
				{
					NetSendingFlag = 0;
				}
			}break;
			/*****  01  *****/
			case CMD_READVER://读取版本号 01
			{
//				Net_Answer(NetRecBuff[1].PROT_CFG,NetRecBuff[1].Comend);//应答
//				/******模拟******/
//				if(NetRecBuff[1].PROT_CFG == 0x01)//查询网关版本
//				{
//					Net_Send(NetRecBuff[1].PROT_CFG,CMD_READVER,GATEWAY_VERSION,0,0);
//				}
//				else//查询终端版本
//				{
//					Net_Send(NetRecBuff[1].PROT_CFG,CMD_READVER,TERMINAL_VERSION,0,0);
//				}
				if(NetRecBuff[1].PROT_CFG == 0x61)
				{
					Net_Send(NetRecBuff[1].PROT_CFG,CMD_READVER,TERMINAL_VERSION,0,0);
				}
				else
				{
					Net_Send(NetRecBuff[1].PROT_CFG,CMD_READVER,GATEWAY_VERSION,0,0);
				}
			}break;
			/*****  02  *****/
			case CMD_QUERYSTATE://查询当前X轴，Z轴坐标位置 02
			{
				Beep(50,50,1);
				Current_Position_Buff[0] = ((Axis_X.Coordinate*X_MPP)+0.5)/256;
				Current_Position_Buff[1] = (u8)(((Axis_X.Coordinate*X_MPP)+0.5)/1)%256;
				Current_Position_Buff[2] = ((Axis_Z.Coordinate*Z_MPP)+0.5)/256;
				Current_Position_Buff[3] =	(u8)(((Axis_Z.Coordinate*Z_MPP)+0.5)/1)%256;
				Current_Position_Buff[4] = ((Axis_Y.Coordinate*Y_MPP)+0.5)/256;
				Current_Position_Buff[5] = (u8)(((Axis_Y.Coordinate*Y_MPP)+0.5)/1)%256;
				Current_Position_Buff[6] = 0;
				Current_Position_Buff[7] = 0;
//				Net_Send(Prot_Cfg,CMD_QUERYSTATE,0,1,Current_Position_Buff);
				
			}break;
			/*****  03  *****/
			case CMD_CTRL_XZ://控制X，Z轴到指定位置	03
			{
					Action.command = NetRecBuff[1].ComData;
					Action.tray_position = NetRecBuff[1].RecNumBuff[0][4];
					if((Action.command == Get_Left_Box)||(Action.command == Get_Right_Box))//抓箱
					{
					NowAxis_X = ((Axis_X.Coordinate*X_MPP)+0.5)/1;
					NowAxis_Z = ((Axis_Z.Coordinate*Z_MPP)+0.5)/1;
						if((Axis_X.InCtrl_Flag==0) && (Axis_Z.InCtrl_Flag==0))//都处于空闲才能控制
						{
							Net_Send(Prot_Cfg ,CMD_CTRL_XZ,First_Stage,0,0);
							Action.Ctrl_OverFlag = 0;
							Motor_CtrlOver_Flag = 1;
							Action.Get_Box_Start_Flag = 1;
							Action.Get_Box_Stage = 1;
							Action.XZRun_Stage = 1;
							Action.XZRUN_Status_Change = 1;
							Action.Tray_PutBox_Stage = 1;
							Action.tray_stage = Zero;
							Action.Tray_PutBox_Status_Change = 1;
							Action.Tray_GetBox_Stage = 1;
							Action.Tray_GetBox_Status_Change = 1;
							Action.tray_position = NetRecBuff[1].RecNumBuff[0][4];
							Action.Target_X = (NetRecBuff[1].RecNumBuff[0][0]*256+NetRecBuff[1].RecNumBuff[0][1]);
							Action.Target_Z = (NetRecBuff[1].RecNumBuff[0][2]*256+NetRecBuff[1].RecNumBuff[0][3])-AxisZ_Float_Disance;
							Action.end_flag = 0;	
						}

					}
					else if((Action.command == Return_Left_Box)||(Action.command == Return_Right_Box))//放箱
					{
						Net_Send(Prot_Cfg ,CMD_CTRL_XZ,First_Stage,0,0);
						Motor_CtrlOver_Flag = 1;
						Action.Put_Box_Start_Flag = 1;
						Action.Ctrl_OverFlag = 0;
						Action.Put_Box_Stage = 1;
						Action.XZRun_Stage = 1;
						Action.XZRUN_Status_Change = 1;
						Action.Tray_PutBox_Stage = 1;
						Action.Tray_PutBox_Status_Change = 1;
						Action.Tray_GetBox_Stage = 1;
						Action.Tray_GetBox_Status_Change = 1;
						Action.end_flag = 0;
						Action.tray_stage  = Zero;
						Action.tray_position = NetRecBuff[1].RecNumBuff[0][4];
						Action.Target_X = (NetRecBuff[1].RecNumBuff[0][0]*256+NetRecBuff[1].RecNumBuff[0][1]);
						Action.Target_Z = (NetRecBuff[1].RecNumBuff[0][2]*256+NetRecBuff[1].RecNumBuff[0][3])+AxisZ_Float_Disance;
						Action.tray_stage  = Zero;
					}			
					else if((Action.command == Reset))//复位
					{
						Motor_BackToZero(AXIS_X);
						Motor_BackToZero(AXIS_Z);
					}
					else//到达
					{
						if((Axis_X.InCtrl_Flag==0) && (Axis_Z.InCtrl_Flag==0))//都处于空闲才能控制
						{
							Action.Ctrl_OverFlag = 0;
							Motor_CtrlOver_Flag = 1;
							Action.Get_Box_Start_Flag = 1;
							Action.Get_Box_Stage = 1;
							Action.end_flag = 0;
							Action.tray_position = NetRecBuff[1].RecNumBuff[0][4];
							Action.Target_X = (NetRecBuff[1].RecNumBuff[0][0]*256+NetRecBuff[1].RecNumBuff[0][1]);
							Action.Target_Z = (NetRecBuff[1].RecNumBuff[0][2]*256+NetRecBuff[1].RecNumBuff[0][3]);
							Action.tray_stage  = Zero;
						}
					}
			}break;
			/*****  04  *****/
			case CMD_INDVICTRL_XYZ://独立控制X,Y,Z轴
			{
        //Beep(50,50,1);
				Action.command = NetRecBuff[1].ComData;
				switch(Action.command&0X30)
				{
					/**************X轴动作*****************/
					case X :
					{
						switch(Action.command&0X01)
						{
							case X_BACK://X轴后退
							{
								DIR_X = X_DIR_BACK;
								Axis_X.NowPulse = 0;
								Axis_X.Target_Pulse = 100;
								Axis_X.InCtrl_Flag = 1;
							}break;
							case X_FRONT://X轴前进
							{
								DIR_X = X_DIR_FRONT;
								Axis_X.NowPulse = 0;
								Axis_X.Target_Pulse = 100;
								Axis_X.InCtrl_Flag = 1;
							}break;
							
						}
						
					}break;
					/**************Y轴动作*****************/
					case Y :
					{
						switch(Action.command&0X01)
						{
							case Y_LEFT://Y轴左伸缩
							{
								DIR_Y = Y_DIR_LEFT;
								Axis_Y.NowPulse = 0;
								Axis_Y.Target_Pulse = 50;
								Axis_Y.InCtrl_Flag = 1;
								Axis_Y.Dir = Y_DIR_LEFT;
							}break;
							case Y_RIGHT://Y轴右伸缩
							{
								DIR_Y = Y_DIR_RIGHT;
								Axis_Y.NowPulse = 0;
								Axis_Y.Target_Pulse = 50;//1700
								Axis_Y.InCtrl_Flag = 1;
								Axis_Y.Dir = Y_DIR_RIGHT;
							}break;
						}
					}break;
					/**************Z轴动作*****************/
					case Z:
					{
						switch(Action.command&0X01)
						{
							case Z_BACK://Z轴下降
							{
								DIR_Z = Z_DIR_BACK;
								Axis_Z.Dir = Z_DIR_BACK;
								Axis_Z.NowPulse = 0;
//								Target_Pulse = 500;
								Axis_Z.InCtrl_Flag = 1;
							}break;
							case Z_FRONT://Z轴上升
							{
								DIR_Z = Z_DIR_FRONT;
								Axis_Z.Dir = Z_DIR_FRONT;
								Axis_Z.NowPulse = 0;
//								Axis_Z.Target_Pulse = 500;
								Axis_Z.InCtrl_Flag = 1;
							}break;
						}
					}break;
				}
			}break;
			/*****  05  *****/
			case CMD_SPEED_STEP://设置步长或者速度档位
			{
				u8 temp_para;
				Action.command = NetRecBuff[1].ComData;
				temp_para = NetRecBuff[1].ComData>>4;
				if(temp_para>0)
				{
					temp_para = 0x10;
				}
				else
				{
					temp_para = 0x00;
				}
				switch(temp_para&0x10)
				{
					case Speed_Stage://速度档位
					{
						switch(Action.command&0x0f)
						{
							case Casual_Fre_1://速度档位1
							{
								Action.Speed_Grade = 0;
							}break;
							case Casual_Fre_2://速度档位2
							{
								Action.Speed_Grade = 1;
							}break;
							case Casual_Fre_3://速度档位3
							{
								Action.Speed_Grade = 2;
							}break;
							case Casual_Fre_4://速度档位4
							{
								Action.Speed_Grade = 3;
							}break;
									case Casual_Fre_5://速度档位4
							{
								Action.Speed_Grade = 4;
							}break;
									case Casual_Fre_6://速度档位4
							{
								Action.Speed_Grade = 5;
							}break;
									case Casual_Fre_7://速度档位4
							{
								Action.Speed_Grade = 6;
							}break;
							default:
							{
								Action.Speed_Grade = 0;
							}break;
						
						}
					}break;
					case Step_Stage://步长档位
					{
						switch(Action.command&0xf0)
						{
							case Casual_Ste_1://步长1
							{
								Action.Step_Grade = 0;
							}break;
							case Casual_Ste_2://步长2
							{
								Action.Step_Grade = 1;
							}break;
							case Casual_Ste_3://步长3
							{
								Action.Step_Grade = 2;
							}break;
							case Casual_Ste_4://步长4
							{
								Action.Step_Grade = 3;
							}break;
								case Casual_Ste_5://步长5
							{
								Action.Step_Grade = 4;
							}break;
								case Casual_Ste_6://步长6
							{
								Action.Step_Grade = 5;
							}break;
								case Casual_Ste_7://步长7
							{
								Action.Step_Grade = 6;
							}break;
								default://步长4
							{
								Action.Step_Grade = 0;
							}break;
						}
					}break;
				}
			}break;
			/*****  06  *****/
			case CMD_STOP:
			{
//				Axis_X.InCtrl_Flag = 0;
//				TIM_Cmd(TIM2, DISABLE);
//				Axis_Y.InCtrl_Flag = 0;
//				TIM_Cmd(TIM3, DISABLE);
//				Axis_Z.InCtrl_Flag = 0;
//				TIM_Cmd(TIM4, DISABLE);
//				Action.Get_Box_Start_Flag = 0;
//				Action.Put_Box_Start_Flag = 0;
//				Action.Target_Z = 0;
//				Action.Target_X = 0;
//				Motor_CtrlOver_Flag = 1;
//				Action.Get_Box_Stage = 1;
//				Action.Put_Box_Stage = 1;
//				Action.XZRun_Stage = 1;
//				Action.XZRUN_Status_Change = 1;
//				Action.Tray_PutBox_Stage = 1;
//				Action.Tray_PutBox_Status_Change = 1;
//				Action.Tray_GetBox_Stage = 1;
//				Action.Tray_GetBox_Status_Change = 1;
//				Action.end_flag = 0;
				StepMotorCtrl_Pulse(600,AXIS_X,1);
			}break;
			case 0x07:
			{
//				Axis_X.InCtrl_Flag = 0;
//				TIM_Cmd(TIM2, DISABLE);
//				Axis_Y.InCtrl_Flag = 0;
//				TIM_Cmd(TIM3, DISABLE);
//				Axis_Z.InCtrl_Flag = 0;
//				TIM_Cmd(TIM4, DISABLE);
//				Action.Get_Box_Start_Flag = 0;
//				Action.Put_Box_Start_Flag = 0;
//				Action.Target_Z = 0;
//				Action.Target_X = 0;
//				Motor_CtrlOver_Flag = 1;
//				Action.Get_Box_Stage = 1;
//				Action.Put_Box_Stage = 1;
//				Action.XZRun_Stage = 1;
//				Action.XZRUN_Status_Change = 1;
//				Action.Tray_PutBox_Stage = 1;
//				Action.Tray_PutBox_Status_Change = 1;
//				Action.Tray_GetBox_Stage = 1;
//				Action.Tray_GetBox_Status_Change = 1;
//				Action.end_flag = 0;
				StepMotorCtrl_Pulse(600,AXIS_X,0);
			}break;
			
		}
		NowRecNetNum--;
		for(i=1;i<NowRecNetNum+1;i++)
		{
			NetRecBuff[i].PROT_CFG = NetRecBuff[i+1].PROT_CFG;
			NetRecBuff[i].NetNum = NetRecBuff[i+1].NetNum;
			NetRecBuff[i].ComData = NetRecBuff[i+1].ComData;
			NetRecBuff[i].Comend = NetRecBuff[i+1].Comend;
			NetRecBuff[i].RecNum = NetRecBuff[i+1].RecNum;
			for(j=0;j<NetRecBuff[i].RecNum;j++)
			{
				for(k=0;k<8;k++)
				{
					NetRecBuff[i].RecNumBuff[j][k] = NetRecBuff[i+1].RecNumBuff[j][k];
				}
			}
		}
	}
}

