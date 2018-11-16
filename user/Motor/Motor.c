#include "Motor.h"
#include <stdbool.h>
#include "led.h"
#include "Sensor.h"
#include "can.h"																

/*************变量定义*************/
u32 Addnum=0;								//步进电机步数(脉冲累计)
u32 Pulplus=0;

bool Pipe_Sen_old=1;        //管道口传感器上一时刻状态存储

u16 BadBottle_Reject_timer; //不良瓶剔除超时时间
bool First_Bootlecaptest; //瓶盖测试
/*********************************/

void Motor_Init(void)		//工作站电机初始化
{	
	RCC->APB2ENR|=1<<3;			//使能PORTB时钟
	RCC->APB2ENR|=1<<4;			//使能PORTC时钟
	RCC->APB2ENR|=1<<6;			//使能PORTE时钟
	
	//PB 步进电机
	GPIOB->CRH&=0X000FFFFF;	//PB13~15	推挽输出 速度50MHz
	GPIOB->CRH|=0X33300000;
	
	//PE	滑动阀减速电机 L6203
	GPIOE->CRH&=0X00F0FFFF;	//PE12,PE14,PE15	推挽输出 速度2MHz
	GPIOE->CRH|=0X22020000;
	
	//PC 过流检测 
	GPIOC->CRH&=0XFFF0FFFF;	//PC12 上拉输入
	GPIOC->CRH|=0X00080000;

}

/*********************************************
*名称：42步进电机控制
*输入： 电机转动步数/脉冲数   
       回零位 1 回零位  0 向前走 （回零位时，检测到传感器时继续前进1cm 4000step）
*输出：0成功 1失步 2其它故障
**********************************************/
unsigned char StepperMoto_42(unsigned long StepNum,unsigned char Derict)
{
	//加速步数  为5000步 
	unsigned long i;
	unsigned long SpeedDelay;
	unsigned char FirstSensorFlag;          //首次检测到电机零位传感器标志
	unsigned char FirstCapSensorFlag;       //首次检测到瓶盖传感器标志
	unsigned char FirstDownSensorFlag;      //首次检测到下工位传感器标志
	u16 MotoGobackPul;    									//回到零位还要走的脉冲数
	/*********方向设置*********/
	//因为各个工作站的零点不同，为上工位或者下工位，所以需要设置方向
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
			//AES16D上下工位都有传感器，如果已经在下工位，方向为向下，则直接退出，防止过冲
			if(Sensor_MotorDown == 0)
			{
				return 0;
			}
		}
		else if(Device_Prop.DEVICE_TYPE == _ADS16)
		{
			SM_DIRH;				
		}
		//AES16F工作站需要控制刹车
		else if(Device_Prop.DEVICE_TYPE == _AES16F)
		{
			delay_ms(100);
			SM_DIRL;
		}
	}
	else if(Derict == DERICT_BACK)  //回零位
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
		//AES16F工作站需要控制刹车
		else if(Device_Prop.DEVICE_TYPE == _AES16F)
		{
			delay_ms(100);
			SM_DIRH;
			MotoGobackPul = MotoGobackPul_AES16F;  //设置回零点继续走的参数
		}
	}
	/**************************/
	FirstSensorFlag=0;
	for(i=0;i<StepNum;i++)
	{
		//根据总脉冲数跟当前脉冲数计算当前延时数
		SpeedDelay = Motor_SpeedCtrl(StepNum,i);
		
		/***********给脉冲**********/
		SM_PULH;
		delay_us(2);
		SM_PULL;
		delay_us(SpeedDelay);	
		/******步数计算*****/
		if(Derict == DERICT_FRONT)     //上升
		{
			Addnum++;
		}
		else if(Derict == DERICT_BACK) //下降
		{
			if(Addnum>0)Addnum--;
		}
		/**********零点检测**********/
		if(Derict == DERICT_BACK) //方向为回零点方向时，进行零点检测
		{
			if(Sensor_MotorZero==0 && FirstSensorFlag==0)FirstSensorFlag=1;//检测到0位了		
			else if(Sensor_MotorZero==0 && FirstSensorFlag==1)FirstSensorFlag=2;//检测到0位了		
			else if(Sensor_MotorZero==0 && FirstSensorFlag==2)FirstSensorFlag=3;//检测到0位了	
			else if(Sensor_MotorZero==0 && FirstSensorFlag==3)FirstSensorFlag=4;//检测到0位了	
			else if(Sensor_MotorZero==0 && FirstSensorFlag==4)FirstSensorFlag=5;//检测到0位了	
			else if(Sensor_MotorZero==0 && FirstSensorFlag==5)FirstSensorFlag=6;//检测到0位了	
			else if(Sensor_MotorZero==0 && FirstSensorFlag==6)FirstSensorFlag=7;//检测到0位了	
			else if(Sensor_MotorZero==0 && FirstSensorFlag==7)FirstSensorFlag=8;//检测到0位了		
			else if(Sensor_MotorZero==0 && FirstSensorFlag==8)FirstSensorFlag=9;//检测到0位了				
			else if(Sensor_MotorZero==0 && FirstSensorFlag==9)FirstSensorFlag=10;//检测到0位了								
			else if(Sensor_MotorZero==0 && FirstSensorFlag==10)//检测到0位了
			{
				FirstSensorFlag=0xff;
				i=StepNum-MotoGobackPul;
				Addnum = MotoGobackPul;
			}
			else if(Sensor_MotorZero!=0 && FirstSensorFlag!=0xff)FirstSensorFlag=0;//检测到0位了
		}
		if(Device_Prop.DEVICE_TYPE == _ADS16)
		{
			/****瓶顶脉冲记录****/
			if(Pipe_Sen_old!=Sensor_HeightTest)     //管道口传感器跳变 1->0
			{
				if(Sensor_HeightTest == 0)
				{
					if(Sensor_HeightTest == 0)
					{
						//是否要记录瓶顶脉冲标志，此标志在调用ADS16的SendReady_Ctrl()函数时置1一次
						if(First_Bootlecaptest == 1)  
						{
							First_Bootlecaptest = 0;
							BottleTop_Pulse = Addnum;  //记录瓶顶脉冲数
						}
					}
				}
			}
			Pipe_Sen_old = Sensor_HeightTest;
			/******瓶盖检测工位检测*******/
			if(StepNum == POSITION_BOTTLECAP) //目标工位为瓶盖检测工位
			{
				if(Derict == DERICT_FRONT) 
				{
					if(Sensor_HeightTest==0 && FirstCapSensorFlag==0)FirstCapSensorFlag=1;//检测到瓶盖
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==1)FirstCapSensorFlag=2;//检测到0位了		
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==2)FirstCapSensorFlag=3;//检测到0位了	
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==3)FirstCapSensorFlag=4;//检测到0位了	
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==4)FirstCapSensorFlag=5;//检测到0位了	
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==5)FirstCapSensorFlag=6;//检测到0位了	
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==6)FirstCapSensorFlag=7;//检测到0位了	
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==7)FirstCapSensorFlag=8;//检测到0位了		
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==8)FirstCapSensorFlag=9;//检测到0位了				
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==9)FirstCapSensorFlag=10;//检测到0位了								
					else if(Sensor_HeightTest==0 && FirstCapSensorFlag==10)//检测到0位了
					{
						FirstCapSensorFlag=0xff;
						i=StepNum-DISTANCE_CAP_TO_CAPTEST;  //继续移动步数到适合检测瓶盖的位置
					}
					else if(Sensor_MotorZero!=0 && FirstCapSensorFlag!=0xff)FirstCapSensorFlag=0;//检测到0位了
				}
			}
		}
		if(Device_Prop.DEVICE_TYPE == _AES16D)
		{
			/**********下工位检测**********/
			if(Derict == DERICT_FRONT) //方向为回零点方向时，进行零点检测
			{
				if(Sensor_MotorDown==0 && FirstDownSensorFlag==0)FirstDownSensorFlag=1;//检测到0位了		
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==1)FirstDownSensorFlag=2;//检测到0位了		
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==2)FirstDownSensorFlag=3;//检测到0位了	
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==3)FirstDownSensorFlag=4;//检测到0位了	
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==4)FirstDownSensorFlag=5;//检测到0位了	
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==5)FirstDownSensorFlag=6;//检测到0位了	
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==6)FirstDownSensorFlag=7;//检测到0位了	
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==7)FirstDownSensorFlag=8;//检测到0位了		
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==8)FirstDownSensorFlag=9;//检测到0位了				
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==9)FirstDownSensorFlag=10;//检测到0位了								
				else if(Sensor_MotorDown==0 && FirstDownSensorFlag==10)//检测到0位了
				{
					FirstDownSensorFlag=0xff;
					i=StepNum-MotoGobackPul;
				}
				else if(Sensor_MotorDown!=0 && FirstDownSensorFlag!=0xff)FirstDownSensorFlag=0;//检测到0位了
			}			
		}
	}
	return 0;
}

//电机速度控制
//输入：总步数 当前步数
//输出：延时时间
u16 Motor_SpeedCtrl(u32 OverallStep,u32 NowStep)
{
	u16 SpeedCtrlThreshold;   //加减速阀值，控制是否加减速
	u16 PulseThreshold;       //脉冲数阀值，控制加减速脉冲数
	u16 SpeedCtrl_PARM;       //速度控制参数，控制加减速加速度
	u32 Return_value;
	u16 Speed_PARM;           //速度参数
	//通常值
	SpeedCtrlThreshold = 10000;
	PulseThreshold = 5000;
	SpeedCtrl_PARM = 1250;
	Speed_PARM = SPEED;
	//特殊值 特定工作站下对加减速控制参数重新设置
	if(Device_Prop.DEVICE_TYPE == _AES16F)
	{
		SpeedCtrlThreshold = 5000;
		PulseThreshold = 2000;
		SpeedCtrl_PARM = 500;
		Speed_PARM = SPEED_AES16F;
	}
//-----------------------------------------
	if(OverallStep < SpeedCtrlThreshold)   //总步数小于加减速阀值，匀速
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
*名称：移动到目标工位
*输入：目标工位

**********************************************/
void MoveToPosition(u32 Position)
{
	u32 Pulplus;
	/*******************/
	if(Position == POSITION_BOTTLECAP)  //目标工位为瓶盖检测位
	{
		//目标是瓶盖检测位，如果电机此时不在零点，应返回零点
		if(Sensor_MotorZero != 0)
		{
			StepperMoto_42(_FAS_DISTANCE,DERICT_BACK);	//回零位	
			delay_ms(500);
			StepperMoto_42(POSITION_BOTTLECAP,DERICT_FRONT);	//目标工位:瓶盖检测位		
		}
		//电机在零点，则front到瓶盖检测位
		else 
		{
			StepperMoto_42(POSITION_BOTTLECAP,DERICT_FRONT);	//目标工位:瓶盖检测位		
		}
	}
	else   //如果目标不是瓶盖检测位，则为固定工位，移动到该工位
	{
		if(Addnum > Position)  //当前工位大于目标工位，方向back
		{
			Pulplus = Addnum-Position;
			if(Pulplus > MotoGobackPul_NORMAL)
			{
				StepperMoto_42(Pulplus,DERICT_BACK);
			}
		}
		else									 //当前工位小于目标工位，方向front
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
*名称：不良品剔除程序  /ADS16使用
*函数名：BadBottle_Reject()

**********************************************/
u8 BadBottle_Reject()
{
	bool Sensor_SendPosition_save;
		
	ReSendCount = 0;   //剔除瓶子后，清零连续重发次数
	Sensor_SendPosition_old = Sensor_SendPosition; //更新上一时刻发送位传感器状态
	BadBottle_Reject_timer = 3000;   //超时时间 3秒 超过3S则超时，系统报警
	while(BadBottle_Reject_timer)    
	{
		Sensor_SendPosition_save = Sensor_SendPosition;  //保存状态避免突变
		if(Sensor_SendPosition_save == 1)
		{				
			Motor_OFF;
			BadBottle_Reject_timer = 0;
			return 0x00;  //剔除成功
		}
		Sensor_SendPosition_old = Sensor_SendPosition_save;
	}
	return 0x01;      //超时，剔除失败
}

/*********************************************
*名称：推杆控制程序  /AES16D使用
*函数名：PushRod_Ctrl()
*输入：目标工位Position   速度Speed
*输出：0x00成功 0x01失败
**********************************************/
u8 PushRod_Ctrl(u8 Position, u8 Speed)
{
	if(Position == ROD_PUSH)     //目标为推工位
	{
		if(Sensor_Rod_Push == 0)   //已经在推工位，返回成功
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
			Timer_RodCtrl = 3000;   //3S超时
			while(Timer_RodCtrl)    //超时时间内判断
			{
				Motor_F;	
				//转到了目标工位
				if(Sensor_Rod_Push == 0)  //转到了目标工位
				{
					Motor_S;	
					return 0x00;
				}
				//过流处理，回到工位另一工位
				if(Read_OC == 0)				//过流处理，回到工位另一工位
				{
					Motor_S;
					delay_ms(500);
					PushRod_Ctrl(ROD_BACK,10);
					return 0x01;
				}
				//超时处理
				if(Timer_RodCtrl<5)
				{
					Motor_S;
					delay_ms(500);
					if(Sensor_Bottlefull == 0)  //瓶满
					{
						
					}
					else
					{
						Device_Prop.DEVICE_STATUS = 0xf0;  //非瓶满，如果FRONT超时，则故障
					}
					PushRod_Ctrl(ROD_BACK,10);
					return 0x01;
				}
			}
		}
	}
	else if(Position == ROD_BACK)
	{
		if(Sensor_Rod_Back == 0)   //已经在推工位，返回成功
		{
			return 0x00;
		}
		else
		{
			Timer_RodCtrl = 3000;   //3S超时
			while(Timer_RodCtrl)
			{
				Motor_R;	
				//转到了目标工位
				if(Sensor_Rod_Back == 0)  //转到了目标工位
				{
					Motor_S;	
					return 0x00;
				}
				//过流处理，回到工位另一工位
				if(Read_OC == 0)				//过流处理，回到工位另一工位
				{
					Motor_S;
					delay_ms(500);
					PushRod_Ctrl(ROD_PUSH,10);
					return 0x01;
				}
				//超时处理
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
*名称：滑动阀控制程序  /AES16A使用
*函数名：Valve_Ctrl()
*输入：目标工位Position  
*输出：0x00成功 0x01失败
**********************************************/
u8 Valve_Ctrl(u8 Position)
{
	if(Position == VALVE_CLOSE)     //目标为关
	{
		if(Sensor_Valve_Close == 0)   //已经在关工位，返回成功
		{
			return 0x00;
		}
		else
		{
			Timer_ValveCtrl = 3000;   //3S超时
			while(Timer_ValveCtrl)    //超时时间内判断
			{
				Motor_F;	
				//转到了目标工位
				if(Sensor_Valve_Close == 0)  //转到了目标工位
				{
					Motor_S;	
					return 0x00;
				}
				//过流处理，回到工位另一工位
				if(Read_OC == 0)				//过流处理，回到工位另一工位
				{
					Motor_S;
					delay_ms(500);
					Valve_Ctrl(VALVE_OPEN);
					return 0x01;
				}
				//超时处理
				if(Timer_ValveCtrl<5)
				{
					Motor_S;
					delay_ms(500);
					Device_Prop.DEVICE_STATUS = 0xf0;  //非瓶满，如果FRONT超时，则故障
					Valve_Ctrl(VALVE_OPEN);
					return 0x01;
				}
			}
		}
	}
	else if(Position == VALVE_OPEN)
	{
		if(Sensor_Valve_Open == 0)   //已经在开工位，返回成功
		{
			return 0x00;
		}
		else
		{
			Timer_ValveCtrl = 3000;   //3S超时
			while(Timer_ValveCtrl)
			{
				Motor_R;	
				//转到了目标工位
				if(Sensor_Valve_Open == 0)  //转到了目标工位
				{
					Motor_S;	
					return 0x00;
				}
				//过流处理，回到工位另一工位
				if(Read_OC == 0)				//过流处理，回到工位另一工位
				{
					Motor_S;
					delay_ms(500);
					Valve_Ctrl(VALVE_CLOSE);
					return 0x01;
				}
				//超时处理
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
*名称：推杆控制程序 AES16F使用
*函数名：Rod_Ctrl_AES16F()
*输入：目标工位Position  
*输出：0x00成功 0x01失败
**********************************************/
u8 Rod_Ctrl_AES16F(u8 Position)
{
	if(Addnum < 500)   //防止在最高层时推瓶导致卡到管口
	{
		return 0x01; //
	}
	if(Position == ROD_PUSH)     //目标为推工位
	{
		if(Sensor_Rod_Push == 0)   //已经在推工位，返回成功
		{
			return 0x00;
		}
		else
		{
			Timer_RodCtrl = 3980;   //3S超时
			while(Timer_RodCtrl)    //超时时间内判断
			{
				Motor_F;	
				//转到了目标工位
				if(Sensor_Rod_Push == 0)  //转到了目标工位
				{
					delay_ms(300);
					Motor_S;	
					return 0x00;
				}
				//过流处理，回到工位另一工位
				if(Read_OC == 0)				//过流处理，回到工位另一工位
				{
					Motor_S;
					delay_ms(500);
					Rod_Ctrl_AES16F(ROD_BACK);
					Device_Prop.DEVICE_STATUS = 0xf0;
					System_state = STATE_ERR;
					return 0x01;
				}
				//超时处理
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
		if(Sensor_Rod_Back == 0)   //已经在推工位，返回成功
		{
			return 0x00;
		}
		else
		{
			Timer_RodCtrl = 6000;   //3S超时
			while(Timer_RodCtrl)
			{
				Motor_R;	
				//转到了目标工位
				if(Sensor_Rod_Back == 0)  //转到了目标工位
				{
					delay_ms(300);
					Motor_S;	
					return 0x00;
				}
				//过流处理，回到工位另一工位
				if(Read_OC == 0)				//过流处理，回到工位另一工位
				{
					Motor_S;
					delay_ms(500);
					Rod_Ctrl_AES16F(ROD_PUSH);
					return 0x01;
				}
				//超时处理
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
	if(Addnum < 500)   //防止在最高层时推瓶导致卡到管口
	{
		return; //
	}
	Timer_RodCtrl = 3980;   //3S超时
	while(Timer_RodCtrl)    //超时时间内判断
	{
		Motor_F;	
		//超时处理
		if(Timer_RodCtrl<5)
		{
			Motor_S;
			return;
		}
	}
}

/***********************/
