
#include "Key.h"

bool Key_press_flag = 0;
bool KEY1_flag = 0;
bool KEY2_flag = 0;
bool KEY3_flag = 0;
bool KEY4_flag = 0;
bool KEY5_flag = 0;
bool KEY6_flag = 0;
bool KEY7_flag = 0;
bool KEY8_flag = 0;

bool Bottle_Judge_Record; //大小瓶记录模式，由(KEY3 + 复位键)触发	
u8 TEST_MODE;           //测试模式标记，由(KEY4 + 复位键)触发	
u8 TEST_MODE_SAVE = 1;
u8 Bottle_Judge_Mode = 0;	//大小瓶判断脉冲记录模式，记录高矮瓶模式切换
//		char buf0[] = {'+','+','+','\0'};
//		char buf1[] = {'A','T','+','C','W','M','O','D','E','_','D','E','F','=','1','\r','\n','\0'};
//		char buf2[] = {'A','T','+','C','W','J','A','P','_','D','E','F','=','"','J','S','-','L','I','K','U','"',',','"','j','e','s','o','o','1','2','3','"','\r','\n','\0'};
//		char buf3[] = {'A','T','+','C','I','P','S','T','A','R','T','=','"','T','C','P','"',',','"','1','9','2','.','1','6','8','.','0','.','5','3','"',',','6','0','0','0','\r','\n','\0'};
//		char buf4[] = {'A','T','+','C','I','P','M','O','D','E','=','1','\r','\n','\0'};
//		char buf5[] = {'A','T','+','C','I','P','S','T','A','_','D','E','F','=',
//									'"','1','9','2','.','1','6','8','.','0','.','1','1','7','"',',',
//									'"','1','9','2','.','1','6','8','.','0','.','1','1','6','"',',',
//									'"','2','5','5','.','2','5','5','.','2','5','5','.','0','"','\r','\n','\0'};
//		char buf6[] = {'A','T','+','C','I','P','S','E','N','D','\r','\n','\0'};

/*-----------*/
u16 Time_PushRod_Ctrl;   

void KEY_Init(void) //按键IO初始化
{ 
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;//PD0,PD2,PD4,PD6
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOD	
}

/************按键检测***********/
void Key_Scan()
{
	if(KEY1 == 0)
	{
		if(Key_press_flag == 0)
		{
			delay_ms(3);
			if(KEY1 == 0)
			{
				Key_press_flag = 1;
				KEY1_flag = 1;
			}
		}
	}
	else if(KEY2 == 0)
	{
		if(Key_press_flag == 0)
		{
			delay_ms(3);
			if(KEY2 == 0)
			{
				Key_press_flag = 1;
				KEY2_flag = 1;
			}
		}
	}
	else if(KEY3 == 0)
	{
		if(Key_press_flag == 0)
		{
			delay_ms(3);
			if(KEY3 == 0)
			{
				Key_press_flag = 1;
				KEY3_flag = 1;
			}
		}
	}
	else if(KEY4 == 0)
	{
		if(Key_press_flag == 0)
		{
			delay_ms(3);
			if(KEY4 == 0)
			{
				Key_press_flag = 1;
				KEY4_flag = 1;
			}
		}
	}
	else if(KEY5 == 0)
	{
		if(Key_press_flag == 0)
		{
			delay_ms(3);
			if(KEY5 == 0)
			{
				Key_press_flag = 1;
				KEY5_flag = 1;
			}
		}
	}
	else if(KEY6 == 0)
	{
		if(Key_press_flag == 0)
		{
			delay_ms(3);
			if(KEY6 == 0)
			{
				Key_press_flag = 1;
				KEY6_flag = 1;
			}
		}		
	}
	else
	{
		Key_press_flag = 0;
	}
}

/************按键处理***********/
void Key_Deal()
{
	u8 test;
	int dis;
	Key_Scan();
	/***********按键1处理********************/
	if(KEY1_flag == 1)
	{
		KEY1_flag = 0;
		StepMotorCtrl_Pulse(5,AXIS_Z,Z_DIR_FRONT);
		Beep(200,200,1);
		//Axis_Move_Alone(AXIS_X,X_DIR_FRONT);
	}
	/***********按键2处理********************/
	if(KEY2_flag == 1)
	{
		KEY2_flag = 0;
		StepMotorCtrl_Pulse(5,AXIS_Z,Z_DIR_BACK);
		Beep(200,200,1);

	}
	/***********按键3处理********************/
	if(KEY3_flag == 1)
	{
//		u8 CheckAdd =0;
		KEY3_flag = 0;		
		StepMotorCtrl_Pulse(5,AXIS_X,X_DIR_BACK);
		Beep(200,100,1);      //按键蜂鸣一声
	}
	/***********按键4处理********************/
	if(KEY4_flag == 1)
	{
		KEY4_flag = 0;
		//StepMotorCtrl_Pulse(5,AXIS_X,X_DIR_FRONT);
	Aid_Z.Stage = 1;
				Aid_Z.Status_Change = 1;
				Aid_Z.Start_Flag = 1;
		Beep(200,100,1);      //按键蜂鸣一声

//		Uart2_Mode = Laser;
//		Laser_Init();
	}
	/***********按键5处理********************/
	if(KEY5_flag == 1)
	{
		KEY5_flag = 0;
		StepMotorCtrl_Pulse(5,AXIS_Y,Y_DIR_RIGHT);
		Beep(50,50,1);      //按键蜂鸣一声

						
	}
	/***********按键6处理********************/
	if(KEY6_flag == 1)
	{
		KEY6_flag = 0;
//		StepMotorCtrl_Pulse(5,AXIS_Y,Y_DIR_LEFT);
		Beep(50,50,1);      //按键蜂鸣一声		
						Change_X_Zero_Pluse.X_Zero_Pluse_Data = 30;
						I2C_PageWrite(Change_X_Zero_Pluse.X_Zero_Pluse_CharData,4,1);
						X_Zero_Pluse = Change_X_Zero_Pluse.X_Zero_Pluse_Data;
						
						Change_X_MPP.X_MPP_Data = 0.4400;
		        I2C_BufferWrite(Change_X_MPP.X_MPP_CharData,8,10);
		        X_MPP = Change_X_MPP.X_MPP_Data;
						
						Change_First_Goods_Height.First_Goods_Height_Data = 35;
						I2C_BufferWrite(Change_First_Goods_Height.First_Goods_Height_CharData,4,25);
						First_Goods_Height = Change_First_Goods_Height.First_Goods_Height_Data;
						
						Change_Z_MPP.Z_MPP_Data = 0.3400;
						I2C_BufferWrite(Change_Z_MPP.Z_MPP_CharData,8,35);
						Z_MPP = Change_Z_MPP.Z_MPP_Data;
						
						Change_Plat_Axis_Z.Plat_Axis_Z_Data = 227;
						I2C_BufferWrite(Change_Plat_Axis_Z.Plat_Axis_Z_CharData,4,50);
						Plat_Axis_Z = Change_Plat_Axis_Z.Plat_Axis_Z_Data;
						
						Change_Plat_Axis_X.Plat_Axis_X_Data = 0;
						I2C_BufferWrite(Change_Plat_Axis_X.Plat_Axis_X_CharData,4,60);
						Plat_Axis_X = Change_Plat_Axis_X.Plat_Axis_X_Data;
						
						Change_Y_RightZero_Pluse.Y_RightZero_Pluse_Data = 70;
						I2C_BufferWrite(Change_Y_RightZero_Pluse.Y_RightZero_Pluse_CharData,4,70);
						Y_RightZero_Pluse = Change_Y_RightZero_Pluse.Y_RightZero_Pluse_Data;

						Change_Y_LeftZero_Pluse.Y_LeftZero_Pluse_Data = 30;
						I2C_BufferWrite(Change_Y_LeftZero_Pluse.Y_LeftZero_Pluse_CharData,4,80);
						Y_LeftZero_Pluse = Change_Y_LeftZero_Pluse.Y_LeftZero_Pluse_Data;//95

						Change_Y_LeftTrayPluse.Y_LeftTrayPluse_Data = 2180;
						I2C_BufferWrite(Change_Y_LeftTrayPluse.Y_LeftTrayPluse_CharData,4,90);
						Y_LeftTrayPluse = Change_Y_LeftTrayPluse.Y_LeftTrayPluse_Data;

						Change_Y_LeftLim_MorePluse.Y_LeftLim_MorePluse_Data = 2180;
						I2C_BufferWrite(Change_Y_LeftLim_MorePluse.Y_LeftLim_MorePluse_CharData,4,100);
						Y_LeftLim_MorePluse = Change_Y_LeftLim_MorePluse.Y_LeftLim_MorePluse_Data;

						Change_Y_LeftLim_LessPluse.Y_LeftLim_LessPluse_Data = 2180;//修正
						I2C_BufferWrite(Change_Y_LeftLim_LessPluse.Y_LeftLim_LessPluse_CharData,4,110);
						Y_LeftLim_LessPluse = Change_Y_LeftLim_LessPluse.Y_LeftLim_LessPluse_Data;

						Change_Y_RightLim_MorePluse.Y_RightLim_MorePluse_Data = 2180;//修正
						I2C_BufferWrite(Change_Y_RightLim_MorePluse.Y_RightLim_MorePluse_CharData,4,120);
						Y_RightLim_MorePluse = Change_Y_RightLim_MorePluse.Y_RightLim_MorePluse_Data;

						Change_Y_RightLim_LessPluse.Y_RightLim_LessPluse_Data = 2180;//修正
						I2C_BufferWrite(Change_Y_RightLim_LessPluse.Y_RightLim_LessPluse_CharData,4,130);
						Y_RightLim_LessPluse = Change_Y_RightLim_LessPluse.Y_RightLim_LessPluse_Data;

						Change_AxisZ_UpDown_Pluse.AxisZ_UpDown_Pluse_Data = 100;
						I2C_BufferWrite(Change_AxisZ_UpDown_Pluse.AxisZ_UpDown_Pluse_CharData,4,140);
						AxisZ_UpDown_Pluse = Change_AxisZ_UpDown_Pluse.AxisZ_UpDown_Pluse_Data;

						Change_AxisZ_UpDownMore_Pluse.AxisZ_UpDownMore_Pluse_Data = 170;
						I2C_BufferWrite(Change_AxisZ_UpDownMore_Pluse.AxisZ_UpDownMore_Pluse_CharData,4,150);
						AxisZ_UpDownMore_Pluse = Change_AxisZ_UpDownMore_Pluse.AxisZ_UpDownMore_Pluse_Data;
						
	}
}



