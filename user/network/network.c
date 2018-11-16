#include "network.h"

volatile struct_UART3 Uart3[40];
bool Reset_Flag ;//报警复位标志
bool Set_flag;					//常规设置标志 包括：1、将模块设置为STA模式 
bool Enable_Uart3_Flg;
u8 Current_Position_Buff[8] ;//返回当前坐标信息
u8 Cache_Cnt = 0;
u8 netrecflag = 0;
void Net_Analysis(void);

void USART3_Init(u32 bound)		//USART3 连接WIFI
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


  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	Uart3[Cache_Cnt].RecAdd = 0;

}

/************************************************
* 函数名称：void Uart_Send(u8 Length,u8 cmd,u8 net_num,u8 Port_cfg,u8 comdata,u8 Buf[])
* 功能描述：串口返回数剧，数据长度，指令，网关地址，帧信息，转工位指令，拓展字节
* 返回结果：无
*************************************************/
void Uart_Send(u8 Length,u8 cmd,u8 net_num,u8 Port_cfg,u8 comdata,u8 Buf[])
{
	u8 send_buf[10+8*Length];
	memset(&send_buf,0,sizeof(send_buf));
	send_buf[0]=0xaa;
	send_buf[1]=0x55;
	send_buf[2]=Length;
	send_buf[3]=cmd;
	send_buf[4]=net_num;
	send_buf[5]=Port_cfg;
	send_buf[6]=comdata;
	if(Length == 1)
	{
		send_buf[8]=Buf[0];
		send_buf[9]=Buf[1];
		send_buf[10]=Buf[2];
		send_buf[11]=Buf[3];
		send_buf[12]=Buf[4];
		send_buf[13]=Buf[5];
		send_buf[14]=Buf[6];
		send_buf[15]=Buf[7];
		send_buf[8+8*Length]=0xa0;
		send_buf[9+8*Length]=0x5f;
	}
	else
	{
		send_buf[8+8*Length]=0xa0;
		send_buf[9+8*Length]=0x5f;
	}
	for(int j=2;j<(10+8*Length);j++)//计算和校验
	{
		if( (j==7) || (j==(Length*8+8)) || (j==(Length*8+9)) )//i从2开始加，表示帧头不加入校验，i==7为校验位 i==size-1为帧尾
		{
			continue;
		}
		send_buf[7]+=send_buf[j];
	}
	for(int i=0;i<sizeof(send_buf);i++)
	{
		while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET);  
		//等待发送完成。   检测 USART_FLAG_TC 是否置1；    //见库函数 P359 介绍
		USART_SendData(USART3,send_buf[i]);
	}
}

void Uart_Analysis()
{
	if(Cache_Cnt)
		{
//			if((test_add>0)&&(Uart3[Cache_Cnt-1].Buffer[COMEND]!=0x06))//当前处于运动中且除了停止指令外不做解析，返回
//			{
//					Beep(50,50,5);
//				return ;
//			}
			if(Uart3[Cache_Cnt-1].checkadd != Uart3[Cache_Cnt-1].Buffer[CHECKADD])
			{
					Uart3[Cache_Cnt-1].checkadd = 0;
					Uart3[Cache_Cnt-1].RecAdd=0;
					memset(&Uart3[Cache_Cnt-1].Buffer,0,sizeof(Uart3[Cache_Cnt-1].Buffer));//清空数组
					Cache_Cnt--;
					Beep(50,50,3);
					return;
			}
				if(Uart3[Cache_Cnt-1].Buffer[NETNUM]==Local_Ip)//网关地址
					{					
						Uart3[Cache_Cnt-1].Buffer[CHECKADD] = 0;
						Uart3[Cache_Cnt-1].RecAdd = 0;	
						Uart3[Cache_Cnt-1].checkadd = 0;
						Action.Comend  = Uart3[Cache_Cnt-1].Buffer[COMEND];//指令集
						Action.command = Uart3[Cache_Cnt-1].Buffer[COMDATA];//转工位指令
						if(Uart3[Cache_Cnt-1].Buffer[Rec_Length>=1])//解析长报文
						{
							Action.Target_X = (u16)(Uart3[Cache_Cnt-1].Buffer[8]<<8)+ Uart3[Cache_Cnt-1].Buffer[9];//目标X轴坐标
							Action.Target_Z = (u16)(Uart3[Cache_Cnt-1].Buffer[10]<<8)+ Uart3[Cache_Cnt-1].Buffer[11];//目标Z轴坐标
							Action.tray_position = Uart3[Cache_Cnt-1].Buffer[12];//托盘属性
							Action.PlatX_Axis = (u16)(Uart3[Cache_Cnt-1].Buffer[13]<<8)+Uart3[Cache_Cnt-1].Buffer[14];//托架X轴坐标
							Action.PlatZ_Axis = (u16)(Uart3[Cache_Cnt-1].Buffer[15]<<8)+Uart3[Cache_Cnt-1].Buffer[16];//托架Z轴坐标
						}
//						for(int i=0;i<26;i++){
//							while( USART_GetFlagStatus(USART3,USART_FLAG_TC)!= SET); 
//							USART_SendData(USART3,Uart3[Cache_Cnt-1].Buffer[i]);
//						}

						
						memset(&Uart3[Cache_Cnt-1].Buffer,0,sizeof(Uart3[Cache_Cnt-1].Buffer));//清空数组
						Cache_Cnt--;
						Net_Analysis();		
				
					}
	}
}

void Net_Analysis(void)
{
		switch(Action.Comend)
		{
			case CMD_REPLY://正负应答帧 00
			{
//				if(NetRecBuff[1].ComData == Tx_Buffer[3])//如果应答回来的指令数据为当前发送缓存的指令，则视为应答成功
//				{
//					NetSendingFlag = 0;
//				}
			}break;
			/*****  01  *****/
			case CMD_READVER://读取版本号 01
			{		
//					Net_Send(NetRecBuff[1].PROT_CFG,CMD_READVER,TERMINAL_VERSION,0,0);
			}break;
			/*****  02  *****/
			case CMD_QUERYSTATE://查询当前X轴，Z轴坐标位置，以及步长速度档位
			{
				Current_Position_Buff[0] = ((Axis_X.Coordinate*X_MPP)+0.5)/256;//X坐标
				Current_Position_Buff[1] = (u8)(((Axis_X.Coordinate*X_MPP)+0.5)/1)%256;//X坐标
				Current_Position_Buff[2] = ((Axis_Z.Coordinate*Z_MPP)+0.5)/256;//Z坐标
				Current_Position_Buff[3] =	(u8)(((Axis_Z.Coordinate*Z_MPP)+0.5)/1)%256;//Z坐标
				if(Axis_Y.YCoordinate<0)	
				Current_Position_Buff[4] = 1;
				else
				Current_Position_Buff[4] = 0;
				Current_Position_Buff[5] = (u8)(((Axis_Y.YCoordinate*Y_MPP)+0.5)/1)%256;
				if(Action.Step_Grade!=1000)
				{
					Current_Position_Buff[6] = Action.Step_Grade+1;//返回步长档位
				}
				else
				{
					Current_Position_Buff[6] = 0xf0;//返回步长档位
				}
				Current_Position_Buff[7] = Action.Speed_Grade;//返回速度档位
				Uart_Send(1,CMD_QUERYSTATE,Local_Ip,Prot_Cfg,0,Current_Position_Buff);//返回坐标
				Beep(50,50,1);
			}break;
			/*****  03  *****/
			case CMD_CTRL_XZ://控制X，Z轴到指定位置	03
			{
				switch (Action.command)
				{
					case Get_Left_Box://抓左箱
					{
						if((Axis_X.InCtrl_Flag==0) && (Axis_Z.InCtrl_Flag==0)&&(Axis_Y.InCtrl_Flag==0))//都处于空闲才能控制
						{
						if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1||A_SENSOR == 1||B_SENSOR == 1)//动作前判断托盘是否位于零位，否则不执行动作
						{
							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,0xff,0);//当前托盘未处于零位，返回消息
							Beep(50,50,5);
							return ;
						}
							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,First_Stage,0);//接收消息成功，返回
	
							Motor_CtrlOver_Flag = 1;
							Action.Get_Box_Start_Flag = 1;
							Action.Get_Box_Stage = 1;
							Action.XZRun_Stage = 1;
							Action.XZRUN_Status_Change = 1;
							Action.Tray_PutBox_Stage = 1;
							Action.Tray_PutBox_Status_Change = 1;
							Action.Tray_GetBox_Stage = 1;
							Action.Tray_GetBox_Status_Change = 1;
							Action.Target_Z = Action.Target_Z-AxisZ_Float_Disance;							
							Action.end_flag = 0;
			
							Beep(50,50,1);
						}
					}break;
					case Get_Right_Box://抓右箱
					{
						if((Axis_X.InCtrl_Flag==0) && (Axis_Z.InCtrl_Flag==0) &&(Axis_Y.InCtrl_Flag==0))//都处于空闲才能控制
						{
						if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1||A_SENSOR == 1||B_SENSOR == 1)//动作前判断托盘是否位于零位，否则不执行动作
						{
							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,0xff,0);//当前托盘未处于零位，返回消息
							Beep(50,50,5);
							return ;
						}
							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,First_Stage,0);//接收消息成功，返回
	
							Motor_CtrlOver_Flag = 1;
							Action.Get_Box_Start_Flag = 1;
							Action.Get_Box_Stage = 1;
							Action.XZRun_Stage = 1;
							Action.XZRUN_Status_Change = 1;
							Action.Tray_PutBox_Stage = 1;
							Action.Tray_PutBox_Status_Change = 1;
							Action.Tray_GetBox_Stage = 1;
							Action.Tray_GetBox_Status_Change = 1;
							Action.Target_Z = Action.Target_Z-AxisZ_Float_Disance;
							Action.end_flag = 0;
						Beep(50,50,1);
						}
					}break;
					case Return_Left_Box://放右箱
					{
						if((Axis_X.InCtrl_Flag==0) && (Axis_Z.InCtrl_Flag==0) &&(Axis_Y.InCtrl_Flag==0))//都处于空闲才能控制
						{
							if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1||A_SENSOR == 1||B_SENSOR == 1)//动作前判断托盘是否位于零位，否则不执行动作
							{
								Beep(50,50,5);
								Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,0xff,0);//当前托盘未处于零位，返回消息
								return ;
							}
							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,First_Stage,0);//接收消息，返回报文

							Action.PutBox_Cnt++;//计算距离偷偷复位还有几次
							
							Motor_CtrlOver_Flag = 1;
							Action.Put_Box_Start_Flag = 1;
							Action.Put_Box_Stage = 1;
							Action.XZRun_Stage = 1;
							Action.XZRUN_Status_Change = 1;
							Action.Tray_PutBox_Stage = 1;
							Action.Tray_PutBox_Status_Change = 1;
							Action.Tray_GetBox_Stage = 1;
							Action.Tray_GetBox_Status_Change = 1;
							Action.end_flag = 0;
							Action.Target_Z = Action.Target_Z+AxisZ_Float_Disance;
							Beep(50,50,1);
						}
					}break;
					case Return_Right_Box://放左箱
					{
						if((Axis_X.InCtrl_Flag==0) && (Axis_Z.InCtrl_Flag==0) &&(Axis_Y.InCtrl_Flag==0))//都处于空闲才能控制
						{
							if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1||A_SENSOR == 1||B_SENSOR == 1)//动作前判断托盘是否位于零位，否则不执行动作
							{
								Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,0xff,0);//当前托盘未处于零位，返回消息
								Beep(50,50,5);
								return ;
							}
							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,First_Stage,0);//接收消息，返回报文
		
							Action.PutBox_Cnt++;//计算距离偷偷复位还有几次
							
							Motor_CtrlOver_Flag = 1;
							Action.Put_Box_Start_Flag = 1;
							Action.Put_Box_Stage = 1;
							Action.XZRun_Stage = 1;
							Action.XZRUN_Status_Change = 1;
							Action.Tray_PutBox_Stage = 1;
							Action.Tray_PutBox_Status_Change = 1;
							Action.Tray_GetBox_Stage = 1;
							Action.Tray_GetBox_Status_Change = 1;
							Action.end_flag = 0;
							Action.Target_Z = Action.Target_Z+AxisZ_Float_Disance;
							Beep(50,50,1);
						}
					}break;
					case Reset://复位
					{
						if((Axis_X.InCtrl_Flag==0) && (Axis_Z.InCtrl_Flag==0) && (Axis_Y.InCtrl_Flag==0))//都处于空闲才能控制
						{
							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Successed,0);//接收消息，返回报文
							XYZ_To_Zero.Start_Flag = 1;
							XYZ_To_Zero.Over_Flag = 0;
							Beep(50,50,1);
						}
					}break;
					case Arrive://到达
					{
						if((Axis_X.InCtrl_Flag==0) && (Axis_Z.InCtrl_Flag==0) && (Axis_Y.InCtrl_Flag==0))//都处于空闲才能控制
						{
							if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1||A_SENSOR == 1||B_SENSOR == 1)//动作前判断托盘是否位于零位，否则不执行动作
							{
								Beep(50,50,5);
								Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,0xff,0);//当前托盘未处于零位，返回消息
								return ;
							}							
							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,First_Stage,0);//接收消息，返回报文
			
							Motor_CtrlOver_Flag = 1;
							Action.Get_Box_Start_Flag = 1;
							Action.Get_Box_Stage = 1;
							Action.end_flag = 0;
							Action.XZRun_Stage = 1;
							Action.XZRUN_Status_Change = 1;
							Beep(50,50,1);
						}
					}break;
				}
				
			}break;
			/*****  04  *****/
			case CMD_INDVICTRL_XYZ://独立控制X,Y,Z轴
			{
				Action.Move_Alone = 1;//置位独立控制标志位
				Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Successed,0);//返回接收消息成功
				switch(Action.command&0X30)
				{
					/**************X轴动作*****************/
					case X :
					{
						if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1||A_SENSOR ==1||B_SENSOR == 1)//动作前判断托盘是否位于零位，否则不执行动作
						{
							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,0xff,0);//当前托盘未处于零位，返回消息
							Beep(50,50,5);
							return ;
						}
						switch(Action.command&0X01)
						{
							case X_BACK://X轴后退
							{	
								Axis_Move_Alone(AXIS_X , X_DIR_BACK);
							}break;
							case X_FRONT://X轴前进
							{
								Axis_Move_Alone(AXIS_X , X_DIR_FRONT);
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
								Axis_Move_Alone(AXIS_Y , Y_DIR_LEFT);
							}break;
							case Y_RIGHT://Y轴右伸缩
							{
								Axis_Move_Alone(AXIS_Y , Y_DIR_RIGHT);
							}break;
						}
					}break;
					/**************Z轴动作*****************/
					case Z:
					{
						if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1||A_SENSOR ==1||B_SENSOR == 1)//动作前判断托盘是否位于零位，否则不执行动作
						{
							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,0xff,0);//当前托盘未处于零位，返回消息
							Beep(50,50,5);
							return ;
						}
						switch(Action.command&0X01)
						{
							case Z_BACK://Z轴下降
							{
								Axis_Move_Alone(AXIS_Z , Z_DIR_BACK);
							}break;
							case Z_FRONT://Z轴上升
							{
								Axis_Move_Alone(AXIS_Z , Z_DIR_FRONT);
							}break;
						}
					}break;
				}
				Beep(50,50,1);
			}break;
			/*****  05  *****/
			case CMD_SPEED_STEP://设置步长或者速度档位
			{
				Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Successed,0);//返回设置成功信息
				u8 temp_para;
				temp_para = Action.command>>4;
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
							case Casual_Fre_5://速度档位5
							{
								Action.Speed_Grade = 4;
							}break;
							case Casual_Fre_6://速度档位6
							{
								Action.Speed_Grade = 5;
							}break;
							case Casual_Fre_7://速度档位7
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
							case Casual_Ste_None://随动步长
							{
								Action.Step_Grade = 1000;
							}break;
								default://步长4
							{
								Action.Step_Grade = 0;
							}break;
						}
					}break;
				}
				Beep(50,50,1);
			}break;
			/*****  06  *****/
			case CMD_STOP://停止指令以及报警复位清除
			{
				Uart_Send(0,CMD_STOP,Local_Ip,0x61,Successed,0);//返回接收消息成功
				if(Action.command == 0x00)//停止指令
				{
				Axis_X.InCtrl_Flag = 0;
				TIM_Cmd(TIM2, DISABLE);
				Axis_Y.InCtrl_Flag = 0;
				TIM_Cmd(TIM3, DISABLE);
				Axis_Z.InCtrl_Flag = 0;
				TIM_Cmd(TIM4, DISABLE);
				Action.Get_Box_Start_Flag = 0;
				Action.Put_Box_Start_Flag = 0;
				Action.Target_Z = 0;
				Action.Target_X = 0;
				Motor_CtrlOver_Flag = 1;
				Action.Get_Box_Stage = 1;
				Action.Put_Box_Stage = 1;
				Action.XZRun_Stage = 1;
				Action.XZRUN_Status_Change = 1;
				Action.Tray_PutBox_Stage = 1;
				Action.Tray_PutBox_Status_Change = 1;
				Action.Tray_GetBox_Stage = 1;
				Action.Tray_GetBox_Status_Change = 1;
				Action.end_flag = 0;
					
				XYZ_To_Zero.XYZ_Ret_Stage = 1;
				XYZ_To_Zero.XYZRET_Status_Change = 0;
				XYZ_To_Zero.Start_Flag = 0;
//				/******************停止返回当前坐标*************/
//				Current_Position_Buff[0] = ((Axis_X.Coordinate*X_MPP)+0.5)/256;
//				Current_Position_Buff[1] = (u8)(((Axis_X.Coordinate*X_MPP)+0.5)/1)%256;
//				Current_Position_Buff[2] = ((Axis_Z.Coordinate*Z_MPP)+0.5)/256;
//				Current_Position_Buff[3] =	(u8)(((Axis_Z.Coordinate*Z_MPP)+0.5)/1)%256;
//				if(Axis_Y.YCoordinate<0)
//				{					
//					Current_Position_Buff[4] = 1;
//				}
//				else
//				{
//					Current_Position_Buff[4] = 0;
//				}
//				Current_Position_Buff[5] = (u8)(((Axis_Y.YCoordinate*Y_MPP)+0.5)/1)%256;
//				if(Action.Step_Grade!=1000)
//				{
//					Current_Position_Buff[6] = Action.Step_Grade+1;//返回步长档位
//				}
//				else
//				{
//					Current_Position_Buff[6] = 0xf0;//返回步长档位
//				}
//				Current_Position_Buff[7] = Action.Speed_Grade;//返回速度档位
//				Uart_Send(1,CMD_QUERYSTATE,Local_Ip,Prot_Cfg,0,Current_Position_Buff);
				}
				else//报警复位
				{
					PAout(5) = 1;
					Reset_Flag = 1;
					delay_ms(1500);
					delay_ms(1500);
				}
				Beep(50,50,1);
			}break;
			/*****  07  *****/
			case CMD_TRAYSET0://托架Y坐标清零
			{
				Uart_Send(0,CMD_TRAYSET0,Local_Ip,0x61,Successed,0);//返回接收消息成功
				Axis_Y.YCoordinate = 0;
				Beep(50,50,1);
			}break;
		}	
}
void USART3_IRQHandler(void)	//串口3中断服务程序
{	
	USART_ClearITPendingBit(USART3,USART_IT_RXNE); 
	/***********报文接收******************/

		Uart3[Cache_Cnt].RxByte = USART_ReceiveData(USART3);	//接收数据暂存

	switch (netrecflag)
	{
		case 0://帧头1 AA
		{
			if(Uart3[Cache_Cnt].RxByte == 0XAA)
			{
				Uart3[Cache_Cnt].Buffer[Uart3[Cache_Cnt].RecAdd] = Uart3[Cache_Cnt].RxByte;
				netrecflag=1;
				Uart3[Cache_Cnt].RecAdd++;				
			}
			else
			{
//				Beep(50,50,3);
				netrecflag=0;
				Uart3[Cache_Cnt].RecAdd = 0;
				Uart3[Cache_Cnt].checkadd=0;
				Uart3[Cache_Cnt].RecError =1;
//				memset(&Uart3[Cache_Cnt].Buffer,0,sizeof(Uart3[Cache_Cnt].Buffer));//清空数组
			}
		}break;
		case 1://帧头2	55
		{
			if(Uart3[Cache_Cnt].RxByte == 0X55)
			{
				Uart3[Cache_Cnt].Buffer[Uart3[Cache_Cnt].RecAdd] = Uart3[Cache_Cnt].RxByte;
				netrecflag=2;
				Uart3[Cache_Cnt].RecAdd++;	
			
			}
			else
			{
//				Beep(50,50,3);
				netrecflag=0;
				Uart3[Cache_Cnt].RecAdd = 0;
				Uart3[Cache_Cnt].checkadd=0;
				Uart3[Cache_Cnt].RecError =1;
//				memset(&Uart3[Cache_Cnt].Buffer,0,sizeof(Uart3[Cache_Cnt].Buffer));//清空数组
			}
		}break;
		case 2://数据长度
		{
			Uart3[Cache_Cnt].Buffer[Uart3[Cache_Cnt].RecAdd] = Uart3[Cache_Cnt].RxByte;
			netrecflag = 3;
			Uart3[Cache_Cnt].RecAdd++;
			Uart3[Cache_Cnt].checkadd +=Uart3[Cache_Cnt].RxByte;
			
			
		}break;
		case 3://功能指令
		{
			Uart3[Cache_Cnt].Buffer[Uart3[Cache_Cnt].RecAdd] = Uart3[Cache_Cnt].RxByte;
			netrecflag = 4;
			Uart3[Cache_Cnt].RecAdd++;
			Uart3[Cache_Cnt].checkadd +=Uart3[Cache_Cnt].RxByte;
			
			
		}break;
		case 4://网关地址
		{
			Uart3[Cache_Cnt].Buffer[Uart3[Cache_Cnt].RecAdd] = Uart3[Cache_Cnt].RxByte;
			netrecflag = 5;
			Uart3[Cache_Cnt].RecAdd++;
			Uart3[Cache_Cnt].checkadd +=Uart3[Cache_Cnt].RxByte;
			
			
		}break;
		case 5://帧信息
		{
			Uart3[Cache_Cnt].Buffer[Uart3[Cache_Cnt].RecAdd] = Uart3[Cache_Cnt].RxByte;
			netrecflag = 6;
			Uart3[Cache_Cnt].RecAdd++;
			Uart3[Cache_Cnt].checkadd +=Uart3[Cache_Cnt].RxByte;
			
			
		}break;
		case 6://指令数据
		{
			Uart3[Cache_Cnt].Buffer[Uart3[Cache_Cnt].RecAdd] = Uart3[Cache_Cnt].RxByte;
			netrecflag = 7;
			Uart3[Cache_Cnt].RecAdd++;
			Uart3[Cache_Cnt].checkadd +=Uart3[Cache_Cnt].RxByte;
			
			
		}break;
		case 7://求和校验码
		{
			Uart3[Cache_Cnt].Buffer[Uart3[Cache_Cnt].RecAdd] = Uart3[Cache_Cnt].RxByte;
			netrecflag = 8;
			Uart3[Cache_Cnt].RecAdd++;
			
			
			switch (Uart3[Cache_Cnt].Buffer[Rec_Length])
			{
				case 0://无数据位
				{
					netrecflag = 9;					
				}break;
				case 1://数据长度为1
				{
					netrecflag = 8;
				}break;
				case 2://数据长度为2
				{
					netrecflag = 8;
				}break;
				default://超出数据长度
				{
//					Beep(50,50,3);
					netrecflag=0;
					Uart3[Cache_Cnt].RecAdd = 0;
					Uart3[Cache_Cnt].checkadd=0;
					Uart3[Cache_Cnt].RecError =1;
//					memset(&Uart3[Cache_Cnt].Buffer,0,sizeof(Uart3[Cache_Cnt].Buffer));//清空数组
				};
			}
		}break;
		case 8://数据接受
		{			
			switch (Uart3[Cache_Cnt].Buffer[Rec_Length])
			{
				case 1://数据长度为1
				{
					Uart3[Cache_Cnt].Buffer[Uart3[Cache_Cnt].RecAdd] = Uart3[Cache_Cnt].RxByte;
					Uart3[Cache_Cnt].RecAdd++;
					Uart3[Cache_Cnt].checkadd +=Uart3[Cache_Cnt].RxByte;
					if(Uart3[Cache_Cnt].RecAdd-CHECKADD == 8)
					{
						netrecflag = 9;
					}
				}break;
				case 2://数据长度为2
				{
					Uart3[Cache_Cnt].Buffer[Uart3[Cache_Cnt].RecAdd] = Uart3[Cache_Cnt].RxByte;					
					Uart3[Cache_Cnt].checkadd +=Uart3[Cache_Cnt].RxByte;
					if(Uart3[Cache_Cnt].RecAdd-CHECKADD == 16)
					{
						netrecflag = 9;
					}
					Uart3[Cache_Cnt].RecAdd++;
				}break;
			}
		}break;
		case 9://帧尾1 A0
		{
			if(Uart3[Cache_Cnt].RxByte ==0XA0)
			{
				Uart3[Cache_Cnt].Buffer[Uart3[Cache_Cnt].RecAdd] = Uart3[Cache_Cnt].RxByte;
				netrecflag=10;
				Uart3[Cache_Cnt].RecAdd++;	
			}
			else
			{
//				Beep(50,50,3);
				netrecflag=0;
				Uart3[Cache_Cnt].RecAdd = 0;
				Uart3[Cache_Cnt].checkadd=0;
				Uart3[Cache_Cnt].RecError =1;
//				memset(&Uart3[Cache_Cnt].Buffer,0,sizeof(Uart3[Cache_Cnt].Buffer));//清空数组
			}
		}break;
		case 10://帧尾1 5F
		{
			if(Uart3[Cache_Cnt].RxByte == 0X5F)
			{
				Uart3[Cache_Cnt].Buffer[Uart3[Cache_Cnt].RecAdd] = Uart3[Cache_Cnt].RxByte;
				netrecflag = 0;
				Uart3[Cache_Cnt].RecAdd = 0;
				Cache_Cnt++;//接收下一帧数据
				Uart3[Cache_Cnt].RecAdd = 0;
			}
			else
			{
//				Beep(50,50,3);
				netrecflag=0;
				Uart3[Cache_Cnt].RecAdd = 0;
				Uart3[Cache_Cnt].checkadd=0;
				Uart3[Cache_Cnt].RecError =1;
//				memset(&Uart3[Cache_Cnt].Buffer,0,sizeof(Uart3[Cache_Cnt].Buffer));//清空数组
			}
		}break;		
	}
} 



