
#include "Sys_Init.h"

void Sys_Init(void)
{		

	/************系统初始化*******************************************************/
	/*公用*/
	delay_init();	    		//延时函数初始化	  
	LED_Init();		  			//初始化与LED连接的硬件接口
	Beep_Init();					//蜂鸣器初始化
	KEY_Init();						//按键初始化
	GPIO_OutPut_Init();
	GPIO_InPut_Init();
	PWM_Init();
	USART3_Init(4800);		//串口3初始化 波特率9600
	USART2_Init(115200);		//串口2迪文屏初始化
	Uart2_Mode = LCD;
	NVIC_Configuration(); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	TIM3_Int_Init(9,7199);//10Khz的计数频率，计数到1000为1ms 	
	IIC_Init();//IIC写参数初始化

	IWDG_Init(6,3000);//20秒钟喂狗一次
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


