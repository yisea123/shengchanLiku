#include "timer.h"
#include "led.h"
#include "stm32f10x_tim.h"
#include <stdbool.h>
#include "network.h"


   	
/********��������*********/		
TYPE_ALARM Alarm_X;
TYPE_ALARM Alarm_Y;
TYPE_ALARM Alarm_Z;
u16 IC_RevTime;	
u16 ReadSensorTime_100ms;  //ÿ100ms��ȡһ�δ�����״̬��ʱ��
u16 Timer_TestModeFlash;   //����ģʽ��˸��ʱ��    
u8  TestModeFlash_count;   //����ģʽ��˸����
u8 Timer_10s_flag;
u8 	Timer_5ms_flag;
u8 Timer_1ms_flag;
u8 Timer_5s_flag;
u8	test_add;//����XYZ�������˶�״̬
u16 DelayTimes_ms;
unsigned int W5500_Send_Delay_Counter=0; //W5500������ʱ��������(ms)

/************************/
//ͨ�ö�ʱ��3�жϳ�ʼ��
//ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶֵ��
//Tout = ((arr+1) * (psc+1)) / Tclk 
//Tclk��TIM3������ʱ��Ƶ��(��λ��Mhz)
//Tout�����ʱ��(��λ��us)
void TIM3_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�
	
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM5, ENABLE);  //ʹ��TIMx		
}
//ͨ�ö�ʱ��2�жϳ�ʼ��
//ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶֵ��
//(((arr+1) * (7199+1)) / 72 )us
void TIM2_Int_Init(u16 arr,u16 psc)
{
	
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM2��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM2�ж�,��������ж�
	
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx					 
}

//��ʱ��3�жϷ������ /*1ms��ʱ*/
void TIM5_IRQHandler(void)   //TIM3�ж�
{
// 	u32 Period_status;
	u32 status;
	
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{	
		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);  			//���TIMx�����жϱ�־
		/*********************************************************************************************************/
		/*����*/
		//����ģʽ���İ��̵ƿ���----------------------------
		if(TEST_MODE != 0)       //�ж���˸����
		{
			if(TEST_MODE_SAVE != TEST_MODE) //�����ģʽ�л����̵���˸ 
			{
				TestModeFlash_count = TEST_MODE-1;  //TestModeFlash_count����˸����
				Timer_TestModeFlash = 0;   //������˸��ʱ������0��ʼ
			}
			TEST_MODE_SAVE = TEST_MODE;  //���浱ǰģʽ�������ֱ�ģʽ�л�
			//-------------------------------------
			if(TestModeFlash_count)  //ִ����˸
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
		
				/************X�Ჽ���������************/
    if(Axis_X.InCtrl_Flag == 2) //����ģʽ
    {
      Axis_X.Timer_Cnt++;
			switch (Axis_X.SpeedChange_Stage)
			{
      /*********���ٽ׶�*******/
				case 1:
				{
					/*********�Ӽ��ٽ׶�*******/
					//��ʽ V1=V0+1/2ht^2
					if(Axis_X.NowACC < Axis_X.Acc_Speed)   //��ǰ���ٶ�С���ȼ��ټ��ٶ�
					{
						Axis_X.PulsePoint[0] = Axis_X.NowPulse;  //��¼��ǰ������
						Axis_X.TimerPoint[0] = Axis_X.Timer_Cnt; //��¼��ǰʱ���
						/**********************************************************************/
						//���㵱ǰ���ٶ� �Ӽ��ٶ�*ʱ��
						Axis_X.NowACC = Axis_X.Timer_Cnt*Axis_X.Jerk_Speed;    
						//���㵱ǰƵ��
						Axis_X.NowFreq = X_STAR_FREQ+Axis_X.NowACC*Axis_X.Timer_Cnt/2;//���㵱ǰ�ٶ� ���ٶ�*ʱ��/2
						//��¼Ƶ��
						Axis_X.FreqPoint[0] = Axis_X.NowFreq;
					}
					else 
					{
						Axis_X.SpeedChange_Stage++;
						//Axis_X.Stage1_Acc = Axis_X.NowACC;
					}
				}break;
      /*********�����ٽ׶�*******/
      //��ʽ V3=V2+a(t-t1)-1/2h(t-t2)^2
				case 2:
				{
					if(Axis_X.NowACC > 0)
					{
						Axis_X.PulsePoint[1] = Axis_X.NowPulse;  //��¼��ǰ������
						Axis_X.TimerPoint[1] = Axis_X.Timer_Cnt; //��¼��ǰʱ���
						Axis_X.Time_Delta = Axis_X.Timer_Cnt-Axis_X.TimerPoint[0];//���㦤T
						/**********************************************************************/
						//���㵱ǰ���ٶ�
						Axis_X.NowACC = Axis_X.Acc_Speed - (Axis_X.Time_Delta*Axis_X.Jerk_Speed); 
						//���㵱ǰƵ��
						Axis_X.NowFreq = Axis_X.FreqPoint[0] + (Axis_X.Acc_Speed*Axis_X.Time_Delta)
														 - (((Axis_X.Jerk_Speed*Axis_X.Time_Delta)*Axis_X.Time_Delta)/2);
						//��¼��ǰƵ��
						Axis_X.FreqPoint[1] = Axis_X.NowFreq;
					}
					else 
					{
						Axis_X.SpeedChange_Stage++;
					}
				}break;
      /*********���ٽ׶�*******/
				case 3:
				{
					if(Axis_X.NowPulse < (Axis_X.Target_Pulse - Axis_X.PulsePoint[1]))
					{
						Axis_X.PulsePoint[2] = Axis_X.NowPulse;  //��¼��ǰ������
						Axis_X.TimerPoint[2] = Axis_X.Timer_Cnt; //��¼��ǰʱ���
						/**********************************************************************/
						Axis_X.NowFreq = Axis_X.Target_Fre;//Axis_X.FreqPoint[1];
						//��¼��ǰƵ��
						Axis_X.FreqPoint[2] = Axis_X.NowFreq;
					}
					else 
					{
						Axis_X.SpeedChange_Stage++;
					}
				}break;
      /*********�Ӽ��ٽ׶�*******/
      //��ʽ V5=V4-1/2h(t-t4)^2
				case 4:
				{
					if(Axis_X.NowACC < Axis_X.Acc_Speed)
					{
						Axis_X.PulsePoint[3] = Axis_X.NowPulse;  //��¼��ǰ������
						Axis_X.TimerPoint[3] = Axis_X.Timer_Cnt; //��¼��ǰʱ���
						Axis_X.Time_Delta = Axis_X.Timer_Cnt-Axis_X.TimerPoint[2];//���㦤T
						/**********************************************************************/
						Axis_X.NowACC = Axis_X.Time_Delta*Axis_X.Jerk_Speed;   //���㵱ǰ���ٶ� �Ӽ��ٶ�*ʱ��
						Axis_X.NowFreq = Axis_X.FreqPoint[2] - Axis_X.NowACC*Axis_X.Time_Delta/2;
						//��¼��ǰƵ��
						Axis_X.FreqPoint[3] = Axis_X.NowFreq;
					}
					else 
					{
						Axis_X.SpeedChange_Stage++;
						//Axis_X.Stage4_Acc = Axis_X.NowACC;
					}
				}break;
      /*********�Ӽ��ٽ׶�*******/
      //��ʽ V7=V6-a(t-t6)+1/2*h(t-t6)^2
				case 5:
				{
					if(Axis_X.NowFreq > X_END_FREQ)
					{
						Axis_X.PulsePoint[4] = Axis_X.NowPulse;  //��¼��ǰ������
						Axis_X.TimerPoint[4] = Axis_X.Timer_Cnt; //��¼��ǰʱ���
						Axis_X.Time_Delta = Axis_X.Timer_Cnt-Axis_X.TimerPoint[3];//���㦤T
						//������ٶ�
						Axis_X.NowACC = Axis_X.Acc_Speed - (Axis_X.Time_Delta*Axis_X.Jerk_Speed) ;   //���㵱ǰ���ٶ� �Ӽ��ٶ�*ʱ��
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
      if(status > 999 && status < 63157)	//Ƶ����95~6006HZ֮�� 63157
      {
				TIM_Cmd(TIM2, ENABLE);
				PWM_Set_X(status,PWM_PCS);
      }
    }
    else if(Axis_X.InCtrl_Flag == 1)//����ʦģʽ����
    {
      Axis_X.Target_Fre = 50+50*Action.Speed_Grade;
			Axis_X.Target_Pulse = 10+10*Action.Step_Grade;
      status = PWMCtrl_ARRCount(PWM_PCS,Axis_X.Target_Fre);
      PWM_Set_X(status,PWM_PCS);
			TIM_Cmd(TIM2, ENABLE);
    }
		else if(Axis_X.InCtrl_Flag == 3)//��λģʽ
		{
			Axis_X.Target_Fre = 100;
      status = PWMCtrl_ARRCount(PWM_PCS,Axis_X.Target_Fre);
      PWM_Set_X(status,PWM_PCS);
			TIM_Cmd(TIM2, ENABLE);
		}

    else
    {
      
    }
		
		 		/************Z�Ჽ���������************/
    if(Axis_Z.InCtrl_Flag == 2) //����ģʽ
    {
      Axis_Z.Timer_Cnt++;
      /*********���ٽ׶�*******/
			switch (Axis_Z.SpeedChange_Stage)
			{
				case 1:
				{
					/*********�Ӽ��ٽ׶�*******/
					//��ʽ V1=V0+1/2ht^2
					if(Axis_Z.NowACC < Axis_Z.Acc_Speed)   //��ǰ���ٶ�С���ȼ��ټ��ٶ�
					{
						Axis_Z.PulsePoint[0] = Axis_Z.NowPulse;  //��¼��ǰ������
						Axis_Z.TimerPoint[0] = Axis_Z.Timer_Cnt; //��¼��ǰʱ���
						/**********************************************************************/
						//���㵱ǰ���ٶ� �Ӽ��ٶ�*ʱ��
						Axis_Z.NowACC = Axis_Z.Timer_Cnt*Axis_Z.Jerk_Speed;    
						//���㵱ǰƵ��
						Axis_Z.NowFreq = Z_STAR_FREQ+Axis_Z.NowACC*Axis_Z.Timer_Cnt/2;//���㵱ǰ�ٶ� ���ٶ�*ʱ��/2
						//��¼Ƶ��
						Axis_Z.FreqPoint[0] = Axis_Z.NowFreq;
					}
					else 
					{
						Axis_Z.SpeedChange_Stage++;
					}
				}break;

      /*********�����ٽ׶�*******/
      //��ʽ V3=V2+a(t-t1)-1/2h(t-t2)^2
				case 2:
				{
					if(Axis_Z.NowACC > 0 )//&& (Axis_Z.NowFreq <Axis_Z.Target_Fre) &&(Axis_Z.Timer_Cnt<500)
					{
						Axis_Z.PulsePoint[1] = Axis_Z.NowPulse;  //��¼��ǰ������
						Axis_Z.TimerPoint[1] = Axis_Z.Timer_Cnt; //��¼��ǰʱ���
						Axis_Z.Time_Delta = Axis_Z.Timer_Cnt-Axis_Z.TimerPoint[0];//���㦤T
						/**********************************************************************/
						//���㵱ǰ���ٶ�
						Axis_Z.NowACC = Axis_Z.Acc_Speed - (Axis_Z.Time_Delta*Axis_Z.Jerk_Speed); 
						//���㵱ǰƵ��
						Axis_Z.NowFreq = Axis_Z.FreqPoint[0] + (Axis_Z.Acc_Speed*Axis_Z.Time_Delta)
														 - (((Axis_Z.Jerk_Speed*Axis_Z.Time_Delta)*Axis_Z.Time_Delta)/2);
						//��¼��ǰƵ��
						Axis_Z.FreqPoint[1] = Axis_Z.NowFreq;
					}
					else 
					{
						Axis_Z.SpeedChange_Stage++;
					}
				}break;
      /*********���ٽ׶�*******/
				case 3:
				{
					if(Axis_Z.NowPulse < (Axis_Z.Target_Pulse - Axis_Z.PulsePoint[1]))
					{
						Axis_Z.PulsePoint[2] = Axis_Z.NowPulse;  //��¼��ǰ������
						Axis_Z.TimerPoint[2] = Axis_Z.Timer_Cnt; //��¼��ǰʱ���
						/**********************************************************************/
						Axis_Z.NowFreq = Axis_Z.Target_Fre;
						//��¼��ǰƵ��
						Axis_Z.FreqPoint[2] = Axis_Z.NowFreq;
					}
					else 
					{
						Axis_Z.SpeedChange_Stage++;
					}
				}break;
      /*********�Ӽ��ٽ׶�*******/
      //��ʽ V5=V4-1/2h(t-t4)^2
				case 4:
				{
					if(Axis_Z.NowACC < Axis_Z.Acc_Speed)
					{
						Axis_Z.PulsePoint[3] = Axis_Z.NowPulse;  //��¼��ǰ������
						Axis_Z.TimerPoint[3] = Axis_Z.Timer_Cnt; //��¼��ǰʱ���
						Axis_Z.Time_Delta = Axis_Z.Timer_Cnt-Axis_Z.TimerPoint[2];//���㦤T
						/**********************************************************************/
						Axis_Z.NowACC = Axis_Z.Time_Delta*Axis_Z.Jerk_Speed;   //���㵱ǰ���ٶ� �Ӽ��ٶ�*ʱ��
						Axis_Z.NowFreq = Axis_Z.FreqPoint[2] - Axis_Z.NowACC*Axis_Z.Time_Delta/2;
						//��¼��ǰƵ��
						Axis_Z.FreqPoint[3] = Axis_Z.NowFreq;
					}
					else 
					{
						Axis_Z.SpeedChange_Stage++;
					}
				}break;
 
      /*********�Ӽ��ٽ׶�*******/
      //��ʽ V7=V6-a(t-t6)+1/2*h(t-t6)^2
				case 5:
				{
					if(Axis_Z.NowFreq > Z_END_FREQ)
					{
						Axis_Z.PulsePoint[4] = Axis_Z.NowPulse;  //��¼��ǰ������
						Axis_Z.TimerPoint[4] = Axis_Z.Timer_Cnt; //��¼��ǰʱ���
						Axis_Z.Time_Delta = Axis_Z.Timer_Cnt-Axis_Z.TimerPoint[3];//���㦤T
						//������ٶ�
						Axis_Z.NowACC = Axis_Z.Acc_Speed - (Axis_Z.Time_Delta*Axis_Z.Jerk_Speed) ;   //���㵱ǰ���ٶ� �Ӽ��ٶ�*ʱ��
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
      if(status > 999 && status < 63157)	//Ƶ����95~6006HZ֮�� 63157
      {
				PWM_Set_Z(status,PWM_PCS);
				TIM_Cmd(TIM4, ENABLE);
      }
    }
    else if(Axis_Z.InCtrl_Flag == 1)//����ʦģʽ����
    {
      Axis_Z.Target_Fre = 50+50*Action.Speed_Grade;
			Axis_Z.Target_Pulse = 50+10*Action.Step_Grade;
      status = PWMCtrl_ARRCount(PWM_PCS,Axis_Z.Target_Fre);
      PWM_Set_Z(status,PWM_PCS);
			TIM_Cmd(TIM4, ENABLE);
    }
		else if(Axis_Z.InCtrl_Flag == 3)//��λģʽ
		{
			Axis_Z.Target_Fre = 80;
      status = PWMCtrl_ARRCount(PWM_PCS,Axis_Z.Target_Fre);
      PWM_Set_Z(status,PWM_PCS);
			TIM_Cmd(TIM4, ENABLE);
		}
    else
    {
      
    }
				/************Y�Ჽ���������************/
		if(Axis_Y.InCtrl_Flag == 2) //����ģʽ
    {
      Axis_Y.Timer_Cnt++;
      /*********���ٽ׶�*******/
      if(Axis_Y.SpeedChange_Stage == 1)
      {
        /*********�Ӽ��ٽ׶�*******/
        //��ʽ V1=V0+1/2ht^2
        if(Axis_Y.NowACC < Axis_Y.Acc_Speed)   //��ǰ���ٶ�С���ȼ��ټ��ٶ�
        {
          Axis_Y.PulsePoint[0] = Axis_Y.NowPulse;  //��¼��ǰ������
          Axis_Y.TimerPoint[0] = Axis_Y.Timer_Cnt; //��¼��ǰʱ���
          /**********************************************************************/
          //���㵱ǰ���ٶ� �Ӽ��ٶ�*ʱ��
          Axis_Y.NowACC = Axis_Y.Timer_Cnt*Axis_Y.Jerk_Speed;    
          //���㵱ǰƵ��
          Axis_Y.NowFreq = Y_STAR_FREQ+Axis_Y.NowACC*Axis_Y.Timer_Cnt/2;//���㵱ǰ�ٶ� ���ٶ�*ʱ��/2
          //��¼Ƶ��
          Axis_Y.FreqPoint[0] = Axis_Y.NowFreq;
        }
        else 
        {
          Axis_Y.SpeedChange_Stage++;
        }
      }

      /*********�����ٽ׶�*******/
      //��ʽ V3=V2+a(t-t1)-1/2h(t-t2)^2
      else if(Axis_Y.SpeedChange_Stage == 2)
      {
        if((Axis_Y.NowACC > 0) && (Axis_Y.NowFreq <Axis_Y.Target_Fre) &&(Axis_Y.Timer_Cnt<500))
        {
          Axis_Y.PulsePoint[1] = Axis_Y.NowPulse;  //��¼��ǰ������
          Axis_Y.TimerPoint[1] = Axis_Y.Timer_Cnt; //��¼��ǰʱ���
          Axis_Y.Time_Delta = Axis_Y.Timer_Cnt-Axis_Y.TimerPoint[0];//���㦤T
          /**********************************************************************/
          //���㵱ǰ���ٶ�
          Axis_Y.NowACC = Axis_Y.Acc_Speed - (Axis_Y.Time_Delta*Axis_Y.Jerk_Speed); 
          //���㵱ǰƵ��
          Axis_Y.NowFreq = Axis_Y.FreqPoint[0] + (Axis_Y.Acc_Speed*Axis_Y.Time_Delta)
                           - (((Axis_Y.Jerk_Speed*Axis_Y.Time_Delta)*Axis_Y.Time_Delta)/2);
          //��¼��ǰƵ��
          Axis_Y.FreqPoint[1] = Axis_Y.NowFreq;
        }
        else 
        {
          Axis_Y.SpeedChange_Stage++;
        }
      }
      /*********���ٽ׶�*******/
      else if(Axis_Y.SpeedChange_Stage == 3)
      {
        if(Axis_Y.NowPulse < (Axis_Y.Target_Pulse - Axis_Y.PulsePoint[1]))
        {
          Axis_Y.PulsePoint[2] = Axis_Y.NowPulse;  //��¼��ǰ������
          Axis_Y.TimerPoint[2] = Axis_Y.Timer_Cnt; //��¼��ǰʱ���
          /**********************************************************************/
          Axis_Y.NowFreq = Axis_Y.Target_Fre;
          //��¼��ǰƵ��
          Axis_Y.FreqPoint[2] = Axis_Y.NowFreq;
        }
        else 
        {
          Axis_Y.SpeedChange_Stage++;
        }
      }
      /*********�Ӽ��ٽ׶�*******/
      //��ʽ V5=V4-1/2h(t-t4)^2
      else if(Axis_Y.SpeedChange_Stage == 4)
      {
        if(Axis_Y.NowACC < Axis_Y.Acc_Speed)
        {
          Axis_Y.PulsePoint[3] = Axis_Y.NowPulse;  //��¼��ǰ������
          Axis_Y.TimerPoint[3] = Axis_Y.Timer_Cnt; //��¼��ǰʱ���
          Axis_Y.Time_Delta = Axis_Y.Timer_Cnt-Axis_Y.TimerPoint[2];//���㦤T
          /**********************************************************************/
          Axis_Y.NowACC = Axis_Y.Time_Delta*Axis_Y.Jerk_Speed;   //���㵱ǰ���ٶ� �Ӽ��ٶ�*ʱ��
          Axis_Y.NowFreq = Axis_Y.FreqPoint[2] - Axis_Y.NowACC*Axis_Y.Time_Delta/2;
          //��¼��ǰƵ��
          Axis_Y.FreqPoint[3] = Axis_Y.NowFreq;
        }
        else 
        {
          Axis_Y.SpeedChange_Stage++;
        }
      }
 
      /*********�Ӽ��ٽ׶�*******/
      //��ʽ V7=V6-a(t-t6)+1/2*h(t-t6)^2
      else if(Axis_Y.SpeedChange_Stage == 5)
      {
        if(Axis_Y.NowFreq > Y_END_FREQ)
        {
          Axis_Y.PulsePoint[4] = Axis_Y.NowPulse;  //��¼��ǰ������
          Axis_Y.TimerPoint[4] = Axis_Y.Timer_Cnt; //��¼��ǰʱ���
          Axis_Y.Time_Delta = Axis_Y.Timer_Cnt-Axis_Y.TimerPoint[3];//���㦤T
          //������ٶ�
          Axis_Y.NowACC = Axis_Y.Acc_Speed - (Axis_Y.Time_Delta*Axis_Y.Jerk_Speed) ;   //���㵱ǰ���ٶ� �Ӽ��ٶ�*ʱ��
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
      if(status > 999 && status < 63157)	//Ƶ����95~6006HZ֮�� 63157
      {
				PWM_Set_Y(status,PWM_PCS);
				TIM_Cmd(TIM3, ENABLE);
      }
    }
		else if(Axis_Y.InCtrl_Flag == 1)//����ʦģʽ����
		{
			Axis_Y.Target_Fre = 60+50*Action.Speed_Grade;
			Axis_Y.Target_Pulse = 50+10*Action.Step_Grade;
      
			
			status = PWMCtrl_ARRCount(PWM_PCS,Axis_Y.Target_Fre);
      PWM_Set_Y(status,PWM_PCS);
			TIM_Cmd(TIM3, ENABLE);
			
			XYZ_To_Zero.Tray_Reset_Flag = 0;//����
		}	
		else if(Axis_Y.InCtrl_Flag == 3)//��λģʽ
		{
			Axis_Y.Target_Fre = 200;
      status = PWMCtrl_ARRCount(PWM_PCS,Axis_Y.Target_Fre);
      PWM_Set_Y(status,PWM_PCS);
			TIM_Cmd(TIM3, ENABLE);
		}
		else
		{
		}
		

/*****************�������Դ���************************/		
		if(ALARM_Y ==0 )
		{
			Alarm_Y.Timer_cnt++;
			if(Alarm_Y.Timer_cnt>1500)
			{
				Action_Stop();
				Uart_Send(0,CMD_CTRL_XZ,Local_Ip,0x61,Torque_Alm,0);//���ر�����Ϣ	
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
/*****************�������Դ���************************/			
			IWDG_Feed();//���Ź���ι��
//		}
		if(DelayTimes_ms)DelayTimes_ms--;
	  test_add = Axis_X.InCtrl_Flag+Axis_Y.InCtrl_Flag+Axis_Z.InCtrl_Flag;//�����������˶�״̬�������ж��˶��������Ƿ����״̬

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
		
		
		//��ȡ������ʱ�� 1.1��
		if(ReadSensorTime_100ms) ReadSensorTime_100ms--;
				
		//RFID��ʱ
		if(IC_RevTime > 0)	IC_RevTime--;
		
    //ģ�⶯����ʱ
		/*����*/
		/*******************************************************************************************************************/
	}
}



//��ʱ��2�жϷ������  /*250ms��ʱ*/
void TIM2_IRQHandler(void)   //TIM2�ж�
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM3�����жϷ������
	{	
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  			//���TIMx�����жϱ�־
		
	}
}

