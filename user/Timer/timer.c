#include "timer.h"
#include "led.h"
#include "stm32f10x_tim.h"
#include <stdbool.h>
#include "network.h"


   	
/********变量定义*********/		
TYPE_ALARM Alarm_X;
TYPE_ALARM Alarm_Y;
TYPE_ALARM Alarm_Z;
u16 IC_RevTime;	
u16 ReadSensorTime_100ms;  //每100ms读取一次传感器状态计时器
u16 Timer_TestModeFlash;   //工程模式闪烁定时器    
u8  TestModeFlash_count;   //工程模式闪烁次数
u8 Timer_10s_flag;
u8 	Timer_5ms_flag;
u8 Timer_1ms_flag;
u8 Timer_5s_flag;
u8	test_add;//计算XYZ三轴电机运动状态
u16 DelayTimes_ms;
unsigned int W5500_Send_Delay_Counter=0; //W5500发送延时计数变量(ms)

/************************/
//通用定时器3中断初始化
//时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频值。
//Tout = ((arr+1) * (psc+1)) / Tclk 
//Tclk：TIM3的输入时钟频率(单位：Mhz)
//Tout：溢出时间(单位：us)
void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断
	
	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM5, ENABLE);  //使能TIMx		
}
//通用定时器2中断初始化
//时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频值。
//(((arr+1) * (7199+1)) / 72 )us
void TIM2_Int_Init(u16 arr,u16 psc)
{
	
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	
	//定时器TIM2初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断
	
	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级1级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM2, ENABLE);  //使能TIMx					 
}

//定时器3中断服务程序 /*1ms定时*/
void TIM5_IRQHandler(void)   //TIM3中断
{
// 	u32 Period_status;
	u32 status;
	
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{	
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);  			//清除TIMx更新中断标志
		/*********************************************************************************************************/
		/*公用*/
		//工程模式核心板绿灯控制----------------------------
		if(TEST_MODE != 0)       //判断闪烁次数
		{
			if(TEST_MODE_SAVE != TEST_MODE) //如果有模式切换，绿灯闪烁 
			{
				TestModeFlash_count = TEST_MODE-1;  //TestModeFlash_count：闪烁次数
				Timer_TestModeFlash = 0;   //清零闪烁定时器，从0开始
			}
			TEST_MODE_SAVE = TEST_MODE;  //保存当前模式，用来分辨模式切换
			//-------------------------------------
			if(TestModeFlash_count)  //执行闪烁
			{
				Timer_TestModeFlash++;
				if(Timer_TestModeFlash < 200)
				{
					LEDH;
				}
				else if(Timer_TestModeFlash < 500)
				{
					LEDL;
				}
				else 
				{
					Timer_TestModeFlash = 0;
					TestModeFlash_count--;
				}
			}
		}		
		Timer_1ms_flag= 1;
		
				/************X轴步进电机控制************/
    if(Axis_X.InCtrl_Flag == 2) //高速模式
    {
      Axis_X.Timer_Cnt++;
			switch (Axis_X.SpeedChange_Stage)
			{
      /*********加速阶段*******/
				case 1:
				{
					/*********加加速阶段*******/
					//公式 V1=V0+1/2ht^2
					if(Axis_X.NowACC < Axis_X.Acc_Speed)   //当前加速度小于匀加速加速度
					{
						Axis_X.PulsePoint[0] = Axis_X.NowPulse;  //记录当前脉冲数
						Axis_X.TimerPoint[0] = Axis_X.Timer_Cnt; //记录当前时间点
						/**********************************************************************/
						//计算当前加速度 加加速度*时间
						Axis_X.NowACC = Axis_X.Timer_Cnt*Axis_X.Jerk_Speed;    
						//计算当前频率
						Axis_X.NowFreq = X_STAR_FREQ+Axis_X.NowACC*Axis_X.Timer_Cnt/2;//计算当前速度 加速度*时间/2
						//记录频率
						Axis_X.FreqPoint[0] = Axis_X.NowFreq;
					}
					else 
					{
						Axis_X.SpeedChange_Stage++;
						//Axis_X.Stage1_Acc = Axis_X.NowACC;
					}
				}break;
      /*********减加速阶段*******/
      //公式 V3=V2+a(t-t1)-1/2h(t-t2)^2
				case 2:
				{
					if(Axis_X.NowACC > 0)
					{
						Axis_X.PulsePoint[1] = Axis_X.NowPulse;  //记录当前脉冲数
						Axis_X.TimerPoint[1] = Axis_X.Timer_Cnt; //记录当前时间点
						Axis_X.Time_Delta = Axis_X.Timer_Cnt-Axis_X.TimerPoint[0];//计算ΔT
						/**********************************************************************/
						//计算当前加速度
						Axis_X.NowACC = Axis_X.Acc_Speed - (Axis_X.Time_Delta*Axis_X.Jerk_Speed); 
						//计算当前频率
						Axis_X.NowFreq = Axis_X.FreqPoint[0] + (Axis_X.Acc_Speed*Axis_X.Time_Delta)
														 - (((Axis_X.Jerk_Speed*Axis_X.Time_Delta)*Axis_X.Time_Delta)/2);
						//记录当前频率
						Axis_X.FreqPoint[1] = Axis_X.NowFreq;
					}
					else 
					{
						Axis_X.SpeedChange_Stage++;
					}
				}break;
      /*********匀速阶段*******/
				case 3:
				{
					if(Axis_X.NowPulse < (Axis_X.Target_Pulse - Axis_X.PulsePoint[1]))
					{
						Axis_X.PulsePoint[2] = Axis_X.NowPulse;  //记录当前脉冲数
						Axis_X.TimerPoint[2] = Axis_X.Timer_Cnt; //记录当前时间点
						/**********************************************************************/
						Axis_X.NowFreq = Axis_X.Target_Fre;//Axis_X.FreqPoint[1];
						//记录当前频率
						Axis_X.FreqPoint[2] = Axis_X.NowFreq;
					}
					else 
					{
						Axis_X.SpeedChange_Stage++;
					}
				}break;
      /*********加减速阶段*******/
      //公式 V5=V4-1/2h(t-t4)^2
				case 4:
				{
					if(Axis_X.NowACC < Axis_X.Acc_Speed)
					{
						Axis_X.PulsePoint[3] = Axis_X.NowPulse;  //记录当前脉冲数
						Axis_X.TimerPoint[3] = Axis_X.Timer_Cnt; //记录当前时间点
						Axis_X.Time_Delta = Axis_X.Timer_Cnt-Axis_X.TimerPoint[2];//计算ΔT
						/**********************************************************************/
						Axis_X.NowACC = Axis_X.Time_Delta*Axis_X.Jerk_Speed;   //计算当前加速度 加加速度*时间
						Axis_X.NowFreq = Axis_X.FreqPoint[2] - Axis_X.NowACC*Axis_X.Time_Delta/2;
						//记录当前频率
						Axis_X.FreqPoint[3] = Axis_X.NowFreq;
					}
					else 
					{
						Axis_X.SpeedChange_Stage++;
						//Axis_X.Stage4_Acc = Axis_X.NowACC;
					}
				}break;
      /*********加减速阶段*******/
      //公式 V7=V6-a(t-t6)+1/2*h(t-t6)^2
				case 5:
				{
					if(Axis_X.NowFreq > X_END_FREQ)
					{
						Axis_X.PulsePoint[4] = Axis_X.NowPulse;  //记录当前脉冲数
						Axis_X.TimerPoint[4] = Axis_X.Timer_Cnt; //记录当前时间点
						Axis_X.Time_Delta = Axis_X.Timer_Cnt-Axis_X.TimerPoint[3];//计算ΔT
						//计算加速度
						Axis_X.NowACC = Axis_X.Acc_Speed - (Axis_X.Time_Delta*Axis_X.Jerk_Speed) ;   //计算当前加速度 加加速度*时间
						Axis_X.NowFreq = Axis_X.FreqPoint[3] - 
														 ((Axis_X.Acc_Speed*Axis_X.Time_Delta)- (((Axis_X.Jerk_Speed*Axis_X.Time_Delta)*Axis_X.Time_Delta)/2));
					}
					else 
					{
						Axis_X.SpeedChange_Stage++;
					}
				}break;    
				case 6:
				{
					Axis_X.NowFreq = X_END_FREQ;
				}break;
			}
      status = PWMCtrl_ARRCount(PWM_PCS,Axis_X.NowFreq);
      if(status > 999 && status < 63157)	//频率在95~6006HZ之间 63157
      {
				TIM_Cmd(TIM2, ENABLE);
				PWM_Set_X(status,PWM_PCS);
      }
    }
    else if(Axis_X.InCtrl_Flag == 1)//工程师模式调节
    {
      Axis_X.Target_Fre = 50+50*Action.Speed_Grade;
			Axis_X.Target_Pulse = 10+10*Action.Step_Grade;
      status = PWMCtrl_ARRCount(PWM_PCS,Axis_X.Target_Fre);
      PWM_Set_X(status,PWM_PCS);
			TIM_Cmd(TIM2, ENABLE);
    }
		else if(Axis_X.InCtrl_Flag == 3)//归位模式
		{
			Axis_X.Target_Fre = 100;
      status = PWMCtrl_ARRCount(PWM_PCS,Axis_X.Target_Fre);
      PWM_Set_X(status,PWM_PCS);
			TIM_Cmd(TIM2, ENABLE);
		}

    else
    {
      
    }
		
		 		/************Z轴步进电机控制************/
    if(Axis_Z.InCtrl_Flag == 2) //高速模式
    {
      Axis_Z.Timer_Cnt++;
      /*********加速阶段*******/
			switch (Axis_Z.SpeedChange_Stage)
			{
				case 1:
				{
					/*********加加速阶段*******/
					//公式 V1=V0+1/2ht^2
					if(Axis_Z.NowACC < Axis_Z.Acc_Speed)   //当前加速度小于匀加速加速度
					{
						Axis_Z.PulsePoint[0] = Axis_Z.NowPulse;  //记录当前脉冲数
						Axis_Z.TimerPoint[0] = Axis_Z.Timer_Cnt; //记录当前时间点
						/**********************************************************************/
						//计算当前加速度 加加速度*时间
						Axis_Z.NowACC = Axis_Z.Timer_Cnt*Axis_Z.Jerk_Speed;    
						//计算当前频率
						Axis_Z.NowFreq = Z_STAR_FREQ+Axis_Z.NowACC*Axis_Z.Timer_Cnt/2;//计算当前速度 加速度*时间/2
						//记录频率
						Axis_Z.FreqPoint[0] = Axis_Z.NowFreq;
					}
					else 
					{
						Axis_Z.SpeedChange_Stage++;
					}
				}break;

      /*********减加速阶段*******/
      //公式 V3=V2+a(t-t1)-1/2h(t-t2)^2
				case 2:
				{
					if(Axis_Z.NowACC > 0 )//&& (Axis_Z.NowFreq <Axis_Z.Target_Fre) &&(Axis_Z.Timer_Cnt<500)
					{
						Axis_Z.PulsePoint[1] = Axis_Z.NowPulse;  //记录当前脉冲数
						Axis_Z.TimerPoint[1] = Axis_Z.Timer_Cnt; //记录当前时间点
						Axis_Z.Time_Delta = Axis_Z.Timer_Cnt-Axis_Z.TimerPoint[0];//计算ΔT
						/**********************************************************************/
						//计算当前加速度
						Axis_Z.NowACC = Axis_Z.Acc_Speed - (Axis_Z.Time_Delta*Axis_Z.Jerk_Speed); 
						//计算当前频率
						Axis_Z.NowFreq = Axis_Z.FreqPoint[0] + (Axis_Z.Acc_Speed*Axis_Z.Time_Delta)
														 - (((Axis_Z.Jerk_Speed*Axis_Z.Time_Delta)*Axis_Z.Time_Delta)/2);
						//记录当前频率
						Axis_Z.FreqPoint[1] = Axis_Z.NowFreq;
					}
					else 
					{
						Axis_Z.SpeedChange_Stage++;
					}
				}break;
      /*********匀速阶段*******/
				case 3:
				{
					if(Axis_Z.NowPulse < (Axis_Z.Target_Pulse - Axis_Z.PulsePoint[1]))
					{
						Axis_Z.PulsePoint[2] = Axis_Z.NowPulse;  //记录当前脉冲数
						Axis_Z.TimerPoint[2] = Axis_Z.Timer_Cnt; //记录当前时间点
						/**********************************************************************/
						Axis_Z.NowFreq = Axis_Z.Target_Fre;
						//记录当前频率
						Axis_Z.FreqPoint[2] = Axis_Z.NowFreq;
					}
					else 
					{
						Axis_Z.SpeedChange_Stage++;
					}
				}break;
      /*********加减速阶段*******/
      //公式 V5=V4-1/2h(t-t4)^2
				case 4:
				{
					if(Axis_Z.NowACC < Axis_Z.Acc_Speed)
					{
						Axis_Z.PulsePoint[3] = Axis_Z.NowPulse;  //记录当前脉冲数
						Axis_Z.TimerPoint[3] = Axis_Z.Timer_Cnt; //记录当前时间点
						Axis_Z.Time_Delta = Axis_Z.Timer_Cnt-Axis_Z.TimerPoint[2];//计算ΔT
						/**********************************************************************/
						Axis_Z.NowACC = Axis_Z.Time_Delta*Axis_Z.Jerk_Speed;   //计算当前加速度 加加速度*时间
						Axis_Z.NowFreq = Axis_Z.FreqPoint[2] - Axis_Z.NowACC*Axis_Z.Time_Delta/2;
						//记录当前频率
						Axis_Z.FreqPoint[3] = Axis_Z.NowFreq;
					}
					else 
					{
						Axis_Z.SpeedChange_Stage++;
					}
				}break;
 
      /*********加减速阶段*******/
      //公式 V7=V6-a(t-t6)+1/2*h(t-t6)^2
				case 5:
				{
					if(Axis_Z.NowFreq > Z_END_FREQ)
					{
						Axis_Z.PulsePoint[4] = Axis_Z.NowPulse;  //记录当前脉冲数
						Axis_Z.TimerPoint[4] = Axis_Z.Timer_Cnt; //记录当前时间点
						Axis_Z.Time_Delta = Axis_Z.Timer_Cnt-Axis_Z.TimerPoint[3];//计算ΔT
						//计算加速度
						Axis_Z.NowACC = Axis_Z.Acc_Speed - (Axis_Z.Time_Delta*Axis_Z.Jerk_Speed) ;   //计算当前加速度 加加速度*时间
						Axis_Z.NowFreq = Axis_Z.FreqPoint[3] - 
														 ((Axis_Z.Acc_Speed*Axis_Z.Time_Delta)- (((Axis_Z.Jerk_Speed*Axis_Z.Time_Delta)*Axis_Z.Time_Delta)/2));
					}
					else 
					{
						Axis_Z.SpeedChange_Stage++;
					}
				}break;
				case 6:
				{
					Axis_Z.NowFreq = Z_END_FREQ;
				}break;
			}
      status = PWMCtrl_ARRCount(PWM_PCS,Axis_Z.NowFreq);
      if(status > 999 && status < 63157)	//频率在95~6006HZ之间 63157
      {
				PWM_Set_Z(status,PWM_PCS);
				TIM_Cmd(TIM4, ENABLE);
      }
    }
    else if(Axis_Z.InCtrl_Flag == 1)//工程师模式调节
    {
      Axis_Z.Target_Fre = 50+50*Action.Speed_Grade;
			Axis_Z.Target_Pulse = 50+10*Action.Step_Grade;
      status = PWMCtrl_ARRCount(PWM_PCS,Axis_Z.Target_Fre);
      PWM_Set_Z(status,PWM_PCS);
			TIM_Cmd(TIM4, ENABLE);
    }
		else if(Axis_Z.InCtrl_Flag == 3)//归位模式
		{
			Axis_Z.Target_Fre = 80;
      status = PWMCtrl_ARRCount(PWM_PCS,Axis_Z.Target_Fre);
      PWM_Set_Z(status,PWM_PCS);
			TIM_Cmd(TIM4, ENABLE);
		}
    else
    {
      
    }
				/************Y轴步进电机控制************/
		if(Axis_Y.InCtrl_Flag == 2) //高速模式
    {
      Axis_Y.Timer_Cnt++;
      /*********加速阶段*******/
      if(Axis_Y.SpeedChange_Stage == 1)
      {
        /*********加加速阶段*******/
        //公式 V1=V0+1/2ht^2
        if(Axis_Y.NowACC < Axis_Y.Acc_Speed)   //当前加速度小于匀加速加速度
        {
          Axis_Y.PulsePoint[0] = Axis_Y.NowPulse;  //记录当前脉冲数
          Axis_Y.TimerPoint[0] = Axis_Y.Timer_Cnt; //记录当前时间点
          /**********************************************************************/
          //计算当前加速度 加加速度*时间
          Axis_Y.NowACC = Axis_Y.Timer_Cnt*Axis_Y.Jerk_Speed;    
          //计算当前频率
          Axis_Y.NowFreq = Y_STAR_FREQ+Axis_Y.NowACC*Axis_Y.Timer_Cnt/2;//计算当前速度 加速度*时间/2
          //记录频率
          Axis_Y.FreqPoint[0] = Axis_Y.NowFreq;
        }
        else 
        {
          Axis_Y.SpeedChange_Stage++;
        }
      }

      /*********减加速阶段*******/
      //公式 V3=V2+a(t-t1)-1/2h(t-t2)^2
      else if(Axis_Y.SpeedChange_Stage == 2)
      {
        if((Axis_Y.NowACC > 0) && (Axis_Y.NowFreq <Axis_Y.Target_Fre) &&(Axis_Y.Timer_Cnt<500))
        {
          Axis_Y.PulsePoint[1] = Axis_Y.NowPulse;  //记录当前脉冲数
          Axis_Y.TimerPoint[1] = Axis_Y.Timer_Cnt; //记录当前时间点
          Axis_Y.Time_Delta = Axis_Y.Timer_Cnt-Axis_Y.TimerPoint[0];//计算ΔT
          /**********************************************************************/
          //计算当前加速度
          Axis_Y.NowACC = Axis_Y.Acc_Speed - (Axis_Y.Time_Delta*Axis_Y.Jerk_Speed); 
          //计算当前频率
          Axis_Y.NowFreq = Axis_Y.FreqPoint[0] + (Axis_Y.Acc_Speed*Axis_Y.Time_Delta)
                           - (((Axis_Y.Jerk_Speed*Axis_Y.Time_Delta)*Axis_Y.Time_Delta)/2);
          //记录当前频率
          Axis_Y.FreqPoint[1] = Axis_Y.NowFreq;
        }
        else 
        {
          Axis_Y.SpeedChange_Stage++;
        }
      }
      /*********匀速阶段*******/
      else if(Axis_Y.SpeedChange_Stage == 3)
      {
        if(Axis_Y.NowPulse < (Axis_Y.Target_Pulse - Axis_Y.PulsePoint[1]))
        {
          Axis_Y.PulsePoint[2] = Axis_Y.NowPulse;  //记录当前脉冲数
          Axis_Y.TimerPoint[2] = Axis_Y.Timer_Cnt; //记录当前时间点
          /**********************************************************************/
          Axis_Y.NowFreq = Axis_Y.Target_Fre;
          //记录当前频率
          Axis_Y.FreqPoint[2] = Axis_Y.NowFreq;
        }
        else 
        {
          Axis_Y.SpeedChange_Stage++;
        }
      }
      /*********加减速阶段*******/
      //公式 V5=V4-1/2h(t-t4)^2
      else if(Axis_Y.SpeedChange_Stage == 4)
      {
        if(Axis_Y.NowACC < Axis_Y.Acc_Speed)
        {
          Axis_Y.PulsePoint[3] = Axis_Y.NowPulse;  //记录当前脉冲数
          Axis_Y.TimerPoint[3] = Axis_Y.Timer_Cnt; //记录当前时间点
          Axis_Y.Time_Delta = Axis_Y.Timer_Cnt-Axis_Y.TimerPoint[2];//计算ΔT
          /**********************************************************************/
          Axis_Y.NowACC = Axis_Y.Time_Delta*Axis_Y.Jerk_Speed;   //计算当前加速度 加加速度*时间
          Axis_Y.NowFreq = Axis_Y.FreqPoint[2] - Axis_Y.NowACC*Axis_Y.Time_Delta/2;
          //记录当前频率
          Axis_Y.FreqPoint[3] = Axis_Y.NowFreq;
        }
        else 
        {
          Axis_Y.SpeedChange_Stage++;
        }
      }
 
      /*********加减速阶段*******/
      //公式 V7=V6-a(t-t6)+1/2*h(t-t6)^2
      else if(Axis_Y.SpeedChange_Stage == 5)
      {
        if(Axis_Y.NowFreq > Y_END_FREQ)
        {
          Axis_Y.PulsePoint[4] = Axis_Y.NowPulse;  //记录当前脉冲数
          Axis_Y.TimerPoint[4] = Axis_Y.Timer_Cnt; //记录当前时间点
          Axis_Y.Time_Delta = Axis_Y.Timer_Cnt-Axis_Y.TimerPoint[3];//计算ΔT
          //计算加速度
          Axis_Y.NowACC = Axis_Y.Acc_Speed - (Axis_Y.Time_Delta*Axis_Y.Jerk_Speed) ;   //计算当前加速度 加加速度*时间
          Axis_Y.NowFreq = Axis_Y.FreqPoint[3] - 
                           ((Axis_Y.Acc_Speed*Axis_Y.Time_Delta)- (((Axis_Y.Jerk_Speed*Axis_Y.Time_Delta)*Axis_Y.Time_Delta)/2));
        }
        else 
        {
          Axis_Y.SpeedChange_Stage++;
        }
      }
      else 
      {
        Axis_Y.NowFreq = Y_END_FREQ;
      }
      status = PWMCtrl_ARRCount(PWM_PCS,Axis_Y.NowFreq);
      if(status > 999 && status < 63157)	//频率在95~6006HZ之间 63157
      {
				PWM_Set_Y(status,PWM_PCS);
				TIM_Cmd(TIM3, ENABLE);
      }
    }
		else if(Axis_Y.InCtrl_Flag == 1)//工程师模式调节
		{
			Axis_Y.Target_Fre = 60+50*Action.Speed_Grade;
			Axis_Y.Target_Pulse = 50+10*Action.Step_Grade;
      
			
			status = PWMCtrl_ARRCount(PWM_PCS,Axis_Y.Target_Fre);
      PWM_Set_Y(status,PWM_PCS);
			TIM_Cmd(TIM3, ENABLE);
			
			XYZ_To_Zero.Tray_Reset_Flag = 0;//测试
		}	
		else if(Axis_Y.InCtrl_Flag == 3)//归位模式
		{
			Axis_Y.Target_Fre = 200;
      status = PWMCtrl_ARRCount(PWM_PCS,Axis_Y.Target_Fre);
      PWM_Set_Y(status,PWM_PCS);
			TIM_Cmd(TIM3, ENABLE);
		}
		else
		{
		}
		

/*****************报警测试代码************************/		
		if(ALARM_Y ==0 )
		{
			Alarm_Y.Timer_cnt++;
			if(Alarm_Y.Timer_cnt>1500)
			{
				Action_Stop();
				Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Torque_Alm,0);//返回报警信息	
				Alarm_Y.Timer_cnt = 0;
					PAout(5) = 1;
					Reset_Flag = 1;
					delay_ms(1500);
					delay_ms(1500);
				
			}
		}
		else
		{
			Alarm_Y.Timer_cnt = 0;
		}
/*****************报警测试代码************************/			
			IWDG_Feed();//看门狗，喂狗
//		}
		if(DelayTimes_ms)DelayTimes_ms--;
	  test_add = Axis_X.InCtrl_Flag+Axis_Y.InCtrl_Flag+Axis_Z.InCtrl_Flag;//计算三轴电机运动状态，用于判断运动过程中是否接受状态

		//-------------------------------------------------
		if(Enable_Uart3_Flg)
		{
			Timer_5ms_flag++;
			if(Timer_5ms_flag>5)
			{
				USART_Cmd(USART3, ENABLE);
				Enable_Uart3_Flg = 0;
				Timer_5ms_flag = 0;
			}
		}
		
		
		//读取传感器时间 1.1秒
		if(ReadSensorTime_100ms) ReadSensorTime_100ms--;
				
		//RFID定时
		if(IC_RevTime > 0)	IC_RevTime--;
		
    //模拟动作定时
		/*公用*/
		/*******************************************************************************************************************/
	}
}



//定时器2中断服务程序  /*250ms定时*/
void TIM2_IRQHandler(void)   //TIM2中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{	
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  			//清除TIMx更新中断标志
		
	}
}

