
#include "Sys_Init.h"

void Sys_Init(void)
{		

	/************ϵͳ��ʼ��*******************************************************/
	/*����*/
	delay_init();	    		//��ʱ������ʼ��	  
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	Beep_Init();					//��������ʼ��
	KEY_Init();						//������ʼ��
	GPIO_OutPut_Init();
	GPIO_InPut_Init();
	PWM_Init();
	USART3_Init(4800);		//����3��ʼ�� ������9600
	USART2_Init(115200);		//����2��������ʼ��
	Uart2_Mode = LCD;
	NVIC_Configuration(); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	TIM3_Int_Init(9,7199);//10Khz�ļ���Ƶ�ʣ�������1000Ϊ1ms 	
	IIC_Init();//IICд������ʼ��

	IWDG_Init(6,3000);//20����ι��һ��
	Reset_Flag = 0;

	All_I2C_Read();
	
	X_Zero_Pluse = Change_X_Zero_Pluse.X_Zero_Pluse_Data;
	X_MPP = Change_X_MPP.X_MPP_Data;
	First_Goods_Height = Change_First_Goods_Height.First_Goods_Height_Data;
	Z_MPP = Change_Z_MPP.Z_MPP_Data;
	Plat_Axis_Z = Change_Plat_Axis_Z.Plat_Axis_Z_Data;
	Plat_Axis_X = Change_Plat_Axis_X.Plat_Axis_X_Data;
	Y_RightZero_Pluse = Change_Y_RightZero_Pluse.Y_RightZero_Pluse_Data;
	Y_LeftZero_Pluse = Change_Y_LeftZero_Pluse.Y_LeftZero_Pluse_Data;
	Y_LeftTrayPluse = Change_Y_LeftTrayPluse.Y_LeftTrayPluse_Data;
	Y_LeftLim_MorePluse = Change_Y_LeftLim_MorePluse.Y_LeftLim_MorePluse_Data;
	Y_LeftLim_LessPluse = Change_Y_LeftLim_LessPluse.Y_LeftLim_LessPluse_Data;
	Y_LeftLim_LessPluse = Change_Y_LeftLim_LessPluse.Y_LeftLim_LessPluse_Data;
	Y_RightLim_MorePluse = Change_Y_RightLim_MorePluse.Y_RightLim_MorePluse_Data;
	Y_RightLim_LessPluse = Change_Y_RightLim_LessPluse.Y_RightLim_LessPluse_Data;
	AxisZ_UpDown_Pluse = Change_AxisZ_UpDown_Pluse.AxisZ_UpDown_Pluse_Data;
	AxisZ_UpDownMore_Pluse = Change_AxisZ_UpDownMore_Pluse.AxisZ_UpDownMore_Pluse_Data;
	
	Beep(50,50,1);
	
}


