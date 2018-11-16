#include "Action_Status.h"

u32 First_Goods_Height;
u32 Plat_Axis_Z;
u32 Plat_Axis_X;
u32 AxisZ_UpDown_Pluse;
u32 AxisZ_UpDownMore_Pluse;

TYPEDEF_BACKZERO XYZ_To_Zero=
{
	.XYZRET_Status_Change = 0,//XYZRET_Status_Change״̬�ı��־λ
	.XYZ_Ret_Stage 				= 1,//XYZ_Ret_Stage�׶���ת
	.Over_Flag 						= 0,//Over_Flag��λ������������Ϣ����λ��
	.Start_Flag 	 				= 0,//Start_Flag��λ��ʼ��־λ
	.X_Return_Flag				= 0,//X_Return_Flag��X��ص���ɱ�־λ
	.Tray_Reset_Flag			=	0,//�����Զ���λ��־λ
	.Z_Return_Flag  			= 0
};

u8 Last_AxisY_Dir = 2;//�����ϴ������˶�����
void XZ_Run(u16 AXIS_X_Mm,u16 AXIS_Z_Mm);
void Tray_Put_Box(bool Axis_Y_Dir,u32 UpDown_Pluse,bool Tray_Position);
void Tray_Get_Box(bool Axis_Y_Dir,u32 UpDown_Pluse,bool Tray_Position);


/**
  * @brief  ȡ��
  * @param  Ŀ�� X�����꣬Z������
  * @retval ��
  */
void Get_Box(u16 AXIS_X_Mm,u16 AXIS_Z_Mm)//ȡ����ߵ���
{
	if(Action.Get_Box_Start_Flag == 1)
	{
		if(Action.Get_Box_Stage == 1)//XZ�˶��׶�
		{
			if(Axis_Y.InCtrl_Flag == 0&&(A_SENSOR ==1||B_SENSOR == 1))
			{
				Beep(50,50,5);
				LED_RED = ON;//��ɫ�Ʊ���
				LED_GRE = OFF;
				LED_YEL = OFF;
				Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Error,0);//���ر�����Ϣ
				Action.Get_Box_Stage = 6;
				return;
			}
			LED_RED = OFF;
			LED_GRE = ON;//������ɫ�Ʋ�����
			LED_YEL = OFF;
			XZ_Run(AXIS_X_Mm,AXIS_Z_Mm);
			if(Action.end_flag == 1)
			{
				if(Action.command == Arrive)//ת��λָ��Ϊ��������������䶯����ֱ����ת���˶������׶�
				{
					Action.Get_Box_Stage = 6;
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Fifth_Stage,0);//XZ�˶��������������ر���

					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
				}
				else//XZ�˶�״̬������������һ���׶�
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Second_Stage,0);//XZ�˶��������������ر���

					Action.Get_Box_Stage++;
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
				}
			}
		}
		else if(Action.Get_Box_Stage == 2)//��е������׶�
		{
			Tray_Get_Box(!(Action.command&0x01),AxisZ_UpDown_Pluse+10,Goods);
			if(Action.end_flag == 1)
			{				
				if(Axis_Y.InCtrl_Flag == 0&&(A_SENSOR ==1||B_SENSOR == 1))
				{
					Beep(50,50,5);
					LED_RED = ON;//��ɫ�Ʊ���
					LED_GRE = OFF;
					LED_YEL = OFF;
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Error,0);//���ر�����Ϣ
					Action.Get_Box_Stage = 6;
					return;
				}
				if(Action.tray_position == Keep)//��������Ϊ�ޣ�����ת�������׶�
				{
					Beep(50,50,5);
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Third_Stage,0);//��������������ر���
					
					Action.Get_Box_Stage = 5;
				}
				else//�������䶯��������������һ���׶�
				{
					/**************���ڲ��Դ���************/

						if(Length_High_LimCtl())
						{
							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,WID_LENALM,0);//���س�����λ������Ϣ
							Beep(50,50,5);					
							LED_RED = ON;//��ɫ�Ʊ���
							LED_GRE = OFF;
							LED_YEL = OFF;
							Action.Get_Box_Stage = 5;
							goto STATUS5;
						}
						/**************���ڲ��Դ���************/
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Third_Stage,0);//��������������ر���

					Action.Get_Box_Stage++;
					delay_ms(ActionDelay);						
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
				}
			}
		}
		else if(Action.Get_Box_Stage == 3)//XZ�˶��׶�
		{
			if(Action.tray_position == Left_Tray||Action.tray_position == Right_Tray)
			{
				XZ_Run(Action.PlatX_Axis,Action.PlatZ_Axis+AxisZ_PutBoxFloat_Disance);
				if(Action.end_flag == 1)//XZ�˶�����������һ���׶�
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Fourth_Stage,0);//XZ�˶��������������ر���

					
					Action.Get_Box_Stage++;
					delay_ms(ActionDelay);					
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
				}
			}
		}
		else if(Action.Get_Box_Stage == 4)//��е�ַ���׶�
		{
			Tray_Put_Box(!(Action.tray_position>>1),AxisZ_UpDownMore_Pluse,Tray);
			if(Action.end_flag == 1)
			{
				if(Axis_Y.InCtrl_Flag == 0&&(A_SENSOR ==1||B_SENSOR == 1))
				{
					Beep(50,50,5);
					LED_RED = ON;//��ɫ�Ʊ���
					LED_GRE = OFF;
					LED_YEL = OFF;
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Error,0);//���ر�����Ϣ
					Action.Get_Box_Stage = 6;
					return;
				}
				
				Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Fifth_Stage,0);//���̷�����������ر���
				Action_Stop();
			}
		}
		else if(Action.Get_Box_Stage == 5)//������λ�׶�
		{
			STATUS5:
			Tray_Put_Box(!(Action.command&0x01),AxisZ_UpDown_Pluse,Goods);
			if(Action.end_flag == 1)
			{
				Action.Get_Box_Stage++;
				delay_ms(ActionDelay);				
				delay_ms(ActionDelay);
				delay_ms(ActionDelay);
				delay_ms(ActionDelay);
				delay_ms(ActionDelay);
				delay_ms(ActionDelay);
				delay_ms(ActionDelay);
				delay_ms(ActionDelay);
				delay_ms(ActionDelay);
				delay_ms(ActionDelay);
				delay_ms(ActionDelay);
			}
		}
		else//���ж��������ñ�־λ
		{
			Action_Stop();
//			Action.Get_Box_Stage = 1;
//			Action.Get_Box_Start_Flag = 0;
//			Action.end_flag = 0;
//			Action.Target_X = 0;
//			Action.Target_Z = 0;
		}
	}
}
	
/**
  * @brief  ����
  * @param  Ŀ�� X�����꣬Z������
  * @retval ��
  */
void Put_Box(u16 AXIS_X_Mm,u16 AXIS_Z_Mm)//����
{
	if(Action.Put_Box_Start_Flag == 1)
	{
		if(Action.Put_Box_Stage == 1)//XZ�˶��׶Σ��ص��мܲ�
		{
			if(Action.tray_position == Keep)//��������Ϊ��
			{
				Action.Put_Box_Stage = 3;
			}
			else//����������
			{
				if(Axis_Y.InCtrl_Flag == 0&&(A_SENSOR ==1||B_SENSOR == 1))
				{
					Beep(50,50,5);
					LED_RED = ON;//��ɫ�Ʊ���
					LED_GRE = OFF;
					LED_YEL = OFF;
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Error,0);//���ر�����Ϣ
					Action.Put_Box_Stage = 7;
					return;
				}
				LED_RED = OFF;
				LED_GRE = ON;//������ɫ�Ʋ�����
				LED_YEL = OFF;
				XZ_Run(Action.PlatX_Axis,Action.PlatZ_Axis-AxisZ_Float_Disance);
			}
			if(Action.end_flag == 1)
			{
				if(Action.PutBox_Cnt>3)
				{
					Action.PutBox_Cnt = 0;
					Action.PutBox_Status_Change = 1;
					Action.Put_Box_Stage = 6;//����͵͵��λ״̬����û��֪��
					return;
				}
				Privately_Back:
				Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Second_Stage,0);//XZ�˶��������ر���
				
				Action.Put_Box_Stage++;
				delay_ms(ActionDelay);				
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
			}
		}
		else if(Action.Put_Box_Stage == 2)//����ȡ��׶�
		{
			Tray_Get_Box(!(Action.tray_position>>1),AxisZ_UpDownMore_Pluse,Tray);
			if(Action.end_flag == 1)
			{
				/**************���ڲ��Դ���************/
						if(Axis_Y.InCtrl_Flag == 0&&(A_SENSOR ==1||B_SENSOR == 1))
						{
							Beep(50,50,5);
							LED_RED = ON;//��ɫ�Ʊ���
							LED_GRE = OFF;
							LED_YEL = OFF;
							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Error,0);//���ر�����Ϣ
							Action.Put_Box_Stage = 7;
							return;
						}
						if(Length_High_LimCtl())//������λ
						{
							Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,WID_LENALM,0);//���س�����λ������Ϣ
							Beep(50,50,5);
							LED_RED = ON;//��ɫ�Ʊ���
							LED_GRE = OFF;
							LED_YEL = OFF;
							Action.Put_Box_Stage = 5;
							goto status5;
						}
				/**************���ڲ��Դ���************/
				Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Third_Stage,0);//����ȡ����������ر���

						
				Action.Put_Box_Stage++;
				delay_ms(ActionDelay);						
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);

			}
		}
		else if(Action.Put_Box_Stage == 3)//XZ�˶��׶Σ�ǰ�����ܲ�����
		{
			XZ_Run(AXIS_X_Mm,AXIS_Z_Mm);
			if(Action.end_flag == 1)
			{
				if(Action.tray_position == Left_Tray||Action.tray_position == Right_Tray)
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Fourth_Stage,0);//XZ�˶����������ر���

				}
				else
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Second_Stage,0);//XZ�˶����������ر���

				}
				
				Action.Put_Box_Stage++;
				delay_ms(ActionDelay);				
				delay_ms(ActionDelay);
					delay_ms(ActionDelay);					
			}
		}
		else if(Action.Put_Box_Stage == 4)//���̷���׶�
		{

			Tray_Put_Box(!(Action.command&0x01),AxisZ_UpDown_Pluse,Goods);
			if(Action.end_flag == 1)
			{
				
				if(Axis_Y.InCtrl_Flag == 0&&(A_SENSOR ==1||B_SENSOR == 1))//����δ������λ������
				{
					Beep(50,50,5);
					LED_RED = ON;//��ɫ�Ʊ���
					LED_GRE = OFF;
					LED_YEL = OFF;
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Error,0);//���ر�����Ϣ
					Action.Put_Box_Stage = 7;
					return;
				}
						
				if(Action.tray_position == Left_Tray||Action.tray_position == Right_Tray)//����������
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Fifth_Stage,0);//���̷�����������ر���
					Action_Stop();
				}
				else//����������
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Third_Stage,0);//���̷�����������ر���
				}
				Action.Put_Box_Stage+=3;
//				delay_ms(ActionDelay);				
//				delay_ms(ActionDelay);
//				delay_ms(ActionDelay);
			}
		}
		else if(Action.Put_Box_Stage == 5)//�����������߿���λ��������ת������׶Σ�����������䲻����˽׶�
		{
			status5:
			Tray_Put_Box(!(Action.tray_position>>1),AxisZ_UpDownMore_Pluse,Goods);
			if(Action.end_flag == 1)
			{
				Action.Put_Box_Stage+=2;
				delay_ms(ActionDelay);
				
				delay_ms(ActionDelay);
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
					delay_ms(ActionDelay);
			}
		}
		else if(Action.Put_Box_Stage == 6)//͵͵��λ״̬��
		{
			if(Action.PutBox_Status_Change ==1)
			{
				Action.PutBox_Status_Change = 0;
				DIR_X = X_DIR_BACK;
				Axis_X.Dir = X_DIR_BACK;
				Axis_X.NowPulse = 0;
				Axis_X.Target_Pulse = 50000;
				Axis_X.InCtrl_Flag = 3;//��λģʽ
			}
			if(XYZ_To_Zero.X_Return_Flag == 1)
			{
				XYZ_To_Zero.X_Return_Flag = 0;
				Action.Put_Box_Stage = 1;
				goto Privately_Back;
			}
		}
		else//���ж����������ñ�־λ
		{
			Action_Stop();
			Action.Put_Box_Start_Flag = 0;
			Action.Put_Box_Stage = 1;
			Action.end_flag = 0;
			Action.Target_X = 0;
			Action.Target_Z = 0;
		}
	}
}

void XZ_Run(u16 AXIS_X_Mm,u16 AXIS_Z_Mm)//XZ�˶�
{
	if(Action.XZRun_Stage == 1)//XZ�˶��׶�
	{
		if(Action.XZRUN_Status_Change == 1)
		{
			Action.end_flag = 0;
			Action.XZRUN_Status_Change = 0;
			Motor_CtrlOver_Flag = 1;
			Motor_MoveToPosition(AXIS_X_Mm,AXIS_Z_Mm);
			
		}
		if(Motor_CtrlOver_Flag == 2)
		{
			Action.XZRun_Stage++;
		}
	}
	else//�˶�������λ��־λ
	{
		Action.end_flag = 1;
		Action.XZRun_Stage = 1;
		Action.XZRUN_Status_Change = 1;	
	}
}

/**
  * @brief  ���̷��䶯�������������̣�Z���½�����������������
  * @param  ������չ����(�����)��Z���½�������,���̵�ǰλ��
  * @retval ��
  */
void Tray_Put_Box(bool Axis_Y_Dir,u32 UpDown_Pluse,bool Tray_Position)//��е�ַ���
{
		if(Action.Tray_PutBox_Stage == 1)//������
		{
			if(Action.Tray_PutBox_Status_Change == 1)
			{
				Action.end_flag = 0;
				Action.Tray_PutBox_Status_Change = 0;
				if(Tray_Position == Tray)//��ǰ����λ���мܴ�
					{
						if((Action.tray_position == Left_Tray&&Action.command == Get_Left_Box)||
							 (Action.tray_position == Right_Tray&&Action.command == Get_Right_Box))//�����������������һ�£�������
								{
									if(Axis_Y_Dir == Y_DIR_LEFT)//����
									{
										if(Last_AxisY_Dir==Axis_Y_Dir)
										{
											Last_AxisY_Dir = Axis_Y_Dir;//��¼���������˶�����
											StepMotorCtrl_Pulse(Y_LeftLim_LessPluse,AXIS_Y,Axis_Y_Dir);//�궨��
										}
										else
										{
											Last_AxisY_Dir = Axis_Y_Dir;//��¼���������˶�����
											StepMotorCtrl_Pulse(Y_LeftLim_MorePluse,AXIS_Y,Axis_Y_Dir);//�궨��
										}
									}
										else//����
									{
										if(Last_AxisY_Dir==Axis_Y_Dir)
										{
											Last_AxisY_Dir = Axis_Y_Dir;//��¼���������˶�����
											StepMotorCtrl_Pulse(Y_RightLim_LessPluse,AXIS_Y,Axis_Y_Dir);//�궨��
										}
										else
										{
											Last_AxisY_Dir = Axis_Y_Dir;//��¼���������˶�����
											StepMotorCtrl_Pulse(Y_RightLim_MorePluse,AXIS_Y,Axis_Y_Dir);//�궨��
										}
									}
								}
								else//����������������Բ�һ�£�����������
								{
									if(Action.tray_position == Left_Tray&&Action.command == Get_Right_Box)//ȡ��������ұ�ȡ�䣬����мܻ�
									{
										if(Last_AxisY_Dir == Axis_Y_Dir)
										{
											Last_AxisY_Dir=Axis_Y_Dir;
											StepMotorCtrl_Pulse((Y_LeftLim_LessPluse+50),AXIS_Y,Axis_Y_Dir);	
										}								
										else
										{
											Last_AxisY_Dir=Axis_Y_Dir;
											StepMotorCtrl_Pulse((Y_LeftLim_MorePluse+50),AXIS_Y,Axis_Y_Dir);	
										}
									}
									else//ȡ����������ȡ�䣬�ұ��мܻ�
									{
										if(Last_AxisY_Dir == Axis_Y_Dir)
										{
											Last_AxisY_Dir=Axis_Y_Dir;
											StepMotorCtrl_Pulse((Y_RightLim_LessPluse+50),AXIS_Y,Axis_Y_Dir);	
										}								
										else
										{
											Last_AxisY_Dir=Axis_Y_Dir;
											StepMotorCtrl_Pulse((Y_RightLim_MorePluse+50),AXIS_Y,Axis_Y_Dir);	
										}
									}
								}
					}
				else//��ǰ����λ�ڻ��ܴ�
				{
					if(Axis_Y_Dir == Y_DIR_LEFT)//����
					{
						if(Last_AxisY_Dir==Axis_Y_Dir)
						{
							Last_AxisY_Dir = Axis_Y_Dir;//��¼���������˶�����
							StepMotorCtrl_Pulse(Y_LeftLim_LessPluse,AXIS_Y,Axis_Y_Dir);//�궨��
						}
						else
						{
							Last_AxisY_Dir = Axis_Y_Dir;//��¼���������˶�����
							StepMotorCtrl_Pulse(Y_LeftLim_MorePluse,AXIS_Y,Axis_Y_Dir);//�궨��
						}
					}
					else//����
					{
						if(Last_AxisY_Dir==Axis_Y_Dir)
						{
							Last_AxisY_Dir = Axis_Y_Dir;//��¼���������˶�����
							StepMotorCtrl_Pulse(Y_RightLim_LessPluse,AXIS_Y,Axis_Y_Dir);//�궨��
						}
						else
						{
							Last_AxisY_Dir = Axis_Y_Dir;//��¼���������˶�����
							StepMotorCtrl_Pulse(Y_RightLim_MorePluse,AXIS_Y,Axis_Y_Dir);//�궨��
						}
					}
				}
			}
			if(Axis_Y.InCtrl_Flag == 0)
			{
				Action.Tray_PutBox_Status_Change = 1;
				Action.Tray_PutBox_Stage++;
				delay_ms(ActionDelay);
			}
		}
		else if(Action.Tray_PutBox_Stage == 2)//Z���½�
		{
			if(Action.Tray_PutBox_Status_Change == 1)
			{
				Action.Tray_PutBox_Status_Change = 0;
				StepMotorCtrl_Pulse(UpDown_Pluse,AXIS_Z,Z_DIR_BACK);
			}
			if(Axis_Z.InCtrl_Flag == 0)//�˶�����
			{
				Action.Tray_PutBox_Status_Change = 1;
				Action.Tray_PutBox_Stage++;
				delay_ms(ActionDelay);
			}
		}
		else if(Action.Tray_PutBox_Stage == 3)//������
		{
			if(Action.Tray_PutBox_Status_Change == 1)
			{
				Action.Tray_PutBox_Status_Change = 0;
				Last_AxisY_Dir = !Axis_Y_Dir;
				StepMotorCtrl_Pulse(2500,AXIS_Y,!Axis_Y_Dir);
			}
				if(Axis_Y.InCtrl_Flag == 0)//����λ����λ��ת����һ�׶�
				{				
					Action.Tray_PutBox_Stage++;
				}
		}
		else//�����������ñ�־λ
		{
			Action.Tray_PutBox_Status_Change = 1;
			Action.Tray_PutBox_Stage = 1;
			Action.end_flag=1;
		}
	}

	
/**
  * @brief  ����ȡ�䶯�������������̣�Z���½�����������������
  * @param  ������չ����(�����)��Z���½�������,���̵�ǰλ��
  * @retval ��
  */
void Tray_Get_Box(bool Axis_Y_Dir,u32 UpDown_Pluse,bool Tray_Position)//��е������
{
		if(Action.Tray_GetBox_Stage == 1)//������
		{
			if(Action.Tray_GetBox_Status_Change)
			{
				Action.end_flag = 0;
				Action.Tray_GetBox_Status_Change = 0;
				if(Tray_Position == Tray)//��ǰ���̴����мܴ�
				{
					if((Action.tray_position == Left_Tray&&Action.command == Return_Left_Box)||
						(Action.tray_position == Right_Tray&&Action.command == Return_Right_Box))//�����������������һ�£�������
					{
						if(Axis_Y_Dir == Y_DIR_LEFT)//������
						{
							if(Last_AxisY_Dir==Axis_Y_Dir)//�������ϴ��ƶ���ͬ�ķ���
							{
								Last_AxisY_Dir = Axis_Y_Dir;//��¼���������˶�����
								StepMotorCtrl_Pulse(Y_LeftLim_LessPluse,AXIS_Y,Axis_Y_Dir);
							}
							else//�������ϴ��ƶ���ͬ�ķ���
							{
								Last_AxisY_Dir = Axis_Y_Dir;//��¼���������˶�����
								StepMotorCtrl_Pulse(Y_LeftLim_MorePluse,AXIS_Y,Axis_Y_Dir);
							}
						}
						else//������
						{
							if(Last_AxisY_Dir==Axis_Y_Dir)//���ϴ�������ͬ�ķ���
							{
								Last_AxisY_Dir=Axis_Y_Dir;
								StepMotorCtrl_Pulse(Y_RightLim_LessPluse+30,AXIS_Y,Axis_Y_Dir);
							}
							else//���ϴ����߲�ͬ�ķ���
							{
								Last_AxisY_Dir=Axis_Y_Dir;
								StepMotorCtrl_Pulse(Y_RightLim_MorePluse+30,AXIS_Y,Axis_Y_Dir);
							}
						}
					}
					else//����������������Է���һ�£�����������
					{
							if(Action.tray_position == Left_Tray&&Action.command == Return_Right_Box)//�������������м�ȡ�䣬�ұ߻��ܷ���
							{									
								if(Last_AxisY_Dir == Axis_Y_Dir)
								{
									Last_AxisY_Dir=Axis_Y_Dir;
									StepMotorCtrl_Pulse((Y_LeftLim_LessPluse+50),AXIS_Y,Axis_Y_Dir);	
								}								
								else
								{
									Last_AxisY_Dir=Axis_Y_Dir;
									StepMotorCtrl_Pulse((Y_LeftLim_MorePluse+50),AXIS_Y,Axis_Y_Dir);	
								}
							}
							else//����������ұ��м�ȡ�䣬��߻��ܷ���
							{
								if(Last_AxisY_Dir == Axis_Y_Dir)
								{
									Last_AxisY_Dir=Axis_Y_Dir;
									StepMotorCtrl_Pulse((Y_RightLim_LessPluse+50),AXIS_Y,Axis_Y_Dir);	
								}								
								else
								{
									Last_AxisY_Dir=Axis_Y_Dir;
									StepMotorCtrl_Pulse((Y_RightLim_MorePluse+50),AXIS_Y,Axis_Y_Dir);	
								}
							}
					}
				}
				else//��ǰ���̴��ڻ���
				{
					if(Axis_Y_Dir == Y_DIR_LEFT)//������
					{
						if(Last_AxisY_Dir==Axis_Y_Dir)//�������ϴ��ƶ���ͬ�ķ���
						{
							Last_AxisY_Dir = Axis_Y_Dir;//��¼���������˶�����
							StepMotorCtrl_Pulse(Y_LeftLim_LessPluse,AXIS_Y,Axis_Y_Dir);
						}
						else//�������ϴ��ƶ���ͬ�ķ���
						{
							Last_AxisY_Dir = Axis_Y_Dir;//��¼���������˶�����
							StepMotorCtrl_Pulse(Y_LeftLim_MorePluse,AXIS_Y,Axis_Y_Dir);
						}
					}
					else//������
					{
						if(Last_AxisY_Dir==Axis_Y_Dir)//���ϴ�������ͬ�ķ���
						{
							Last_AxisY_Dir=Axis_Y_Dir;
							StepMotorCtrl_Pulse(Y_RightLim_LessPluse,AXIS_Y,Axis_Y_Dir);
						}
						else//���ϴ����߲�ͬ�ķ���
						{
							Last_AxisY_Dir=Axis_Y_Dir;
							StepMotorCtrl_Pulse(Y_RightLim_MorePluse,AXIS_Y,Axis_Y_Dir);
						}
					}
				}
			}
			if(Axis_Y.InCtrl_Flag == 0)//Y�ᶯ���������׶���ת
			{
				Action.Tray_GetBox_Stage++;
				Action.Tray_GetBox_Status_Change = 1;
				delay_ms(ActionDelay);
			}
		}
		else if(Action.Tray_GetBox_Stage == 2)//Z������
		{
			if(Action.Tray_GetBox_Status_Change)
			{
				Action.Tray_GetBox_Status_Change = 0;
				StepMotorCtrl_Pulse(UpDown_Pluse,AXIS_Z,Z_DIR_FRONT);
			}
			if(Axis_Z.InCtrl_Flag == 0)
			{
				Action.Tray_GetBox_Stage++;
				Action.Tray_GetBox_Status_Change = 1;
				delay_ms(ActionDelay);
			}
		}
		else if(Action.Tray_GetBox_Stage == 3)//�����̼��ٽ׶�
		{
			if(Action.Tray_GetBox_Status_Change)
			{
				Last_AxisY_Dir = !Axis_Y_Dir;//��¼���������˶�����
				StepMotorCtrl_Pulse(1600,AXIS_Y,!Axis_Y_Dir);
				Action.Tray_GetBox_Status_Change = 0;
			}
			if(Axis_Y.InCtrl_Flag == 0)
			{
				Action.Tray_GetBox_Status_Change = 1;
				Action.Tray_GetBox_Stage++;
			}
		}
		else if(Action.Tray_GetBox_Stage == 4)//���������ٽ׶Σ���ֹ����̫����λ�ز�׼
		{
			if(Action.Tray_GetBox_Status_Change)
			{
				DIR_Y = !Axis_Y_Dir;
				Axis_Y.Dir = !Axis_Y_Dir;
				Axis_Y.Target_Pulse = 1000;
				Axis_Y.NowPulse = 0;
				Axis_Y.InCtrl_Flag = 3;//��λģʽ
				Action.Tray_GetBox_Status_Change = 0;
			}
			if(Axis_Y.InCtrl_Flag == 0)
			{
				Action.Tray_GetBox_Status_Change = 1;
				Action.Tray_GetBox_Stage++;
			}
			
		}
		else//������������λ��־λ
		{						
			Action.Tray_GetBox_Status_Change = 1;
			Action.Tray_GetBox_Stage = 1;
			Action.end_flag = 1;
			delay_ms(ActionDelay);
			delay_ms(ActionDelay);
		}
}

/**
  * @brief  ����ʦģʽ�������
  * @param  Ŀ������ᣬ����
  * @retval ��
  */
void Axis_Move_Alone(u8 Axis ,bool Dir)//����ʦģʽ�������
{
	switch (Axis)
	{
		case AXIS_X:
		{
			if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1||A_SENSOR ==1||B_SENSOR == 1)//����ǰ�ж������Ƿ�λ����λ������ִ�ж���
				{
					Beep(50,50,1);
					return ;
				}
				Axis_X.Dir = Dir;
				DIR_X = Axis_X.Dir;
				Axis_X.NowPulse = 0;
				Axis_X.InCtrl_Flag = 1;
		}break;
		case AXIS_Y: 
		{
			Axis_Y.Dir = Dir;
			DIR_Y = Axis_Y.Dir;
			Axis_Y.NowPulse = 0;
			Axis_Y.InCtrl_Flag = 1;
		}break;
		case AXIS_Z:
		{
			if((Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1)||
					(Dir == Z_DIR_FRONT&&Axis_Z_Up_Sensor ==0)||A_SENSOR ==1||B_SENSOR == 1)//����ǰ�ж������Ƿ�λ����λ������ִ�ж���
				{
					Beep(50,50,1);
					return ;
				}
			Axis_Z.Dir = Dir;
			DIR_Z = Axis_Z.Dir;
			Axis_Z.NowPulse = 0;
			Axis_Z.InCtrl_Flag = 1;
		}break;
	}
}

/**
  * @brief  ���⸴λ״̬��
  * @param  ����Z������;Z���½���X�����;Y����λ;Y���Ҹ�λ.�������
  * @retval ��
  */
void XYZ_BackZero()
{
	if(XYZ_To_Zero.Start_Flag == 1)
	{
		if(XYZ_To_Zero.XYZ_Ret_Stage == 1)//�Զ���λ
		{
			if(XYZ_To_Zero.XYZRET_Status_Change == 0)
		{
				XYZ_To_Zero.XYZRET_Status_Change =1;
			if(A_SENSOR == 1&&B_SENSOR == 0)//����λ���ұߣ�����λ
			{
				DIR_Y = Y_DIR_LEFT;
				Axis_Y.Dir = Y_DIR_LEFT;
				Axis_Y.Target_Pulse = 2500;
				Axis_Y.NowPulse = 0;
				Axis_Y.InCtrl_Flag = 3;//��λģʽ
				XYZ_To_Zero.Tray_Reset_Flag = 1;
			}
			else if(B_SENSOR == 1&&A_SENSOR == 0)//����λ����ߣ����Ҹ�λ
			{				
					DIR_Y = Y_DIR_RIGHT;
					Axis_Y.Dir = Y_DIR_RIGHT;
					Axis_Y.Target_Pulse = 2500;
					Axis_Y.NowPulse = 0;
					Axis_Y.InCtrl_Flag = 3;//��λģʽ
					XYZ_To_Zero.Tray_Reset_Flag = 1;
			}
			else//����λ�����м�
			{
				
			}
		}
			if(Axis_Y.InCtrl_Flag == 0)
			{
				XYZ_To_Zero.XYZ_Ret_Stage++;
				XYZ_To_Zero.XYZRET_Status_Change = 0;
				delay_ms(500);
			}
		}
		else if(XYZ_To_Zero.XYZ_Ret_Stage == 2)//Z�Ḵλ����
		{
			if(XYZ_To_Zero.XYZRET_Status_Change == 0)
			{
				if(A_SENSOR == 1||B_SENSOR == 1)//����ǰ�ж������Ƿ�λ����λ������ִ�ж���
				{
					Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,0xff,0);//��ǰ����δ������λ��������Ϣ
					return ;
				}
				if((Axis_Z_Up_Sensor == 0)||(Axis_Z_Up_Sensor == 1&&Axis_Z_Down_Sensor == 1))//�Ѿ����ڶ��������¹�λ
				{
					XYZ_To_Zero.XYZ_Ret_Stage = 3;
					XYZ_To_Zero.XYZRET_Status_Change = 0;
					return ;
				}
				if(Axis_Z_Down_Sensor == 0)//���̴�������λλ�ã������Ϲ�λ������
				{
					XYZ_To_Zero.XYZRET_Status_Change = 1;
					DIR_Z = Z_DIR_FRONT;
					Axis_Z.Dir = Z_DIR_FRONT;
					Axis_Z.Target_Pulse = Z_ZeroUp_Pluse;
					Axis_Z.NowPulse = 0;
					Axis_Z.InCtrl_Flag = 3;//��λģʽ
				}
			}
			if(Axis_Z.InCtrl_Flag == 0)
			{
				XYZ_To_Zero.XYZ_Ret_Stage++;
				XYZ_To_Zero.XYZRET_Status_Change = 0;
				delay_ms(500);
			}
		}
		else if(XYZ_To_Zero.XYZ_Ret_Stage == 3)//Z���½���X��������
		{
			if(XYZ_To_Zero.XYZRET_Status_Change == 0)
			{
				XYZ_To_Zero.XYZRET_Status_Change = 1;
				/*************Z���½���λ********************/
				DIR_Z = Z_DIR_BACK;
				Axis_Z.Dir = Z_DIR_BACK;
				Axis_Z.NowPulse = 0;
				Axis_Z.Target_Pulse = 50000;
				Axis_Z.InCtrl_Flag = 3;//��λģʽ
				/*************X���λ***********************/
				DIR_X = X_DIR_BACK;
				Axis_X.Dir = X_DIR_BACK;
				Axis_X.NowPulse = 0;
				Axis_X.Target_Pulse = 50000;
				Axis_X.InCtrl_Flag = 3;//��λģʽ
			}
			if(Axis_Z.InCtrl_Flag == 0&&Axis_X.InCtrl_Flag == 0&&XYZ_To_Zero.X_Return_Flag == 1)
			{
				XYZ_To_Zero.X_Return_Flag = 0;
				XYZ_To_Zero.XYZ_Ret_Stage++;
				XYZ_To_Zero.XYZRET_Status_Change = 0;
				delay_ms(500);
			}
		}
		else if(XYZ_To_Zero.XYZ_Ret_Stage == 4)//Y����λ���̣��������̻�е���
		{
			if(XYZ_To_Zero.XYZRET_Status_Change == 0)
			{
				XYZ_To_Zero.XYZRET_Status_Change =1;
				DIR_Y = Y_DIR_LEFT;
				Axis_Y.Dir = Y_DIR_LEFT;
				Axis_Y.Target_Pulse = 500;
				Axis_Y.NowPulse = 0;
				Axis_Y.InCtrl_Flag = 3;//��λģʽ
			}
			if(Axis_Y.InCtrl_Flag == 0)
			{
				XYZ_To_Zero.XYZ_Ret_Stage++;
				XYZ_To_Zero.XYZRET_Status_Change = 0;
				delay_ms(500);
			}
		}
		else if(XYZ_To_Zero.XYZ_Ret_Stage == 5)//Y���Ҹ�λ���̣��������̻�е���
		{
			if(XYZ_To_Zero.XYZRET_Status_Change == 0)
			{
				XYZ_To_Zero.XYZRET_Status_Change =1;
				DIR_Y = Y_DIR_RIGHT;
				Axis_Y.Dir = Y_DIR_RIGHT;
				Axis_Y.Target_Pulse = 1000;
				Axis_Y.NowPulse = 0;
				Axis_Y.InCtrl_Flag = 3;//��λģʽ
			}
			if(Axis_Y.InCtrl_Flag == 0)
			{
				Last_AxisY_Dir = Y_DIR_RIGHT;//��¼�����˶�����
				XYZ_To_Zero.XYZ_Ret_Stage++;
				XYZ_To_Zero.XYZRET_Status_Change = 0;
				delay_ms(500);
			}
		}
		else//���̽�����λ
		{
			XYZ_To_Zero.Over_Flag = 1;
			XYZ_To_Zero.XYZ_Ret_Stage = 1;
			XYZ_To_Zero.XYZRET_Status_Change = 0;
			XYZ_To_Zero.Start_Flag = 0;
			Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Fifth_Stage,0);//XZ�˶��������������ر���
			Beep(50,50,1);
		}
	}
}

/**
  * @brief  ȡ�䣬���䣬�������
  * @param  1:����ָ��ָ��ȡ��\����\���2:�������ԣ�3:�������ԣ�4:X�����꣬5:Z������
  * @retval ��
  */
void Get_PutBox_Analysis(pick_one open,u8 tray_position,u8 command_data,u32 x_run, u32 Z_Run)
{
	open(tray_position,command_data,x_run,Z_Run);//ѡ�������ĺ���
}
/**
  * @brief  �������
  * @param  1:�������ԣ�2:�������ԣ�3:X�����꣬4:Z������
  * @retval ��
  */
void Arrive_Analysis(u8 tray_position,u8 command_data,u32 x_run, u32 Z_Run)
{
	if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1||A_SENSOR == 1||B_SENSOR == 1)//����ǰ�ж������Ƿ�λ����λ������ִ�ж���
	{
		return ;
	}
	Action.Comend  = CMD_CTRL_XZ;//ָ�
	Action.command = Arrive;//ת��λָ��
	Action.Target_X = x_run;//Ŀ��X������
	Action.Target_Z = Z_Run;//Ŀ��
	Action.tray_position = Keep;//��������
	Action.PlatX_Axis = 0;//�м�X������
	Action.PlatZ_Axis = 240;//�м�Z������
	Motor_CtrlOver_Flag = 1;
	Action.Get_Box_Start_Flag = 1;
	Action.Get_Box_Stage = 1;
	Action.end_flag = 0;
	Action.XZRun_Stage = 1;
	Action.XZRUN_Status_Change = 1;
}
/**
  * @brief  �������
  * @param  1:�������ԣ�2:�������ԣ�3:X�����꣬4:Z������
  * @retval ��
  */
void Put_Box_Analysis(u8 tray_position,u8 command_data,u32 x_run, u32 Z_Run)
{
if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1||A_SENSOR == 1||B_SENSOR == 1)//����ǰ�ж������Ƿ�λ����λ������ִ�ж���
	{
		return ;
	}
	Action.Comend  = CMD_CTRL_XZ;//ָ�
	Action.command = command_data;//ת��λָ��
	Action.Target_X = x_run;//Ŀ��X������
	Action.Target_Z = Z_Run+AxisZ_Float_Disance;//Ŀ��
	Action.tray_position = tray_position;//��������
	Action.PlatX_Axis = Plat_Axis_X;//�м�X������
	Action.PlatZ_Axis = Plat_Axis_Z;//�м�Z������
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
}

/**
  * @brief  ȡ�����
  * @param  1:�������ԣ�2:�������ԣ�3:X�����꣬4:Z������
  * @retval ��
  */
void Get_Box_Analysis(u8 tray_position,u8 command_data,u32 x_run, u32 Z_Run)
{
if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1||A_SENSOR == 1||B_SENSOR == 1)//����ǰ�ж������Ƿ�λ����λ������ִ�ж���
	{
		return ;
	}
	Action.Comend  = CMD_CTRL_XZ;//ָ�
	Action.command = command_data;//ת��λָ��
	Action.Target_X = x_run;//Ŀ��X������
	Action.Target_Z = Z_Run-AxisZ_Float_Disance;//Ŀ��
	Action.tray_position = tray_position;//��������
	Action.PlatX_Axis = Plat_Axis_X;//�м�X������
	Action.PlatZ_Axis = Plat_Axis_Z;//�м�Z������
	Motor_CtrlOver_Flag = 1;
	Action.Get_Box_Start_Flag = 1;
	Action.Get_Box_Stage = 1;
	Action.XZRun_Stage = 1;
	Action.XZRUN_Status_Change = 1;
	Action.Tray_PutBox_Stage = 1;
	Action.Tray_PutBox_Status_Change = 1;
	Action.Tray_GetBox_Stage = 1;
	Action.Tray_GetBox_Status_Change = 1;
	Action.end_flag = 0;
}


/************************************************
* �������ƣ�void Action_Stop()
* ����������ֹͣ���й���
* ���ؽ������
*************************************************/
void Action_Stop(void)
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

}


/**
  * @brief  ������λ�ж�
  * @param  
* @retval 1:������λ������������0 :�޴���
  */
bool Length_High_LimCtl(void)
{
	if((A_HIN+A_LIN+B_LIN+B_HIN)>0)
	{
		LED_RED = ON;
		LED_GRE = OFF;
		LED_YEL = OFF;
		return 1;
	}
//	if(AB_HIGHTLENGTH_SENSOR == 0)
//	{
//		
//	}
	else
	{
		return 0;
	}
}

//void asd()
//{
//	bool ydir=0;
//	ydir = A_SENSOR ==0?(B_SENSOR ==1?1:0):(B_SENSOR ==0?0:1);
//}
void Auto_Rest()
{
	XYZ_To_Zero.Start_Flag = 1;
	XYZ_To_Zero.Over_Flag = 0;
}
char Axis_X_Demarcate(u8 X_X,u8 Z_Z,u8 Tray_Position)//������������ѹ��д�����ģ�һ���ֶ��궨����
{
		switch (Tray_Position)//��������
		{
			case Left_Tray://��
			{
				switch (X_X)
				{
					case 0:
					{
						switch (Z_Z)
						{
							case 0:
							{
								
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 1:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 2:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 3:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 4:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 5:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 6:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 7:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 8:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 9:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 10:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 11:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					default:
					{
					}break;
				}
			}break;
			case Right_Tray://��
			{
			switch (X_X)
				{
					case 0:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 1:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 2:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 3:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 4:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 5:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 6:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 7:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 8:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 9:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 10:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					case 11:
					{
						switch (Z_Z)
						{
							case 0:
							{
							}break;
							case 1:
							{
							}break;
							case 2:
							{
							}break;
							case 3:
							{
							}break;
							case 4:
							{
							}break;
							case 5:
							{
							}break;
							case 6:
							{
							}break;
							case 7:
							{
							}break;
							case 8:
							{
							}break;
							case 9:
							{
							}break;
						}
					}break;
					default:
					{
					}break;
				}
			}break;
			
		}
}
