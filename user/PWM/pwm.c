#include "pwm.h"
#include "led.h"

#include "Motor.h"
TIM_OCInitTypeDef  TIM_OCInitStructure;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TYPE_StepMotor Axis_X;
TYPE_StepMotor Axis_Z;
TYPE_StepMotor Axis_Y;
TYPEDEF_ACTION Action;

bool x_stop_flag=0;
u32 X_Zero_Pluse;
double X_MPP;
double Z_MPP;
u32 Y_RightZero_Pluse;
u32 Y_LeftZero_Pluse;
u32 Y_LeftTrayPluse;
u32 Y_LeftLim_MorePluse;
u32 Y_LeftLim_LessPluse;
u32 Y_RightLim_MorePluse;
u32 Y_RightLim_LessPluse;

bool X_ZEROFLAG ;
u8 Motor_CtrlOver_Flag;		//������ƽ�����־���˱�־��1���ϴ���������Ӧ��
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void PWM_Init()
{  
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD,ENABLE);
	
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //ʹ��GPIO�����AFIO���ù���ģ��ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//����TIM3��CH2�����PWMͨ����LED��ʾ
 //���ø�����Ϊ�����������,���TIM3 CH3��TIM4 CH1��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_6; //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	//���ø�����Ϊ�����������,���TIM2 CH1��PWM���岨��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ; //
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //�����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
			/************TIM2 CH1 X����PWM��ʼ��*********/
//	TIM_TimeBaseStructure.TIM_Period=3000-1;                   // �Զ���װ�ؼĴ�����ֵ        
//	TIM_TimeBaseStructure.TIM_Prescaler=5;                  // ʱ��Ԥ��Ƶ��        
//	TIM_TimeBaseStructure.TIM_Prescaler=72-1;                  // ʱ��Ԥ��Ƶ��        
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//	TIM_OCInitStructure.TIM_Pulse=900;                                 //����ռ�ձ�ʱ��         
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
		
	TIM_Cmd(TIM2, DISABLE);
//	TIM_Cmd(TIM2, ENABLE);
	
	
		/************TIM3 CH4 Y����PWM��ʼ��*********/
//	TIM_TimeBaseStructure.TIM_Period=3000-1;                   // �Զ���װ�ؼĴ�����ֵ        
//	TIM_TimeBaseStructure.TIM_Prescaler=5;                  // ʱ��Ԥ��Ƶ��        
//	TIM_TimeBaseStructure.TIM_Prescaler=72-1;                  // ʱ��Ԥ��Ƶ��        
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//	TIM_OCInitStructure.TIM_Pulse=900;                                 //����ռ�ձ�ʱ��         
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	
	TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
		
	TIM_Cmd(TIM3, DISABLE);
//	TIM_Cmd(TIM3, ENABLE);

	/************TIM4 CH1	Z����PWM��ʼ��*********/
	TIM_TimeBaseStructure.TIM_Prescaler =0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  ����Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
		
	TIM_Cmd(TIM4, DISABLE);
	
//	TIM_Cmd(TIM4, ENABLE);
	/************EXIT 1*********/
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
  //GPIOC.5�ж����Լ��жϳ�ʼ������
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource1);
  EXTI_InitStructure.EXTI_Line=EXTI_Line1;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn ;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�2 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;					//�����ȼ�2 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
  
	/************EXIT 0*********/
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOB, &GPIO_InitStructure);
  //GPIOC.1 �ж����Լ��жϳ�ʼ������
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
  EXTI_InitStructure.EXTI_Line=EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�2 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//�����ȼ�2 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
		/************EXIT 4*********/
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
  //GPIOC.1 �ж����Լ��жϳ�ʼ������
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource4);
  EXTI_InitStructure.EXTI_Line=EXTI_Line4;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);	 	//����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���
  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//ʹ�ܰ������ڵ��ⲿ�ж�ͨ��
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//��ռ���ȼ�2 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;					//�����ȼ�2 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//ʹ���ⲿ�ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);  	  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4;//PE0��P27_DIR2,PE2��P27_PUL2,PE4��P28_DIR1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				
 	GPIO_Init(GPIOE, &GPIO_InitStructure);									
	GPIO_ResetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_4);
	
	
		/************DERECTION*********/
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 				//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOA, &GPIO_InitStructure);									//��ʼ��GPIOEA
  
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 				//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOC, &GPIO_InitStructure);								//��ʼ��GPIOEA
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 				//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
		/************ALM����*********/
		GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8; //IO		X:PA7 Y:PA6 Z:PA8
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOA, &GPIO_InitStructure);				//��ʼ��GPIOB
}


//PWM���ARR��ֵ����
u32 PWMCtrl_ARRCount(u32 pcs, float Fre)//���� PCS Ƶ��
{
	float ARR_status;
	float status;
	status = pcs+1;
	status = status * Fre;
	ARR_status =(72000000/status)-1;
//	ARR_status =(648648/status)-1;
//	ARR_status = (72000000/((pcs+1)*Fre))-1;
	(u32)ARR_status;
	return ARR_status;
}

void PWM_Set_X(u32 Arr,u32 Psc)
{
	if(TIM_GetCounter(TIM2) > Arr)//�жϵ�ǰ��������ֵ�Ƿ����ARR�������������
	{
		TIM_SetCounter(TIM2,0);
	}
  TIM2->ARR = Arr;
  TIM2->PSC = Psc;
  TIM2->CCR1 = Arr/2;
}



void PWM_Set_Y(u32 Arr,u32 Psc)
{
	if(TIM_GetCounter(TIM3) > Arr)//�жϵ�ǰ��������ֵ�Ƿ����ARR�������������
	{
		TIM_SetCounter(TIM3,0);
	}
  TIM3->ARR = Arr;
  TIM3->PSC = Psc;
  TIM3->CCR4 = Arr/2;
}

void PWM_Set_Z(u32 Arr,u32 Psc)
{
	if(TIM_GetCounter(TIM4) > Arr)//�жϵ�ǰ��������ֵ�Ƿ����ARR�������������
	{
		TIM_SetCounter(TIM4,0);
	}
  TIM4->ARR = Arr;
  TIM4->PSC = Psc;
  TIM4->CCR1 = Arr/2;
}



void Motor_MoveToPosition(u16 AXIS_X_Mm,u16 AXIS_Z_Mm)
{
	float TargetPulse_X;
	float TargetPulse_Z;
	u32 Differrence_X;
	u32 Differrence_Z;

	bool Dir_X;
	bool Dir_Z;
	/**********X������������********/
	
	TargetPulse_X = (float)(AXIS_X_Mm/X_MPP);

	if(TargetPulse_X > Axis_X.Coordinate)//Ŀ��������ڵ�ǰ����
	{
		Dir_X = X_DIR_FRONT;
		Differrence_X = TargetPulse_X - Axis_X.Coordinate;
	}
	else
	{
		Dir_X = X_DIR_BACK;
		Differrence_X = Axis_X.Coordinate - TargetPulse_X;
	}
	/**********Z������������********/
	
	TargetPulse_Z = (float)(AXIS_Z_Mm/Z_MPP);
//	Z_New_Error = (int)(TargetPulse_Z*100)%100;
//		if((Z_New_Error+Z_Old_Error)>=100)
//		{
//			TargetPulse_Z=(u32)(TargetPulse_Z+1);
//			Z_Old_Error = (Z_Old_Error+Z_New_Error)%100;
//		}
//		else
//		{
//			Z_Old_Error = Z_Old_Error+Z_New_Error;
//		}
	if(TargetPulse_Z > Axis_Z.Coordinate)//Ŀ��������ڵ�ǰ����
	{
		Dir_Z = Z_DIR_FRONT;
		Differrence_Z = TargetPulse_Z - Axis_Z.Coordinate;
	}
	else
	{
		Dir_Z = Z_DIR_BACK;
		Differrence_Z = Axis_Z.Coordinate - TargetPulse_Z;
	}
	StepMotorCtrl_Pulse(Differrence_X,AXIS_X,Dir_X);
	StepMotorCtrl_Pulse(Differrence_Z,AXIS_Z,Dir_Z);
}



/*******************������λ***********************/
void Tray_Limit_Contr()
{
	static u8 Y_Left_Cnt=0;
	static u8 Y_Right_Cnt=0;
	if(Axis_Y.InCtrl_Flag)
	{
/*************����ԭ����λֹͣ����������*************/
	if(Proximity_1 == 0&&
			Proximity_2 == 0&&
			Proximity_3 == 0&&
			Axis_Y.Dir == Y_DIR_LEFT&&
			Axis_Y.NowPulse>200&&
			XYZ_To_Zero.Tray_Reset_Flag == 0
		)
	{
		Y_Left_Cnt++;
		if(Y_Left_Cnt == 1)
		{
			Axis_Y.NowPulse = Axis_Y.Target_Pulse - Y_LeftZero_Pluse;
		}
		while(Axis_Y.InCtrl_Flag);
		Axis_Y.InCtrl_Flag = 0;
		TIM_Cmd(TIM3,DISABLE);
		Y_Left_Cnt = 0;
		Axis_Y.YCoordinate = 0;
	}
	/*************����ԭ����λֹͣ����������*************/
	if(Proximity_1 == 0&&
		Proximity_2 == 0&&
		Proximity_3 == 0&&
		Axis_Y.Dir == Y_DIR_RIGHT&&
		Axis_Y.NowPulse>200&&
		XYZ_To_Zero.Tray_Reset_Flag == 0
		)
	{
		Y_Right_Cnt++;
		if(Y_Right_Cnt == 1)
		{
			Axis_Y.NowPulse = Axis_Y.Target_Pulse - Y_RightZero_Pluse;
		}
		while(Axis_Y.InCtrl_Flag);
		Axis_Y.InCtrl_Flag = 0;
		TIM_Cmd(TIM3, DISABLE);
		Axis_Y.YCoordinate = 0;
		Y_Right_Cnt = 0;
	}
	/*********************�����Զ���λ���Ҹ�λ����***********************************/
	if(A_SENSOR == 0&&
			B_SENSOR == 0&&
			XYZ_To_Zero.Tray_Reset_Flag == 1&&
			Axis_Y.Dir == Y_DIR_RIGHT&&
			Axis_Y.NowPulse>10
			)
			{
				Y_Right_Cnt++;
				if(Y_Right_Cnt == 1)
				{
					Axis_Y.NowPulse = Axis_Y.Target_Pulse - Tray_AutoReset_Pulse;
				}
				while(Axis_Y.InCtrl_Flag);
				Axis_Y.InCtrl_Flag = 0;
				TIM_Cmd(TIM3, DISABLE);
				Axis_Y.YCoordinate = 0;
				Y_Right_Cnt = 0;
				XYZ_To_Zero.Tray_Reset_Flag = 0;
			}
	/*********************�����Զ���λ����λ����***********************************/
		if(A_SENSOR == 0&&
			B_SENSOR == 0&&
			XYZ_To_Zero.Tray_Reset_Flag == 1&&
			Axis_Y.Dir == Y_DIR_LEFT&&
			Axis_Y.NowPulse>10
			)
			{
				Y_Right_Cnt++;
				if(Y_Right_Cnt == 1)
				{
					Axis_Y.NowPulse = Axis_Y.Target_Pulse - Tray_AutoReset_Pulse;
				}
				while(Axis_Y.InCtrl_Flag);
				Axis_Y.InCtrl_Flag = 0;
				TIM_Cmd(TIM3, DISABLE);
				Axis_Y.YCoordinate = 0;
				Y_Right_Cnt = 0;
				XYZ_To_Zero.Tray_Reset_Flag = 0;
			}
	}
}
/***********XZ����λ***************************/
void Axis_Ultra_Limit()
{
	if(Axis_Z.InCtrl_Flag)
	{
/*************Z������λ**************/		
		if(Axis_Z_Up_Sensor == 0&&Axis_Z.Dir == Z_DIR_FRONT)//
		{
			Beep(50,50,1);
			Axis_Z.InCtrl_Flag = 0;
			TIM_Cmd(TIM4, DISABLE);
			Action_Stop();
		}		

/*************Z������λ����λ**************/	
		if(Axis_Z_UpLim_Sensor == 0&&Axis_Z.Dir == Z_DIR_FRONT)
		{
			Axis_Z.InCtrl_Flag = 0;
			TIM_Cmd(TIM4, DISABLE);
			Beep(50,50,5);
		}
		
	}
	
	
			if(x_stop_flag == 1)
		{
			x_stop_flag = 0;
			DelayTimes_ms = 500;
			while(DelayTimes_ms);
			StepMotorCtrl_Zero_Pulse(X_Zero_Pluse,X_DIR_FRONT);
				while(Axis_X.InCtrl_Flag);
				Axis_X.Coordinate = 0;
				XYZ_To_Zero.X_Return_Flag = 1;
				//Auto_AdjustX.Back_Zero_Flag = 1;
		}
/*************X��ǰ��λ**************/
	if(Axis_X.InCtrl_Flag)
	{

/*************X��ǰ��λ����λ*************/
		if(Axis_X_ForwardLim_Sensor == 0&&Axis_X.Dir == X_DIR_BACK)//
		{
			Axis_X.InCtrl_Flag = 0;
			TIM_Cmd(TIM2, DISABLE);
			Beep(50,50,5);			
		}
		
/*************X�����λ**************/
		if(Axis_X_Backward_Sensor == 0&&Axis_X.Dir == X_DIR_FRONT)
		{
			Axis_X.InCtrl_Flag = 0;
			TIM_Cmd(TIM2, DISABLE);
			Beep(50,50,1);
		}
/*************X�����λ����λ**************/		
		if(Axis_X_BackwardLim_Sensor == 0&&Axis_X.Dir == X_DIR_FRONT)//
		{
			Axis_X.InCtrl_Flag = 0;
			TIM_Cmd(TIM2, DISABLE);
			Beep(50,50,5);
			//Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Fifth_Stage,0);//���̷�����������ر���
		}
	}

}

void StepMotorCtrl_Zero_Pulse(u32 Target ,bool Dir)//X��ص����ص���������
{

		Axis_X.Dir = X_DIR_FRONT;
		DIR_X = Dir;
		Axis_X.Target_Pulse = Target;
		Axis_X.NowPulse = 0;    //���㵱ǰ������
		Axis_X.InCtrl_Flag = 3;//��λ����ģʽ

}

void StepMotorCtrl_Pulse(u32 Target,u8 AXIS,bool Dir)//��Ŀ���ֵ���ᣬ����
{
  u8 i;
  if(AXIS == AXIS_X)
  {
			DIR_X = Dir;
			Axis_X.Dir = Dir;	
		//�ж��Ƿ�λ����λ����BACK������
		if((Axis_X_Forward_Sensor == 0)&&(Dir==X_DIR_BACK))
		{
			return;
		}
    if(Axis_X.InCtrl_Flag == 0)
    {
			Axis_X.Target_Pulse = Target;
			
			//�ж��Ƿ�Ϊ����ģʽ
      if(Target < X_HORL_PULSE_VALVE_L)
      {
				Axis_X.Target_Fre = X_SPEED_FREQ_L;
				Axis_X.Acc_Speed  = (float)(4*Axis_X.Target_Fre-4*X_STAR_FREQ)/(2*X_Acc_Time_L);
				Axis_X.Jerk_Speed = (float)(8*Axis_X.Target_Fre-8*X_STAR_FREQ)/(2*X_Acc_Time_L*X_Acc_Time_L);
        Axis_X.NowFreq = X_STAR_FREQ;    //��ó�ʼƵ�ʣ��Խ����ⲿ�ж�
        Axis_X.NowPulse = 0;    //���㵱ǰ������
        Axis_X.Timer_Cnt = 0;   //���㵱ǰ��ʱ��
        for(i=0;i<7;i++)        //���㵱ǰʱ��ڵ㣬����ڵ�
        {
          Axis_X.PulsePoint[i] = 0;
          Axis_X.TimerPoint[i] = 0;
          Axis_X.FreqPoint[i] = 0;
        }
        Axis_X.InCtrl_Flag = 2;//����ģʽ
        Axis_X.SpeedChange_Stage = 1;
      }
			//�ж��Ƿ�Ϊ����ģʽ
			else if(Target < X_HORL_PULSE_VALVE_M)
			{
				Axis_X.Target_Fre = X_SPEED_FREQ_M;
				Axis_X.Acc_Speed  = (float)(4*Axis_X.Target_Fre-4*X_STAR_FREQ)/(2*X_Acc_Time_M);
				Axis_X.Jerk_Speed = (float)(8*Axis_X.Target_Fre-8*X_STAR_FREQ)/(2*X_Acc_Time_M*X_Acc_Time_M);
				
        Axis_X.NowFreq = X_STAR_FREQ;    //��ó�ʼƵ�ʣ��Խ����ⲿ�ж�
        Axis_X.NowPulse = 0;    //���㵱ǰ������
        Axis_X.Timer_Cnt = 0;   //���㵱ǰ��ʱ��
        for(i=0;i<7;i++)        //���㵱ǰʱ��ڵ㣬����ڵ�
        {
          Axis_X.PulsePoint[i] = 0;
          Axis_X.TimerPoint[i] = 0;
          Axis_X.FreqPoint[i] = 0;
        }
        Axis_X.InCtrl_Flag = 2;//����ģʽ
        Axis_X.SpeedChange_Stage = 1;
			}
			//�ж��Ƿ�Ϊ����ģʽ
      else 
      {
				//���üӼ��ٲ������ֱ�ΪĿ��Ƶ�ʣ��Ӽ��ٶȣ��ȼ��ټ��ٶ�
				Axis_X.Target_Fre = X_SPEED_FREQ_H;
				Axis_X.Acc_Speed  = (float)(4*Axis_X.Target_Fre-4*X_STAR_FREQ)/(2*X_Acc_Time_H);
				Axis_X.Jerk_Speed = (float)(8*Axis_X.Target_Fre-8*X_STAR_FREQ)/(2*X_Acc_Time_H*X_Acc_Time_H);
				
        Axis_X.NowFreq = X_STAR_FREQ;    //��ó�ʼƵ�ʣ��Խ����ⲿ�ж�
        Axis_X.NowPulse = 0;    //���㵱ǰ������
        Axis_X.Timer_Cnt = 0;   //���㵱ǰ��ʱ��
        for(i=0;i<7;i++)        //���㵱ǰʱ��ڵ㣬����ڵ�
        {
          Axis_X.PulsePoint[i] = 0;
          Axis_X.TimerPoint[i] = 0;
          Axis_X.FreqPoint[i] = 0;
        }
        Axis_X.InCtrl_Flag = 2;//����ģʽ
        Axis_X.SpeedChange_Stage = 1;
      }
    }
  }
  else if(AXIS == AXIS_Z)
  {
		DIR_Z = Dir;
		Axis_Z.Dir = Dir;
		if((Axis_Z_Down_Sensor == 0)&&(Dir==Z_DIR_BACK))
		{
			return;
		}
    if(Axis_Z.InCtrl_Flag == 0)
    {
      Axis_Z.Target_Pulse = Target;
			if(Target < Z_HORL_PULSE_VALVE_SL)
			{
				//���üӼ��ٲ������ֱ�ΪĿ��Ƶ�ʣ��Ӽ��ٶȣ��ȼ��ټ��ٶ�
				Axis_Z.Target_Fre = Z_SPEED_FREQ_SL ;
				Axis_Z.Jerk_Speed = (float)(8*Axis_Z.Target_Fre-8*Z_STAR_FREQ)/(2*Z_Acc_Time_SL*Z_Acc_Time_SL);
				Axis_Z.Acc_Speed  = (float)(4*Axis_Z.Target_Fre-4*Z_STAR_FREQ)/(2*Z_Acc_Time_SL);
				
        Axis_Z.NowFreq = Z_STAR_FREQ;    //��ó�ʼƵ�ʣ��Խ����ⲿ�ж�
        Axis_Z.NowPulse = 0;    //���㵱ǰ������
        Axis_Z.Timer_Cnt = 0;   //���㵱ǰ��ʱ��
        for(i=0;i<7;i++)        //���㵱ǰʱ��ڵ㣬����ڵ�
        {
          Axis_Z.PulsePoint[i] = 0;
          Axis_Z.TimerPoint[i] = 0;
          Axis_Z.FreqPoint[i] = 0;
        }
        Axis_Z.InCtrl_Flag = 2;//�Ӽ���ģʽ
        Axis_Z.SpeedChange_Stage = 1;
			}
			//�ж��Ƿ�Ϊ����ģʽ
			else if(Target < Z_HORL_PULSE_VALVE_L)
			{
				//���üӼ��ٲ������ֱ�ΪĿ��Ƶ�ʣ��Ӽ��ٶȣ��ȼ��ټ��ٶ�
				Axis_Z.Target_Fre = Z_SPEED_FREQ_L ;
				Axis_Z.Jerk_Speed = (float)(8*Axis_Z.Target_Fre-8*Z_STAR_FREQ)/(2*Z_Acc_Time_L*Z_Acc_Time_L);
				Axis_Z.Acc_Speed  = (float)(4*Axis_Z.Target_Fre-4*Z_STAR_FREQ)/(2*Z_Acc_Time_L);
				
        Axis_Z.NowFreq = Z_STAR_FREQ;    //��ó�ʼƵ�ʣ��Խ����ⲿ�ж�
        Axis_Z.NowPulse = 0;    //���㵱ǰ������
        Axis_Z.Timer_Cnt = 0;   //���㵱ǰ��ʱ��
        for(i=0;i<7;i++)        //���㵱ǰʱ��ڵ㣬����ڵ�
        {
          Axis_Z.PulsePoint[i] = 0;
          Axis_Z.TimerPoint[i] = 0;
          Axis_Z.FreqPoint[i] = 0;
        }
        Axis_Z.InCtrl_Flag = 2;//�Ӽ���ģʽ
        Axis_Z.SpeedChange_Stage = 1;
			}
			//�ж��Ƿ�Ϊ����ģʽ
			else if(Target < Z_HORL_PULSE_VALVE_M)
			{
				//���üӼ��ٲ������ֱ�ΪĿ��Ƶ�ʣ��Ӽ��ٶȣ��ȼ��ټ��ٶ�
				Axis_Z.Target_Fre = Z_SPEED_FREQ_M;
				Axis_Z.Acc_Speed  = (float)(4*Axis_Z.Target_Fre-4*Z_STAR_FREQ)/(2*Z_Acc_Time_M);
				Axis_Z.Jerk_Speed = (float)(8*Axis_Z.Target_Fre-8*Z_STAR_FREQ)/(2*Z_Acc_Time_M*Z_Acc_Time_M);
				
        Axis_Z.NowFreq = Z_STAR_FREQ;    //��ó�ʼƵ�ʣ��Խ����ⲿ�ж�
        Axis_Z.NowPulse = 0;    //���㵱ǰ������
        Axis_Z.Timer_Cnt = 0;   //���㵱ǰ��ʱ��
        for(i=0;i<7;i++)        //���㵱ǰʱ��ڵ㣬����ڵ�
        {
          Axis_Z.PulsePoint[i] = 0;
          Axis_Z.TimerPoint[i] = 0;
          Axis_Z.FreqPoint[i] = 0;
        }
        Axis_Z.InCtrl_Flag = 2;//�Ӽ���ģʽ
        Axis_Z.SpeedChange_Stage = 1;
			}
			
			//�ж��Ƿ�Ϊ����ģʽ
			else
			{
				Axis_Z.Target_Fre = Z_SPEED_FREQ_H;
				Axis_Z.Acc_Speed  = (float)(4*Axis_Z.Target_Fre-4*Z_STAR_FREQ)/(2*Z_Acc_Time_H);
				Axis_Z.Jerk_Speed = (float)(8*Axis_Z.Target_Fre-8*Z_STAR_FREQ)/(2*Z_Acc_Time_H*Z_Acc_Time_H);
				
        Axis_Z.NowFreq = Z_STAR_FREQ;    //��ó�ʼƵ�ʣ��Խ����ⲿ�ж�
        Axis_Z.NowPulse = 0;    //���㵱ǰ������
        Axis_Z.Timer_Cnt = 0;   //���㵱ǰ��ʱ��
        for(i=0;i<7;i++)        //���㵱ǰʱ��ڵ㣬����ڵ�
        {
          Axis_Z.PulsePoint[i] = 0;
          Axis_Z.TimerPoint[i] = 0;
          Axis_Z.FreqPoint[i] = 0;
        }
        Axis_Z.InCtrl_Flag = 2;//�Ӽ���ģʽ
        Axis_Z.SpeedChange_Stage = 1;
      }
		}

	}
	else
	{
		DIR_Y = Dir;	
		Axis_Y.Dir = Dir;
    if(Axis_Y.InCtrl_Flag == 0)
    {
			Axis_Y.Target_Pulse = Target;
			//���üӼ��ٲ������ֱ�ΪĿ��Ƶ�ʣ��Ӽ��ٶȣ��ȼ��ټ��ٶ�
			Axis_Y.Target_Fre = Y_SPEED_FREQ;
			Axis_Y.Acc_Speed  = (float)(4*Axis_Y.Target_Fre-4*Y_STAR_FREQ)/(2*Y_ACC_TIME);
			Axis_Y.Jerk_Speed = (float)(8*Axis_Y.Target_Fre-8*Y_STAR_FREQ)/(2*Y_ACC_TIME*Y_ACC_TIME);	
			Axis_Y.NowFreq = Y_STAR_FREQ;    //��ó�ʼƵ�ʣ��Խ����ⲿ�ж�
      Axis_Y.NowPulse = 0;    //���㵱ǰ������
      Axis_Y.Timer_Cnt = 0;   //���㵱ǰ��ʱ��
      for(i=0;i<7;i++)        //���㵱ǰʱ��ڵ㣬����ڵ�
      {
        Axis_Y.PulsePoint[i] = 0;
				Axis_Y.TimerPoint[i] = 0;
				Axis_Y.FreqPoint[i] = 0;
			}
			Axis_Y.InCtrl_Flag = 2;//�Ӽ���ģʽ
			Axis_Y.SpeedChange_Stage = 1; 
    }
	}
}

void Motor_BackToZero(u8 Axis)
{
	if(Axis == AXIS_X)
	{
		if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1||A_SENSOR == 1||B_SENSOR == 1)
		{
			return;
		}
		DIR_X = X_DIR_BACK;
		Axis_X.Dir = X_DIR_BACK;
		Axis_X.NowPulse = 0;
		Axis_X.Target_Pulse = 50000;
		Axis_X.InCtrl_Flag = 3;//��λģʽ
	}
	else
	{
		if(Proximity_1 == 1||Proximity_2 == 1||Proximity_3 == 1||A_SENSOR == 1||B_SENSOR == 1)
		{
			return;
		}
			DIR_Z = Z_DIR_FRONT;
			Axis_Z.Dir = Z_DIR_FRONT;
			Axis_Z.Target_Pulse = Z_ZeroUp_Pluse;
			Axis_Z.NowPulse = 0;
			Axis_Z.InCtrl_Flag = 3;//��λģʽ
			while(Axis_Z.InCtrl_Flag)
			{
				Uart_Analysis();//���ڽ���
				if(Axis_Z_Up_Sensor == 0&&Axis_Z.Dir == Z_DIR_FRONT)
				{
					
					Axis_Z.InCtrl_Flag = 0;
					TIM_Cmd(TIM4, DISABLE);
					Action_Stop();
					Beep(50,50,1);
				}

			}
			delay_ms(500);
			DIR_Z = Z_DIR_BACK;
			Axis_Z.Dir = Z_DIR_BACK;
			Axis_Z.NowPulse = 0;
			Axis_Z.Target_Pulse = 50000;
			Axis_Z.InCtrl_Flag = 3;//��λģʽ
		
	}
}


void EXTI1_IRQHandler(void)//X��������
{
	u8 i;
	EXTI_ClearITPendingBit(EXTI_Line1);
	if(Axis_X.InCtrl_Flag!=0)
  {
    Axis_X.NowPulse++;
		if(Axis_X.Dir == X_DIR_FRONT)
		{
			Axis_X.Coordinate++;
		}
		else 
		{
			if(Axis_X.Coordinate>0)Axis_X.Coordinate--;
		}
		if((Axis_X_Forward_Sensor == 0)&&(Axis_X.Dir == X_DIR_BACK))
		{
			x_stop_flag = 1;
			Axis_X.InCtrl_Flag = 0;
			TIM_Cmd(TIM2, DISABLE);
		}
    if(Axis_X.NowPulse >= Axis_X.Target_Pulse)
    {
      TIM_Cmd(TIM2, DISABLE);
			if(Axis_Z.InCtrl_Flag == 0)//Z���Ѿ�������ɣ����ϱ������������
			{
				Motor_CtrlOver_Flag = 2;
			}
			
      Axis_X.InCtrl_Flag = 0;
			
      Axis_X.NowPulse = 0;
      Axis_X.NowFreq = 0;
      Axis_X.NowACC = 0;
      Axis_X.Target_Pulse = 0;
      Axis_X.Timer_Cnt = 0;
      Axis_X.SpeedChange_Stage = 0;
			Axis_X.Time_Delta = 0;
			Axis_X.Target_Fre = 0;
			Axis_X.Jerk_Speed = 0;
			Axis_X.Acc_Speed = 0;
			/*if(Axis_X.InCtrl_Flag == 0&&Action.Move_Alone == 1)
			{
				Action.Move_Alone = 0;
				Current_Position_Buff[0] = ((Axis_X.Coordinate*X_MPP)+0.5)/256;
				Current_Position_Buff[1] = (u8)(((Axis_X.Coordinate*X_MPP)+0.5)/1)%256;
				Current_Position_Buff[2] = ((Axis_Z.Coordinate*Z_MPP)+0.5)/256;
				Current_Position_Buff[3] =	(u8)(((Axis_Z.Coordinate*Z_MPP)+0.5)/1)%256;
				if(Axis_Y.YCoordinate<0)	
				Current_Position_Buff[4] = 1;
				else
				Current_Position_Buff[4] = 0;
				Current_Position_Buff[5] = (u8)(((Axis_Y.YCoordinate*Y_MPP)+0.5)/1)%256;
				if(Action.Step_Grade!=1000)
				{
					Current_Position_Buff[6] = Action.Step_Grade+1;//���ز�����λ
				}
				else
				{
					Current_Position_Buff[6] = 0xf0;//���ز�����λ
				}
				Current_Position_Buff[7] = Action.Speed_Grade;//�����ٶȵ�λ
				Uart_Send(1,CMD_QUERYSTATE,Local_Ip,Prot_Cfg,0,Current_Position_Buff);
			}*/
      for(i=0;i<7;i++)
      {
        Axis_X.PulsePoint[i] = 0;
        Axis_X.TimerPoint[i] = 0;
        Axis_X.FreqPoint[i] = 0;
      }
		}
	}
}

void EXTI0_IRQHandler(void)//Y��������
{
	u8 i;
	EXTI_ClearITPendingBit(EXTI_Line0);
	if(Axis_Y.InCtrl_Flag!=0)
  {
    Axis_Y.NowPulse++;
		if(Axis_Y.Dir == Y_DIR_LEFT)
		{
			Axis_Y.YCoordinate++;
		}
		else 
		{
			Axis_Y.YCoordinate--;
		}
    if(Axis_Y.NowPulse >= Axis_Y.Target_Pulse)
    {
      TIM_Cmd(TIM3, DISABLE);
      Axis_Y.InCtrl_Flag = 0;
      Axis_Y.NowPulse = 0;
      Axis_Y.NowFreq = 0;
      Axis_Y.NowACC = 0;
      Axis_Y.Target_Pulse = 0;
      Axis_Y.Timer_Cnt = 0;
      Axis_Y.SpeedChange_Stage = 0;
			Axis_Y.Time_Delta = 0;
			Axis_Y.Target_Fre = 0;
			Axis_Y.Jerk_Speed = 0;
			Axis_Y.Acc_Speed = 0;
//			if(Axis_Y.InCtrl_Flag == 0&&Action.Move_Alone == 1)
//			{
//				Action.Move_Alone = 0;
//				Current_Position_Buff[0] = ((Axis_X.Coordinate*X_MPP)+0.5)/256;
//				Current_Position_Buff[1] = (u8)(((Axis_X.Coordinate*X_MPP)+0.5)/1)%256;
//				Current_Position_Buff[2] = ((Axis_Z.Coordinate*Z_MPP)+0.5)/256;
//				Current_Position_Buff[3] =	(u8)(((Axis_Z.Coordinate*Z_MPP)+0.5)/1)%256;
//				if(Axis_Y.YCoordinate<0)	
//				Current_Position_Buff[4] = 1;
//				else
//				Current_Position_Buff[4] = 0;
//				Current_Position_Buff[5] = (u8)(((Axis_Y.YCoordinate*Y_MPP)+0.5)/1)%256;
//				if(Action.Step_Grade!=1000)
//				{
//					Current_Position_Buff[6] = Action.Step_Grade+1;//���ز�����λ
//				}
//				else
//				{
//					Current_Position_Buff[6] = 0xf0;//���ز�����λ
//				}
//				Current_Position_Buff[7] = Action.Speed_Grade;//�����ٶȵ�λ
//				Uart_Send(1,CMD_QUERYSTATE,Local_Ip,Prot_Cfg,0,Current_Position_Buff);
//			}
      for(i=0;i<7;i++)
      {
        Axis_Y.PulsePoint[i] = 0;
        Axis_Y.TimerPoint[i] = 0;
        Axis_Y.FreqPoint[i] = 0;
      }
    }
  }

}

void EXTI4_IRQHandler(void)//Z��������
{
	u8 i;
	EXTI_ClearITPendingBit(EXTI_Line4);
	  if(Axis_Z.InCtrl_Flag!=0)
  {
    Axis_Z.NowPulse++;
		if(Axis_Z.Dir == Z_DIR_FRONT)
		{
			Axis_Z.Coordinate++;
		}
		else 
		{
			if(Axis_Z.Coordinate>0)Axis_Z.Coordinate--;
		}
		if(Axis_Z_Down_Sensor == 0&&Axis_Z.Dir == Z_DIR_BACK)
		{			
			Axis_Z.InCtrl_Flag = 0;
			Axis_Z.Coordinate = 0;
			XYZ_To_Zero.Z_Return_Flag = 1;
			//Auto_AdjustZ.Back_Zero_Flag = 1;
			TIM_Cmd(TIM4, DISABLE);
		}
    if(Axis_Z.NowPulse >= Axis_Z.Target_Pulse)
    {
      TIM_Cmd(TIM4, DISABLE);
			if(Axis_X.InCtrl_Flag == 0)//X���Ѿ�������ɣ����ϱ������������
			{
				Motor_CtrlOver_Flag = 2;
			}
      Axis_Z.InCtrl_Flag = 0;
			Axis_Z.NowACC = 0;
      Axis_Z.NowPulse = 0;
      Axis_Z.NowFreq = 0;   
      Axis_Z.Target_Pulse = 0;
      Axis_Z.Timer_Cnt = 0;
      Axis_Z.SpeedChange_Stage = 0;
			Axis_Z.Time_Delta = 0;
			Axis_Z.Target_Fre = 0;
			Axis_Z.Jerk_Speed = 0;
			Axis_Z.Acc_Speed = 0;
//			if(Axis_Z.InCtrl_Flag == 0&&Action.Move_Alone == 1)
//			{
//				Action.Move_Alone = 0;
//				Current_Position_Buff[0] = ((Axis_X.Coordinate*X_MPP)+0.5)/256;
//				Current_Position_Buff[1] = (u8)(((Axis_X.Coordinate*X_MPP)+0.5)/1)%256;
//				Current_Position_Buff[2] = ((Axis_Z.Coordinate*Z_MPP)+0.5)/256;
//				Current_Position_Buff[3] =	(u8)(((Axis_Z.Coordinate*Z_MPP)+0.5)/1)%256;
//				if(Axis_Y.YCoordinate<0)	
//				Current_Position_Buff[4] = 1;
//				else
//				Current_Position_Buff[4] = 0;
//				Current_Position_Buff[5] = (u8)(((Axis_Y.YCoordinate*Y_MPP)+0.5)/1)%256;
//				if(Action.Step_Grade!=1000)
//				{
//					Current_Position_Buff[6] = Action.Step_Grade+1;//���ز�����λ
//				}
//				else
//				{
//					Current_Position_Buff[6] = 0xf0;//���ز�����λ
//				}
//				Current_Position_Buff[7] = Action.Speed_Grade;//�����ٶȵ�λ
//				Uart_Send(1,CMD_QUERYSTATE,Local_Ip,Prot_Cfg,0,Current_Position_Buff);
//			}
      for(i=0;i<7;i++)
      {
        Axis_Z.PulsePoint[i] = 0;
        Axis_Z.TimerPoint[i] = 0;
        Axis_Z.FreqPoint[i] = 0;
      }
    }
  }
	
	
}



