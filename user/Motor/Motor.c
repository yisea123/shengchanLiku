#include "Motor.h"
#include <stdbool.h>
#include "led.h"
#include "Sensor.h"
#include "can.h"																

/*************��������*************/
u32 Addnum=0;								//�����������(�����ۼ�)
u32 Pulplus=0;

bool Pipe_Sen_old=1;        //�ܵ��ڴ�������һʱ��״̬�洢

u16 BadBottle_Reject_timer; //����ƿ�޳���ʱʱ��
bool First_Bootlecaptest; //ƿ�ǲ���
/*********************************/

void Motor_Init(void)		//����վ�����ʼ��
{	
	RCC->APB2ENR|=1<<3;			//ʹ��PORTBʱ��
	RCC->APB2ENR|=1<<4;			//ʹ��PORTCʱ��
	RCC->APB2ENR|=1<<6;			//ʹ��PORTEʱ��
	
	//PB �������
	GPIOB->CRH&=0X000FFFFF;	//PB13~15	������� �ٶ�50MHz
	GPIOB->CRH|=0X33300000;
	
	//PE	���������ٵ�� L6203
	GPIOE->CRH&=0X00F0FFFF;	//PE12,PE14,PE15	������� �ٶ�2MHz
	GPIOE->CRH|=0X22020000;
	
	//PC ������� 
	GPIOC->CRH&=0XFFF0FFFF;	//PC12 ��������
	GPIOC->CRH|=0X00080000;

}

/*********************************************
*���ƣ�42�����������
*���룺 ���ת������/������   
       ����λ 1 ����λ  0 ��ǰ�� ������λʱ����⵽������ʱ����ǰ��1cm 4000step��
*�����0�ɹ� 1ʧ�� 2��������
**********************************************/
unsigned char StepperMoto_42(unsigned long StepNum,unsigned char Derict)
{
	//���ٲ���  Ϊ5000�� 
	unsigned long i;
	unsigned long SpeedDelay;
	unsigned char FirstSensorFlag;          //�״μ�⵽�����λ��������־
	unsigned char FirstCapSensorFlag;       //�״μ�⵽ƿ�Ǵ�������־
	unsigned char FirstDownSensorFlag;      //�״μ�⵽�¹�λ��������־
	u16 MotoGobackPul;    									//�ص���λ��Ҫ�ߵ�������
	/*********��������*********/
	//��Ϊ��������վ����㲻ͬ��Ϊ�Ϲ�λ�����¹�λ��������Ҫ���÷���
	if(Derict == DERICT_FRONT)    //
	{
		SM_DIRH;	
		if(Device_Prop.DEVICE_TYPE == _AES16)
		{
			SM_DIRL;		
		}
		else if(Device_Prop.DEVICE_TYPE == _AES16D)
		{
			SM_DIRH;		
			//AES16D���¹�λ���д�����������Ѿ����¹�λ������Ϊ���£���ֱ���˳�����ֹ����
			if(Sensor_MotorDown == 0)
			{
				return 0;
			}
		}
		else if(Device_Prop.DEVICE_TYPE == _ADS16)
		{
			SM_DIRH;				
		}
		//AES16F����վ��Ҫ����ɲ��
		else if(Device_Prop.DEVICE_TYPE == _AES16F)
		{
			delay_ms(100);
			SM_DIRL;
		}
	}
	else if(Derict == DERICT_BACK)  //����λ
	{
		SM_DIRL;
		MotoGobackPul = MotoGobackPul_NORMAL;
		if(Device_Prop.DEVICE_TYPE == _AES16)
		{
			SM_DIRH;		
			MotoGobackPul = MotoGobackPul_NORMAL;
		}
		else if(Device_Prop.DEVICE_TYPE == _AES16D)
		{
			SM_DIRL;	
			MotoGobackPul = MotoGobackPul_NORMAL;
		}
		else if(Device_Prop.DEVICE_TYPE == _ADS16)
		{
			SM_DIRL;				
			MotoGobackPul = MotoGobackPul_NORMAL;
		}
		//AES16F����վ��Ҫ����ɲ��
		else if(Device_Prop.DEVICE_TYPE == _AES16F)
		{
			delay_ms(100);
			SM_DIRH;
			MotoGobackPul = MotoGobackPul_AES16F;  //���û��������ߵĲ���
		}
	}
	/**************************/
	FirstSensorFlag=0;
	for(i=0;i<StepNum;i++)
	{
		//����������������ǰ���������㵱ǰ��ʱ��
		SpeedDelay = Motor_SpeedCtrl(StepNum,i);
		
		/***********������**********/
		SM_PULH;
		delay_us(2);
		SM_PULL;
		delay_us(SpeedDelay);	
		/******��������*****/
		if(Derict == DERICT_FRONT)     //����
		{
			Addnum++;
		}
		else if(Derict == DERICT_BACK) //�½�
		{
			if(Addnum>0)Addnum--;
		}
		/**********�����**********/
		if(Derict == DERICT_BACK) //����Ϊ����㷽��ʱ�����������
		{
			if(Sensor_MotorZero==0 && FirstSensorFlag==0)FirstSensorFlag=1;//��⵽0λ��		
			else if(Sensor_MotorZero==0 && FirstSensorFlag==1)FirstSensorFlag=2;//��⵽0λ��		
			else if(Sensor_MotorZero==0 && FirstSensorFlag==2)FirstSensorFlag=3;//��⵽0λ��	
			else if(Sensor_MotorZero==0 && FirstSensorFlag==3)FirstSensorFlag=4;//��⵽0λ��	
			else if(Sensor_MotorZero==0 && FirstSensorFlag==4)FirstSensorFlag=5;//��⵽0λ��	
			else if(Sensor_MotorZero==0 && FirstSensorFlag==5)FirstSensorFlag=6;//��⵽0λ��	
			else if(Sensor_MotorZero==0 && FirstSensorFlag==6)FirstSensorFlag=7;//��⵽0λ��	
			else if(Sensor_MotorZero==0 && FirstSensorFlag==7)FirstSensorFlag=8;//��⵽0λ��		
			else if(Sensor_MotorZero==0 && FirstSensorFlag==8)FirstSensorFlag=9;//��⵽0λ��				
			else if(Sensor_MotorZero==0 && FirstSensorFlag==9)FirstSensorFlag=10;//��⵽0λ��								
			else if(Sensor_MotorZero==0 && FirstSensorFlag==10)//��⵽0λ��
			{
				FirstSensorFlag=0xff;
				i=StepNum-MotoGobackPul;
				Addnum = MotoGobackPul;
			}
			else if(Sensor_MotorZero!=0 && FirstSensorFlag!=0xff)FirstSensorFlag=0;//��⵽0λ��
		}
		if(Device_Prop.DEVICE_TYPE == _ADS16)
		{
			/****ƿ�������¼****/
			if(Pipe_Sen_old!=Sensor_HeightTest)     //�ܵ��ڴ��������� 1->0
			{
				if(Sensor_HeightTest == 0)
				{
					if(Sensor_HeightTest == 0)
					{
						//�Ƿ�Ҫ��¼ƿ�������־���˱�־�ڵ���ADS16��SendReady_Ctrl()����ʱ��1һ��
						if(First_Bootlecaptest == 1)  
						{
							First_Bootlecaptest = 0;
							BottleTop_Pulse = Addnum;  //��¼ƿ��������
						}
					}
				}
			}
			Pipe_Sen_old = Sensor_HeightTest;
			/******ƿ�Ǽ�⹤λ���*******/
			if(StepNum == POSITION_BOTTLECAP) //Ŀ�깤λΪƿ�Ǽ�⹤λ
			{
				if(Derict == DERICT_FRONT) 
				{
					if(Sensor_HeightTest==0 && FirstCapSensorFlag==0)FirstCapSensorFlag=1;//��⵽ƿ��
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==1)FirstCapSensorFlag=2;//��⵽0λ��		
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==2)FirstCapSensorFlag=3;//��⵽0λ��	
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==3)FirstCapSensorFlag=4;//��⵽0λ��	
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==4)FirstCapSensorFlag=5;//��⵽0λ��	
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==5)FirstCapSensorFlag=6;//��⵽0λ��	
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==6)FirstCapSensorFlag=7;//��⵽0λ��	
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==7)FirstCapSensorFlag=8;//��⵽0λ��		
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==8)FirstCapSensorFlag=9;//��⵽0λ��				
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==9)FirstCapSensorFlag=10;//��⵽0λ��								
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==10)//��⵽0λ��
					{
						FirstCapSensorFlag=0xff;
						i=StepNum-DISTANCE_CAP_TO_CAPTEST;  //�����ƶ��������ʺϼ��ƿ�ǵ�λ��
					}
					else if(Sensor_MotorZero!=0 && FirstCapSensorFlag!=0xff)FirstCapSensorFlag=0;//��⵽0λ��
				}
			}
		}
		if(Device_Prop.DEVICE_TYPE == _AES16D)
		{
			/**********�¹�λ���**********/
			if(Derict == DERICT_FRONT) //����Ϊ����㷽��ʱ�����������
			{
				if(Sensor_MotorDown==0 && FirstDownSensorFlag==0)FirstDownSensorFlag=1;//��⵽0λ��		
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==1)FirstDownSensorFlag=2;//��⵽0λ��		
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==2)FirstDownSensorFlag=3;//��⵽0λ��	
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==3)FirstDownSensorFlag=4;//��⵽0λ��	
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==4)FirstDownSensorFlag=5;//��⵽0λ��	
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==5)FirstDownSensorFlag=6;//��⵽0λ��	
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==6)FirstDownSensorFlag=7;//��⵽0λ��	
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==7)FirstDownSensorFlag=8;//��⵽0λ��		
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==8)FirstDownSensorFlag=9;//��⵽0λ��				
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==9)FirstDownSensorFlag=10;//��⵽0λ��								
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==10)//��⵽0λ��
				{
					FirstDownSensorFlag=0xff;
					i=StepNum-MotoGobackPul;
				}
				else if(Sensor_MotorDown!=0 && FirstDownSensorFlag!=0xff)FirstDownSensorFlag=0;//��⵽0λ��
			}			
		}
	}
	return 0;
}

//����ٶȿ���
//���룺�ܲ��� ��ǰ����
//�������ʱʱ��
u16 Motor_SpeedCtrl(u32 OverallStep,u32 NowStep)
{
	u16 SpeedCtrlThreshold;   //�Ӽ��ٷ�ֵ�������Ƿ�Ӽ���
	u16 PulseThreshold;       //��������ֵ�����ƼӼ���������
	u16 SpeedCtrl_PARM;       //�ٶȿ��Ʋ��������ƼӼ��ټ��ٶ�
	u32 Return_value;
	u16 Speed_PARM;           //�ٶȲ���
	//ͨ��ֵ
	SpeedCtrlThreshold = 10000;
	PulseThreshold = 5000;
	SpeedCtrl_PARM = 1250;
	Speed_PARM = SPEED;
	//����ֵ �ض�����վ�¶ԼӼ��ٿ��Ʋ�����������
	if(Device_Prop.DEVICE_TYPE == _AES16F)
	{
		SpeedCtrlThreshold = 5000;
		PulseThreshold = 2000;
		SpeedCtrl_PARM = 500;
		Speed_PARM = SPEED_AES16F;
	}
//-----------------------------------------
	if(OverallStep < SpeedCtrlThreshold)   //�ܲ���С�ڼӼ��ٷ�ֵ������
	{
		Return_value = Speed_PARM*5;
	}
	else
	{
		if(NowStep<PulseThreshold)
		{
			Return_value = Speed_PARM*5 - Speed_PARM*NowStep/SpeedCtrl_PARM;
		}
		else if(NowStep<OverallStep-PulseThreshold) Return_value=Speed_PARM;
		else	
		{
			Return_value = Speed_PARM + Speed_PARM*(PulseThreshold-(OverallStep-NowStep))/SpeedCtrl_PARM;
		}
	}
	return Return_value;
}

/*********************************************
*���ƣ��ƶ���Ŀ�깤λ
*���룺Ŀ�깤λ

**********************************************/
void MoveToPosition(u32 Position)
{
	u32 Pulplus;
	/*******************/
	if(Position == POSITION_BOTTLECAP)  //Ŀ�깤λΪƿ�Ǽ��λ
	{
		//Ŀ����ƿ�Ǽ��λ����������ʱ������㣬Ӧ�������
		if(Sensor_MotorZero != 0)
		{
			StepperMoto_42(_FAS_DISTANCE,DERICT_BACK);	//����λ	
			delay_ms(500);
			StepperMoto_42(POSITION_BOTTLECAP,DERICT_FRONT);	//Ŀ�깤λ:ƿ�Ǽ��λ		
		}
		//�������㣬��front��ƿ�Ǽ��λ
		else 
		{
			StepperMoto_42(POSITION_BOTTLECAP,DERICT_FRONT);	//Ŀ�깤λ:ƿ�Ǽ��λ		
		}
	}
	else   //���Ŀ�겻��ƿ�Ǽ��λ����Ϊ�̶���λ���ƶ����ù�λ
	{
		if(Addnum > Position)  //��ǰ��λ����Ŀ�깤λ������back
		{
			Pulplus = Addnum-Position;
			if(Pulplus > MotoGobackPul_NORMAL)
			{
				StepperMoto_42(Pulplus,DERICT_BACK);
			}
		}
		else									 //��ǰ��λС��Ŀ�깤λ������front
		{
			Pulplus = Position-Addnum;
			if(Pulplus > MotoGobackPul_NORMAL)
			{
				StepperMoto_42(Pulplus,DERICT_FRONT);
			}
		}
	}
}

/*********************************************
*���ƣ�����Ʒ�޳�����  /ADS16ʹ��
*��������BadBottle_Reject()

**********************************************/
u8 BadBottle_Reject()
{
	bool Sensor_SendPosition_save;
		
	ReSendCount = 0;   //�޳�ƿ�Ӻ����������ط�����
	Sensor_SendPosition_old = Sensor_SendPosition; //������һʱ�̷���λ������״̬
	BadBottle_Reject_timer = 3000;   //��ʱʱ�� 3�� ����3S��ʱ��ϵͳ����
	while(BadBottle_Reject_timer)    
	{
		Sensor_SendPosition_save = Sensor_SendPosition;  //����״̬����ͻ��
		if(Sensor_SendPosition_save == 1)
		{				
			Motor_OFF;
			BadBottle_Reject_timer = 0;
			return 0x00;  //�޳��ɹ�
		}
		Sensor_SendPosition_old = Sensor_SendPosition_save;
	}
	return 0x01;      //��ʱ���޳�ʧ��
}

/*********************************************
*���ƣ��Ƹ˿��Ƴ���  /AES16Dʹ��
*��������PushRod_Ctrl()
*���룺Ŀ�깤λPosition   �ٶ�Speed
*�����0x00�ɹ� 0x01ʧ��
**********************************************/
u8 PushRod_Ctrl(u8 Position, u8 Speed)
{
	if(Position == ROD_PUSH)     //Ŀ��Ϊ�ƹ�λ
	{
		if(Sensor_Rod_Push == 0)   //�Ѿ����ƹ�λ�����سɹ�
		{
			return 0x00;
		}
		else
		{
			Time_PushRod_Ctrl = 650;
			while(Time_PushRod_Ctrl)
			{
				Motor_F;	
			}
			Motor_S;	
			delay_ms(500);
			Timer_RodCtrl = 3000;   //3S��ʱ
			while(Timer_RodCtrl)    //��ʱʱ�����ж�
			{
				Motor_F;	
				//ת����Ŀ�깤λ
				if(Sensor_Rod_Push == 0)  //ת����Ŀ�깤λ
				{
					Motor_S;	
					return 0x00;
				}
				//���������ص���λ��һ��λ
				if(Read_OC == 0)				//���������ص���λ��һ��λ
				{
					Motor_S;
					delay_ms(500);
					PushRod_Ctrl(ROD_BACK,10);
					return 0x01;
				}
				//��ʱ����
				if(Timer_RodCtrl<5)
				{
					Motor_S;
					delay_ms(500);
					if(Sensor_Bottlefull == 0)  //ƿ��
					{
						
					}
					else
					{
						Device_Prop.DEVICE_STATUS = 0xf0;  //��ƿ�������FRONT��ʱ�������
					}
					PushRod_Ctrl(ROD_BACK,10);
					return 0x01;
				}
			}
		}
	}
	else if(Position == ROD_BACK)
	{
		if(Sensor_Rod_Back == 0)   //�Ѿ����ƹ�λ�����سɹ�
		{
			return 0x00;
		}
		else
		{
			Timer_RodCtrl = 3000;   //3S��ʱ
			while(Timer_RodCtrl)
			{
				Motor_R;	
				//ת����Ŀ�깤λ
				if(Sensor_Rod_Back == 0)  //ת����Ŀ�깤λ
				{
					Motor_S;	
					return 0x00;
				}
				//���������ص���λ��һ��λ
				if(Read_OC == 0)				//���������ص���λ��һ��λ
				{
					Motor_S;
					delay_ms(500);
					PushRod_Ctrl(ROD_PUSH,10);
					return 0x01;
				}
				//��ʱ����
				if(Timer_RodCtrl<5)
				{
					Motor_S;
					delay_ms(500);
					PushRod_Ctrl(ROD_BACK,10);
					return 0x01;
				}
			}
		}
	}
	return 0;
}


/*********************************************
*���ƣ����������Ƴ���  /AES16Aʹ��
*��������Valve_Ctrl()
*���룺Ŀ�깤λPosition  
*�����0x00�ɹ� 0x01ʧ��
**********************************************/
u8 Valve_Ctrl(u8 Position)
{
	if(Position == VALVE_CLOSE)     //Ŀ��Ϊ��
	{
		if(Sensor_Valve_Close == 0)   //�Ѿ��ڹع�λ�����سɹ�
		{
			return 0x00;
		}
		else
		{
			Timer_ValveCtrl = 3000;   //3S��ʱ
			while(Timer_ValveCtrl)    //��ʱʱ�����ж�
			{
				Motor_F;	
				//ת����Ŀ�깤λ
				if(Sensor_Valve_Close == 0)  //ת����Ŀ�깤λ
				{
					Motor_S;	
					return 0x00;
				}
				//���������ص���λ��һ��λ
				if(Read_OC == 0)				//���������ص���λ��һ��λ
				{
					Motor_S;
					delay_ms(500);
					Valve_Ctrl(VALVE_OPEN);
					return 0x01;
				}
				//��ʱ����
				if(Timer_ValveCtrl<5)
				{
					Motor_S;
					delay_ms(500);
					Device_Prop.DEVICE_STATUS = 0xf0;  //��ƿ�������FRONT��ʱ�������
					Valve_Ctrl(VALVE_OPEN);
					return 0x01;
				}
			}
		}
	}
	else if(Position == VALVE_OPEN)
	{
		if(Sensor_Valve_Open == 0)   //�Ѿ��ڿ���λ�����سɹ�
		{
			return 0x00;
		}
		else
		{
			Timer_ValveCtrl = 3000;   //3S��ʱ
			while(Timer_ValveCtrl)
			{
				Motor_R;	
				//ת����Ŀ�깤λ
				if(Sensor_Valve_Open == 0)  //ת����Ŀ�깤λ
				{
					Motor_S;	
					return 0x00;
				}
				//���������ص���λ��һ��λ
				if(Read_OC == 0)				//���������ص���λ��һ��λ
				{
					Motor_S;
					delay_ms(500);
					Valve_Ctrl(VALVE_CLOSE);
					return 0x01;
				}
				//��ʱ����
				if(Timer_ValveCtrl<5)
				{
					Motor_S;
					delay_ms(500);
					Valve_Ctrl(VALVE_CLOSE);
					return 0x01;
				}
			}
		}
	}
	return 0;
}

/*********************************************
*���ƣ��Ƹ˿��Ƴ��� AES16Fʹ��
*��������Rod_Ctrl_AES16F()
*���룺Ŀ�깤λPosition  
*�����0x00�ɹ� 0x01ʧ��
**********************************************/
u8 Rod_Ctrl_AES16F(u8 Position)
{
	if(Addnum < 500)   //��ֹ����߲�ʱ��ƿ���¿����ܿ�
	{
		return 0x01; //
	}
	if(Position == ROD_PUSH)     //Ŀ��Ϊ�ƹ�λ
	{
		if(Sensor_Rod_Push == 0)   //�Ѿ����ƹ�λ�����سɹ�
		{
			return 0x00;
		}
		else
		{
			Timer_RodCtrl = 3980;   //3S��ʱ
			while(Timer_RodCtrl)    //��ʱʱ�����ж�
			{
				Motor_F;	
				//ת����Ŀ�깤λ
				if(Sensor_Rod_Push == 0)  //ת����Ŀ�깤λ
				{
					delay_ms(300);
					Motor_S;	
					return 0x00;
				}
				//���������ص���λ��һ��λ
				if(Read_OC == 0)				//���������ص���λ��һ��λ
				{
					Motor_S;
					delay_ms(500);
					Rod_Ctrl_AES16F(ROD_BACK);
					Device_Prop.DEVICE_STATUS = 0xf0;
					System_state = STATE_ERR;
					return 0x01;
				}
				//��ʱ����
				if(Timer_RodCtrl<5)
				{
					Motor_S;
					delay_ms(500);
					Rod_Ctrl_AES16F(ROD_BACK);
					Device_Prop.DEVICE_STATUS = 0xf0;
					System_state = STATE_ERR;
					return 0x01;
				}
			}
		}
	}
	else if(Position == ROD_BACK)
	{
		if(Sensor_Rod_Back == 0)   //�Ѿ����ƹ�λ�����سɹ�
		{
			return 0x00;
		}
		else
		{
			Timer_RodCtrl = 6000;   //3S��ʱ
			while(Timer_RodCtrl)
			{
				Motor_R;	
				//ת����Ŀ�깤λ
				if(Sensor_Rod_Back == 0)  //ת����Ŀ�깤λ
				{
					delay_ms(300);
					Motor_S;	
					return 0x00;
				}
				//���������ص���λ��һ��λ
				if(Read_OC == 0)				//���������ص���λ��һ��λ
				{
					Motor_S;
					delay_ms(500);
					Rod_Ctrl_AES16F(ROD_PUSH);
					return 0x01;
				}
				//��ʱ����
				if(Timer_RodCtrl<5)
				{
					Motor_S;
					delay_ms(500);
					Rod_Ctrl_AES16F(ROD_PUSH);
					return 0x01;
				}
			}
		}
	}
	return 0;
}
	
void Rod_Ctrl_PUSH_AES16F()
{
	if(Addnum < 500)   //��ֹ����߲�ʱ��ƿ���¿����ܿ�
	{
		return; //
	}
	Timer_RodCtrl = 3980;   //3S��ʱ
	while(Timer_RodCtrl)    //��ʱʱ�����ж�
	{
		Motor_F;	
		//��ʱ����
		if(Timer_RodCtrl<5)
		{
			Motor_S;
			return;
		}
	}
}

/***********************/
