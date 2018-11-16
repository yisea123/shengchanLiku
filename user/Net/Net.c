
#include "Net.h"
#include <string.h>

/***************��������***************/

volatile struct_RECEIVENET NetRecBuff[RECBUFFERNUM];
volatile struct_SENDNET NetSendBuff[SENDBUFFERNUM];
u8 NowRecNetNum=0;	//��ǰ���ջ����������������
u8 NowSendNetNum=0;	//��ǰ���ͻ����������������
static u8 NetSendingFlag;		//��ǰ���ڷ������ݵı�־
u8 sendbuff[8][8];

/*******************************************************************************
* ������  : W5500_Initialization
* ����    : W5500��ʼ����
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��
*******************************************************************************/
void W5500_Initialization(void)
{
	W5500_Init();		//��ʼ��W5500�Ĵ�������
	Detect_Gateway();	//������ط����� 
	Socket_Init(0);		//ָ��Socket(0~7)��ʼ��,��ʼ���˿�0
}

/*******************************************************************************
* ������  : Load_Net_Parameters
* ����    : װ���������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : ���ء����롢�����ַ������IP��ַ���˿ںš�Ŀ��IP��ַ��Ŀ�Ķ˿ںš��˿ڹ���ģʽ
*******************************************************************************/
void Load_Net_Parameters(void)
{
	Gateway_IP[0] = 192;//�������ز���
	Gateway_IP[1] = 168;
	Gateway_IP[2] = 0;
	Gateway_IP[3] = 1;

	Sub_Mask[0]=255;//������������
	Sub_Mask[1]=255;
	Sub_Mask[2]=255;
	Sub_Mask[3]=0;

	Phy_Addr[0]=0x0c;//���������ַ
	Phy_Addr[1]=0x29;
	Phy_Addr[2]=0xab;
	Phy_Addr[3]=0x7c;
	Phy_Addr[4]=0x00;
	Phy_Addr[5]=0x01;

	IP_Addr[0]=192;//���ر���IP��ַ
	IP_Addr[1]=168;
	IP_Addr[2]=0;
	IP_Addr[3]=109;

	S0_Port[0] = 0x13;//���ض˿�0�Ķ˿ں�5000 1388
	S0_Port[1] = 0x88;

	S0_DIP[0]=192;//���ض˿�0��Ŀ��IP��ַ
	S0_DIP[1]=168;
	S0_DIP[2]=0;
	S0_DIP[3]=114;//189
	
	S0_DPort[0] = 0x17;//���ض˿�0��Ŀ�Ķ˿ں�6000
	S0_DPort[1] = 0x70;

	S0_Mode=TCP_CLIENT;//���ض˿�0�Ĺ���ģʽ,TCP�ͻ���ģʽ
}

/*******************************************************************************
* ������  : W5500_Socket_Set
* ����    : W5500�˿ڳ�ʼ������
* ����    : ��
* ���    : ��
* ����ֵ  : ��
* ˵��    : �ֱ�����4���˿�,���ݶ˿ڹ���ģʽ,���˿�����TCP��������TCP�ͻ��˻�UDPģʽ.
*			�Ӷ˿�״̬�ֽ�Socket_State�����ж϶˿ڵĹ������
*******************************************************************************/
void W5500_Socket_Set(void)
{
	if(S0_State==0)//�˿�0��ʼ������
	{
		if(S0_Mode==TCP_SERVER)//TCP������ģʽ 
		{
			if(Socket_Listen(0)==TRUE)
				S0_State=S_INIT;
			else
				S0_State=0;
		}
		else if(S0_Mode==TCP_CLIENT)//TCP�ͻ���ģʽ 
		{
			if(Socket_Connect(0)==TRUE)
				S0_State=S_INIT;
			else
				S0_State=0;
		}
		else//UDPģʽ 
		{
			if(Socket_UDP(0)==TRUE)
				S0_State=S_INIT|S_CONN;
			else
				S0_State=0;
		}
	}
}
/*******************************************************************************
* ������  : Process_Socket_Data
* ����    : W5500���ղ����ͽ��յ�������
* ����    : s:�˿ں�
* ���    : ��
* ����ֵ  : ��
* ˵��    : �������ȵ���S_rx_process()��W5500�Ķ˿ڽ������ݻ�������ȡ����,
*			Ȼ�󽫶�ȡ�����ݴ�Rx_Buffer������Temp_Buffer���������д���
*			������ϣ������ݴ�Temp_Buffer������Tx_Buffer������������S_tx_process()
*			�������ݡ�
*******************************************************************************/
void Process_Socket_Data(SOCKET s)
{
	unsigned short size;
	size=Read_SOCK_Data_Buffer(s, Rx_Buffer);
	memcpy(Tx_Buffer, Rx_Buffer, size);			
	Write_SOCK_Data_Buffer(s, Tx_Buffer, size);
}

/*******************************************************************************
* ������  : Net_Send(u8 cmd_num,u8 cmd_data,u8 rec_num,u8 size)
* ����    : �������ݵ�����
* ����    : cmd_num��ָ��
						cmd_data��ָ������
						rec_num����¼��
						size:ָ���ֽ���
* ���    : ��
* ����ֵ  : ��
* ˵��    : 
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
* ������  : Net_SendBuffer()
* ����    : ���ͻ�������
* ����    : 
* ���    : ��
* ����ֵ  : ��
* ˵��    : 
*******************************************************************************/
void Net_SendBuffer()
{
	static u16 ReSendTimer;
	static u8 size;
	u8 i,j;
	/*******�����ݷ��ͣ����ͻ���������*******/
	if(NowSendNetNum)
	{
		/**********�����ݷŵ����ͻ�����**********/
		Tx_Buffer[0] = 0xaa;//֡ͷ
		Tx_Buffer[1] = 0x55;
		Tx_Buffer[2] = NetSendBuff[1].RecNum;//��¼��	
		Tx_Buffer[3] = NetSendBuff[1].Comend;//ָ��
		Tx_Buffer[4] = Net_Id;//����ID
		Tx_Buffer[5] = NetSendBuff[1].PROT_CFG;//CAN�豸ID
		Tx_Buffer[6] = NetSendBuff[1].ComData;
		if(NetSendBuff[1].RecNum)//��¼����
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
			if( (i==7) || (i==(size-1)) || (i==(size-2)) )//i��2��ʼ�ӣ���ʾ֡ͷ������У�飬i==7ΪУ��λ i==size-1Ϊ֡β
			{
				continue;
			}
			Tx_Buffer[7]+=Tx_Buffer[i];
		}
		Tx_Buffer[size-2] = 0xa0;
		Tx_Buffer[size-1] = 0x5f;
		/**********��������**********/
		Write_SOCK_Data_Buffer(0, Tx_Buffer, size);//ָ��Socket(0~7)�������ݴ���,�˿�0����size�ֽ�����

		/**********���Ͷ���ǰ��**********/
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
* ������  : Net_Receive()
* ����    : ����W5500�����ݵ�NetRecBuff
* ����    : s:�˿ں�
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��ȡ������W5500���յ������ݴ���NetRecBuff���棬�����н���
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


	if((S0_Data & S_RECEIVE) == S_RECEIVE)//���Socket0���յ�����
	{
		S0_Data &= ~S_RECEIVE;//������ձ�־
		/*********ȡ������********/
		size=Read_SOCK_Data_Buffer(s, Rx_Buffer);//ȡ�����ݵ�Rx_Buffer�����������ֽ���
		while(size > 0)
		{
			CheckAdd = 0;//����У���
			for(i=2;i<Rx_Buffer[2]*8+10;i++)
			{
				if( (i==7) || (i==(Rx_Buffer[2]*8+9)) || (i==(Rx_Buffer[2]*8+8)) )//i��2��ʼ�ӣ���ʾ֡ͷ������У�飬i==7ΪУ��λ i==size-1Ϊ֡β
				{
					continue;
				}
				CheckAdd += Rx_Buffer[i];
			}
			if((CheckAdd == Rx_Buffer[7]) //У����ȷ
				&& (Rx_Buffer[0]==0xaa && Rx_Buffer[1]==0x55)//֡ͷ��ȷ
				&& (Rx_Buffer[8+Rx_Buffer[2]*8]==0xa0 && Rx_Buffer[9+Rx_Buffer[2]*8]==0x5f)//֡β��ȷ
				&& (Rx_Buffer[4] == IP_Addr[3])		//Ŀ�����غ�Ϊ������
				)
			{
				NowRecNetNum++;
				NetRecBuff[NowRecNetNum].RecNum = Rx_Buffer[2];//��¼��
				NetRecBuff[NowRecNetNum].Comend = Rx_Buffer[3];//ָ������
				NetRecBuff[NowRecNetNum].NetNum = Rx_Buffer[4];//���ص�ַ
				NetRecBuff[NowRecNetNum].PROT_CFG = Rx_Buffer[5];//֡��Ϣ
				NetRecBuff[NowRecNetNum].ComData = Rx_Buffer[6];//ָ������
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
* ������  : Net_Answer()
* ����    : Ӧ��
* ����    : ָ�CAN�ţ�
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��ȡ������W5500���յ������ݴ���NetRecBuff���棬�����н���
*******************************************************************************/
void Net_Answer(u8 CAN_id,u8 Cmd)
{
	Net_Send(CAN_id,0x00,Cmd,0,0);
}

/*******************************************************************************
* ������  : Net_Analysis()
* ����    : ����W5500���յ�������
* ����    : s:�˿ں�
* ���    : ��
* ����ֵ  : ��
* ˵��    : ��ȡ������W5500���յ������ݴ���NetRecBuff���棬�����н���
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
			case CMD_REPLY://����Ӧ��֡ 00
			{
				if(NetRecBuff[1].ComData == Tx_Buffer[3])//���Ӧ�������ָ������Ϊ��ǰ���ͻ����ָ�����ΪӦ��ɹ�
				{
					NetSendingFlag = 0;
				}
			}break;
			/*****  01  *****/
			case CMD_READVER://��ȡ�汾�� 01
			{
//				Net_Answer(NetRecBuff[1].PROT_CFG,NetRecBuff[1].Comend);//Ӧ��
//				/******ģ��******/
//				if(NetRecBuff[1].PROT_CFG == 0x01)//��ѯ���ذ汾
//				{
//					Net_Send(NetRecBuff[1].PROT_CFG,CMD_READVER,GATEWAY_VERSION,0,0);
//				}
//				else//��ѯ�ն˰汾
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
			case CMD_QUERYSTATE://��ѯ��ǰX�ᣬZ������λ�� 02
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
			case CMD_CTRL_XZ://����X��Z�ᵽָ��λ��	03
			{
					Action.command = NetRecBuff[1].ComData;
					Action.tray_position = NetRecBuff[1].RecNumBuff[0][4];
					if((Action.command == Get_Left_Box)||(Action.command == Get_Right_Box))//ץ��
					{
					NowAxis_X = ((Axis_X.Coordinate*X_MPP)+0.5)/1;
					NowAxis_Z = ((Axis_Z.Coordinate*Z_MPP)+0.5)/1;
						if((Axis_X.InCtrl_Flag==0) && (Axis_Z.InCtrl_Flag==0))//�����ڿ��в��ܿ���
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
					else if((Action.command == Return_Left_Box)||(Action.command == Return_Right_Box))//����
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
					else if((Action.command == Reset))//��λ
					{
						Motor_BackToZero(AXIS_X);
						Motor_BackToZero(AXIS_Z);
					}
					else//����
					{
						if((Axis_X.InCtrl_Flag==0) && (Axis_Z.InCtrl_Flag==0))//�����ڿ��в��ܿ���
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
			case CMD_INDVICTRL_XYZ://��������X,Y,Z��
			{
        //Beep(50,50,1);
				Action.command = NetRecBuff[1].ComData;
				switch(Action.command&0X30)
				{
					/**************X�ᶯ��*****************/
					case X :
					{
						switch(Action.command&0X01)
						{
							case X_BACK://X�����
							{
								DIR_X = X_DIR_BACK;
								Axis_X.NowPulse = 0;
								Axis_X.Target_Pulse = 100;
								Axis_X.InCtrl_Flag = 1;
							}break;
							case X_FRONT://X��ǰ��
							{
								DIR_X = X_DIR_FRONT;
								Axis_X.NowPulse = 0;
								Axis_X.Target_Pulse = 100;
								Axis_X.InCtrl_Flag = 1;
							}break;
							
						}
						
					}break;
					/**************Y�ᶯ��*****************/
					case Y :
					{
						switch(Action.command&0X01)
						{
							case Y_LEFT://Y��������
							{
								DIR_Y = Y_DIR_LEFT;
								Axis_Y.NowPulse = 0;
								Axis_Y.Target_Pulse = 50;
								Axis_Y.InCtrl_Flag = 1;
								Axis_Y.Dir = Y_DIR_LEFT;
							}break;
							case Y_RIGHT://Y��������
							{
								DIR_Y = Y_DIR_RIGHT;
								Axis_Y.NowPulse = 0;
								Axis_Y.Target_Pulse = 50;//1700
								Axis_Y.InCtrl_Flag = 1;
								Axis_Y.Dir = Y_DIR_RIGHT;
							}break;
						}
					}break;
					/**************Z�ᶯ��*****************/
					case Z:
					{
						switch(Action.command&0X01)
						{
							case Z_BACK://Z���½�
							{
								DIR_Z = Z_DIR_BACK;
								Axis_Z.Dir = Z_DIR_BACK;
								Axis_Z.NowPulse = 0;
//								Target_Pulse = 500;
								Axis_Z.InCtrl_Flag = 1;
							}break;
							case Z_FRONT://Z������
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
			case CMD_SPEED_STEP://���ò��������ٶȵ�λ
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
					case Speed_Stage://�ٶȵ�λ
					{
						switch(Action.command&0x0f)
						{
							case Casual_Fre_1://�ٶȵ�λ1
							{
								Action.Speed_Grade = 0;
							}break;
							case Casual_Fre_2://�ٶȵ�λ2
							{
								Action.Speed_Grade = 1;
							}break;
							case Casual_Fre_3://�ٶȵ�λ3
							{
								Action.Speed_Grade = 2;
							}break;
							case Casual_Fre_4://�ٶȵ�λ4
							{
								Action.Speed_Grade = 3;
							}break;
									case Casual_Fre_5://�ٶȵ�λ4
							{
								Action.Speed_Grade = 4;
							}break;
									case Casual_Fre_6://�ٶȵ�λ4
							{
								Action.Speed_Grade = 5;
							}break;
									case Casual_Fre_7://�ٶȵ�λ4
							{
								Action.Speed_Grade = 6;
							}break;
							default:
							{
								Action.Speed_Grade = 0;
							}break;
						
						}
					}break;
					case Step_Stage://������λ
					{
						switch(Action.command&0xf0)
						{
							case Casual_Ste_1://����1
							{
								Action.Step_Grade = 0;
							}break;
							case Casual_Ste_2://����2
							{
								Action.Step_Grade = 1;
							}break;
							case Casual_Ste_3://����3
							{
								Action.Step_Grade = 2;
							}break;
							case Casual_Ste_4://����4
							{
								Action.Step_Grade = 3;
							}break;
								case Casual_Ste_5://����5
							{
								Action.Step_Grade = 4;
							}break;
								case Casual_Ste_6://����6
							{
								Action.Step_Grade = 5;
							}break;
								case Casual_Ste_7://����7
							{
								Action.Step_Grade = 6;
							}break;
								default://����4
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

