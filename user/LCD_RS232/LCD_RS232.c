
//#include "stm32f10x.h"
#include "LCD_RS232.h"

unsigned char R_right;                                  //接收报文正确标志
volatile struct_USART2 Uart2Buff[5];                    //在stm32f10x_it.h里定义	  //串口2接收数据CCU-TO-TCU
unsigned int X_Row;   
unsigned int Z_Column;  
unsigned int X_Run;   
unsigned int Z_Run;  
void PE7OUTPUT(void);
void PE7INPUT(void);
u8 Tray_Property = 0xff;
u8 Box_Property = 0xff;
u8 dataH,dataL;


Typedef_X_Zero_Pluse Change_X_Zero_Pluse;
Typedef_X_MPP Change_X_MPP;
Typedef_First_Goods_Height Change_First_Goods_Height;
Typedef_Z_MPP Change_Z_MPP;
Typedef_Plat_Axis_Z Change_Plat_Axis_Z;
Typedef_Plat_Axis_X Change_Plat_Axis_X;
Typedef_Y_RightZero_Pluse Change_Y_RightZero_Pluse;
Typedef_Y_LeftZero_Pluse Change_Y_LeftZero_Pluse;
Typedef_Y_LeftTrayPluse Change_Y_LeftTrayPluse;
Typedef_Y_LeftLim_MorePluse Change_Y_LeftLim_MorePluse;
Typedef_Y_LeftLim_LessPluse Change_Y_LeftLim_LessPluse;
Typedef_Y_RightLim_MorePluse Change_Y_RightLim_MorePluse;
Typedef_Y_RightLim_LessPluse Change_Y_RightLim_LessPluse;
Typedef_AxisZ_UpDownMore_Pluse Change_AxisZ_UpDownMore_Pluse;
Typedef_AxisZ_UpDown_Pluse Change_AxisZ_UpDown_Pluse;

//串口2初始化
void USART2_Init(u32 baud)
{
	NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure; 
	USART_InitTypeDef USART_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);					//USART2时钟设置
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	USART_DeInit(USART2);  //复位串口2
	
	/* Configure USART2 Tx (PA.2) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  /* Configure USART2 Rx (PA.3) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate =baud;						    				//波特率115200bps
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;			//8为数据位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;					//1位停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;							//无奇偶校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //无硬件流控制
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

  /********************USART2中断初始化***********************/
  //Usart2 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  

  USART_Init(USART2, &USART_InitStructure); 		//初始化串口
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART2, ENABLE);                    //使能串口
	USART_ClearFlag(USART2, USART_FLAG_TXE);			//清发送完成标志
}

void USART2_printf_str(u8 len, u8*put_str)
{
	u8 i;
	for(i=0; i<len; i++)
	{
		USART_SendData(USART2,put_str[i]);
	}
}

/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles USART2 global interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char recflag=0; //接收标志
unsigned int reclenth=0;//接收长度记录（分段）
unsigned char recbuff;	 //接收缓存
unsigned char check_temp=0;//校验位

void USART2_IRQHandler(void)
{
	u8 rec_buff;
  USART_ClearITPendingBit(USART2,USART_IT_RXNE); 
	if(Uart2_Mode == LCD)//迪文屏模式
	{
	recbuff=USART_ReceiveData(USART2);	//接收数据暂存
	switch(recflag)//初始值为0
	{
		case 0: //帧头1
		{
			 if(recbuff==0xa5)
			 {
			 	Uart2Buff[R_right].Star0=recbuff;
			 	recflag=1;
			 }
       else 
			 {
			 	recflag=0;//接收初始化重新接收
				reclenth=0;
				check_temp=0;
			 }
    }break;
		case 1: //帧头2
		{
			 if(recbuff==0x5a)
			 {
			 	Uart2Buff[R_right].Star1=recbuff;
			 	recflag=2;
			 }
       else 
			 {
			 	recflag=0;//接收初始化重新接收
				reclenth=0;
				check_temp=0;
			 }
    }break;
		case 2: //数据长度
		{
			 Uart2Buff[R_right].Len=recbuff;
			 check_temp+=recbuff;
			 recflag=3;
		}break;
		case 3: //指令
		{
			 Uart2Buff[R_right].Comend=recbuff;
			 check_temp+=recbuff;
			 recflag=4;
		}break;
		case 4: //数据
		{
			 Uart2Buff[R_right].ComData[reclenth]=recbuff;
			 reclenth++;
			 check_temp+=recbuff;
			 if(reclenth==(Uart2Buff[R_right].Len-1))
			 {
					 recflag=0;//接收初始化重新接收
					 check_temp=0;
					 reclenth=0;
					 R_right++;//接收成功标志【使能】
					 if(R_right>=4)R_right=1;
			 }
		}break;
	} 
		}
		else//激光传感器测距模式
		{
			rec_buff=USART_ReceiveData(USART2);	//接收数据暂存
			switch(rec_num)//初始值为0
			{
			case 0: //数据0
			{
				if(rec_buff!=0x6d)
				{
				 Laser_Rec[Laser_Rright].Buf[Laser_Length] = rec_buff ;	//
				 Laser_Length++;			
				 rec_num = 1;
				}
				else
				{
					Laser_Rec[Laser_Rright].Buf[Laser_Length] = rec_buff ;
					rec_num = 0;//接收初始化重新接收
					Lrecflag = 0; //接收完成标志
					Laser_Length = 0;
				}
			}break;
			case 1: //数据1
			{
				 if(rec_buff!=0x6d)//结束
				 {
						 Laser_Rec[Laser_Rright].Buf[Laser_Length] = rec_buff ;	//
						 Laser_Length++;
						 rec_num = 2;					   
				 }				 
					else
					{
					 Laser_Rec[Laser_Rright].Buf[Laser_Length] = rec_buff ;
						Laser_Length++;
					 Laser_Rec[Laser_Rright].Data_Len = 1;						
					 rec_num = 4;//接收初始化重新接收
					}				 					
			}break;			
			case 2://数据2
			{
				 if(rec_buff!=0x6d)//结束
				 {
					 Laser_Rec[Laser_Rright].Buf[Laser_Length] = rec_buff ;	//
					 Laser_Length++;
					 rec_num = 3;					 
				 }
				 else
				 {
					 Laser_Rec[Laser_Rright].Buf[Laser_Length] = rec_buff ;
					 Laser_Rec[Laser_Rright].Data_Len = 2;
					 rec_num = 4;//接收初始化重新接收
					 Laser_Length++;
				 }
			
							 
			}break;

			case 3://校验1
			{			
				if(rec_buff == 0x6d)//结束
			 {				 
				 Laser_Rec[Laser_Rright].Data_Len = 3;
				 rec_num = 4;//接收初始化重新接收
				 Laser_Rec[Laser_Rright].Buf[Laser_Length] = rec_buff ;
				 Laser_Length++;
			 }
			 else
			 {
				 rec_num = 0;//接收初始化重新接收
				 Lrecflag = 0; //接收完成标志
				 Laser_Length = 0;
			 }			 		
			}break;
			case 4://校验2
			{			
				if(rec_buff == 0x6d)//结束
			 {				 
				 rec_num = 5;//接收初始化重新接收
				 Laser_Rec[Laser_Rright].Buf[Laser_Length] = rec_buff ;
				 Laser_Length++;
			 }
			 else
			 {
				 rec_num = 0;//接收初始化重新接收
				 Lrecflag = 0; //接收完成标志
				 Laser_Length = 0;
			 }			 		
			}break;
			case 5://校验3
			{			
				if(rec_buff == 0x0d)//结束
			 {				 
				 rec_num = 6;//接收初始化重新接收
				 Laser_Rec[Laser_Rright].Buf[Laser_Length] = rec_buff ;
				 Laser_Length++;
			 }
			 else
			 {
				 rec_num = 0;//接收初始化重新接收
				 Lrecflag = 0; //接收完成标志
				 Laser_Length = 0;
			 }			 		
			}break;
			case 6://校验4
			{			
				if(rec_buff == 0x0a)//结束
			 {			
				 Laser_Rec[Laser_Rright].Buf[Laser_Length] = rec_buff ;				  
				 rec_num = 0;//接收初始化重新接收
				 Laser_Rright++;
				 Laser_Length=0;
				 Lrecflag = 1; //接收完成
				 
				 if(Laser_Rright>=(LasBufLen-1))
				 {
					Laser_Rright = 1;
					Laser_Length = 0;						 
				 }
			 }
			 else
			 {
				 rec_num = 0;//接收初始化重新接收
				 Lrecflag = 0; //接收完成标志
				 Laser_Length = 0;
			 }			 		
			}break;
			

			} 
		}
}

/***********************************************
* 函数名称：void SURE_REC_UART2_Analyze(void)
* 功能描述：判断是否要解析串口接收到的数据
* 输入参数：无
* 调用函数：无
* 返回结果：无
***********************************************/
void SURE_REC_UART2_Analyze(void)
{
	unsigned char i;
	if(R_right)			
	{	
		if(Uart2Buff[4].Comend==0)
		{
			//取出数据来进行解析
			Uart2Buff[4].check      = Uart2Buff[R_right-1].check;
			Uart2Buff[4].Star0      = Uart2Buff[R_right-1].Star0;
			Uart2Buff[4].Star1      = Uart2Buff[R_right-1].Star1;
			Uart2Buff[4].Len		    = Uart2Buff[R_right-1].Len;
			Uart2Buff[4].Comend     = Uart2Buff[R_right-1].Comend;
			for(i=0;i<Uart2Buff[4].Len;i++)
			{
				Uart2Buff[4].ComData[i]=Uart2Buff[R_right-1].ComData[i];
			}
			R_right--;
			REC_UART2_Analyze(4);
			Uart2Buff[4].Comend=0;
		}
	}
}

/***********************************************
* 函数名称：void REC_UART2_Analyze(void)
* 功能描述：解析串口接收到的数据
* 输入参数：报文号right_num
* 调用函数：无
* 返回结果：无
***********************************************/
void REC_UART2_Analyze(unsigned char right_num)//串口数析【SCREEN-TO-ACU】
{			
   
	switch(Uart2Buff[right_num].Comend)
	{
		case 0x81:
		{
		}break;
		case 0x83:	 //屏串口返回指令
		{
			switch(Uart2Buff[right_num].ComData[1])//指令帧第六字节 屏变量地址低位地址
			{
				case 0x00:
				{
				}break;
				case 0x01:
				{
					switch(Uart2Buff[right_num].ComData[4])
					{
						case 0x01:    //步长1
						{
							Action.Step_Grade = 0;
						}break;
						case 0x02:    //步长2
						{
							Action.Step_Grade = 1;
						}break;
						case 0x03:    //步长3
						{
							Action.Step_Grade = 2;
						}break;
						case 0x04:    //步长4
						{
							Action.Step_Grade = 3;
							//StepMotorCtrl_Pulse(2500,AXIS_Y,Y_DIR_LEFT);//宏定义Y_DIR_LEFT
						}break;
						case 0x05:    //步长5
						{
							Action.Step_Grade = 4;
							//StepMotorCtrl_Pulse(2500,AXIS_Y,Y_DIR_RIGHT);//宏定义
						}break;
						case 0x06:    //步长6
						{
							Action.Step_Grade = 5;
							//StepMotorCtrl_Pulse(2150,AXIS_Y,Y_DIR_LEFT);//2200
						}break;
						case 0x07:    //步长7
						{
							Action.Step_Grade = 6;
							//StepMotorCtrl_Pulse(2180,AXIS_Y,Y_DIR_RIGHT);//宏定义
						}break;
            case 0x08:    //速度档位1
						{
							Action.Speed_Grade = 0;
						}break;
						case 0x09:    //速度档位2
						{
							Action.Speed_Grade = 1;
						}break;
						case 0x0a:    //速度档位3
						{
							Action.Speed_Grade = 2;
						}break;
						case 0x0b:    //速度档位4
						{
							Action.Speed_Grade = 3;
						}break;
						case 0x0c:    //速度档位5
						{
							Action.Speed_Grade = 4;
						}break;
						case 0x0d:    //速度档位6
						{
							Action.Speed_Grade = 5;
						}break;
						case 0x0e:    //速度档位7
						{
							Action.Speed_Grade = 6;
						}break;
						case 0x0f:    // x轴前进
						{
							 Axis_Move_Alone(AXIS_X,X_DIR_FRONT);
						}break;
						case 0x10:    // x轴后退,归位方向
						{						
							 Axis_Move_Alone(AXIS_X,X_DIR_BACK);
						}break;
						case 0x11:    // y轴左移
						{
							 Axis_Move_Alone(AXIS_Y,Y_DIR_LEFT);
						}break;
						case 0x12:    // y轴右移
						{
							 Axis_Move_Alone(AXIS_Y,Y_DIR_RIGHT);
						}break;
						case 0x13:    // z轴上升
						{
							 Axis_Move_Alone(AXIS_Z,Z_DIR_FRONT);
						}break;
						case 0x14:    // z轴下降
						{
							 Axis_Move_Alone(AXIS_Z,Z_DIR_BACK);
						}break;  
						case 0x15:  // z轴归位
						{
							Motor_BackToZero(AXIS_Z);
						}break; 
						case 0x16:   // x轴归位
						{
							Motor_BackToZero(AXIS_X);
						}break; 
						case 0x17:   // 三轴归位
						{
							XYZ_To_Zero.Start_Flag = 1;
							XYZ_To_Zero.XYZ_Ret_Stage = 1;
							XYZ_To_Zero.XYZRET_Status_Change = 0;
						}break; 
						case 0x18:  //停止
						{
							Action_Stop();
						}break; 
						case 0x19:  //到达
						{
							Get_PutBox_Analysis(Arrive_Analysis,Keep,Arrive,X_Run,Z_Run);
							Beep(50,50,1);
						}break;
						case 0x1a:  //抓箱
						{
							if(Box_Property == 0xff || Tray_Property == 0xff)      //必须设置货物属性和托盘属性
							{
								Beep(50,50,3);
								return;
							}
							else
							{							
								Get_Box_Analysis(Tray_Property,Box_Property,X_Run,Z_Run);
								ChangePage(PAGE_POSITION);
							  Box_Property = 0xff;
							  Tray_Property =  0xff;
							}
						}break;
						case 0x1b:  //放箱
						{
							if(Box_Property == 0xff || Tray_Property == 0xff)
							{
								Beep(50,50,3);
								return;
							}
							else
							{
								Action.PutBox_Cnt++;
								Put_Box_Analysis(Tray_Property,Box_Property,X_Run,Z_Run);
								ChangePage(PAGE_POSITION);
							  Box_Property = 0xff;
							  Tray_Property =  0xff;
							}							
						}break;	
						case 0x1c:   //抓箱货物属性左
						{
							Box_Property = Get_Left_Box; 
						}break;
						case 0x1d:   //抓箱货物属性右
						{
							Box_Property = Get_Right_Box; 
						}break;			
            case 0x1e:   //放箱货物属性左
						{
							Box_Property = Return_Left_Box; 
						}break;						
						case 0x1f:   //放箱货物属性右
						{
							Box_Property = Return_Right_Box; 
						}break;						
						case 0x20:   //托盘属性左
						{
							Tray_Property = Left_Tray; 
						}break;			
						case 0x21:   //托盘属性右
						{
							Tray_Property = Right_Tray; 
						}break;									
					}
				}break;
        case 0x02:     //设置X轴坐标
				{
					X_Row = Uart2Buff[right_num].ComData[4];
					if(X_Row>=9)
					{
						return;
					}
					X_Run= 412 * X_Row;
				}break;
				case 0x03:    //设置Z轴坐标
				{
					Z_Column = Uart2Buff[right_num].ComData[4];
					if((Z_Column>=10)||(Z_Column==0))
					{
						return;
					}
					if(Z_Column <= 4)    
					{
						Z_Run = 270*Z_Column - Z_Difference;
					}
					else    //第五层突变
					{
						Z_Run = 270*Z_Column - (Z_Difference-70);//170
					}
				}break;		
				case 0x04:   //修改零位回弹脉冲
				{
						dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
					  Change_X_Zero_Pluse.X_Zero_Pluse_Data = (dataH<<8)|dataL;
						I2C_BufferWrite(Change_X_Zero_Pluse.X_Zero_Pluse_CharData,4,1);
						X_Zero_Pluse = Change_X_Zero_Pluse.X_Zero_Pluse_Data;
				}break;	
				case 0x05:   //修改X轴毫米每脉冲
				{   
					  dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
					  Change_X_MPP.X_MPP_Data = ((dataH<<8)|dataL)/10000.0;
						I2C_BufferWrite(Change_X_MPP.X_MPP_CharData,8,10);
						X_MPP = Change_X_MPP.X_MPP_Data;
				}break;
				case 0x06:    //修改首层货架相对托盘高度
				{
						dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
						Change_First_Goods_Height.First_Goods_Height_Data = (dataH<<8)|dataL;
						I2C_BufferWrite(Change_First_Goods_Height.First_Goods_Height_CharData,4,25);
						First_Goods_Height = Change_First_Goods_Height.First_Goods_Height_Data;
				}break;
				case 0x07:    //修改Z轴毫米每脉冲
				{
					  dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
					  Change_Z_MPP.Z_MPP_Data = ((dataH<<8)|dataL)/10000.0;
						I2C_BufferWrite(Change_Z_MPP.Z_MPP_CharData,8,35);
						Z_MPP = Change_Z_MPP.Z_MPP_Data;
				}break;
				case 0x08:   //修改托架X轴坐标
				{
						dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
						Change_Plat_Axis_Z.Plat_Axis_Z_Data = (dataH<<8)|dataL;
						I2C_BufferWrite(Change_Plat_Axis_Z.Plat_Axis_Z_CharData,4,50);
						Plat_Axis_Z = Change_Plat_Axis_Z.Plat_Axis_Z_Data;
				}break;  
				case 0x09:  //修改托架Z轴坐标
				{
						dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
						Change_Plat_Axis_X.Plat_Axis_X_Data = (dataH<<8)|dataL;
						I2C_BufferWrite(Change_Plat_Axis_X.Plat_Axis_X_CharData,4,60);
						Plat_Axis_X = Change_Plat_Axis_X.Plat_Axis_X_Data;
				}break;
				case 0x0A:     //修改托盘右零位回缩脉冲
				{
						dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
						Change_Y_RightZero_Pluse.Y_RightZero_Pluse_Data = (dataH<<8)|dataL;
						I2C_BufferWrite(Change_Y_RightZero_Pluse.Y_RightZero_Pluse_CharData,4,70);
						Y_RightZero_Pluse = Change_Y_RightZero_Pluse.Y_RightZero_Pluse_Data;
				}break;
				case 0x0B:    //修改托盘左零位回缩脉冲
				{						
					  dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
						Change_Y_LeftZero_Pluse.Y_LeftZero_Pluse_Data = (dataH<<8)|dataL;
						I2C_BufferWrite(Change_Y_LeftZero_Pluse.Y_LeftZero_Pluse_CharData,4,80);
						Y_LeftZero_Pluse = Change_Y_LeftZero_Pluse.Y_LeftZero_Pluse_Data;
				}break;
				case 0x0C:   //修改托盘伸缩脉冲
				{						
					  dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
						Change_Y_LeftTrayPluse.Y_LeftTrayPluse_Data = (dataH<<8)|dataL;
						I2C_BufferWrite(Change_Y_LeftTrayPluse.Y_LeftTrayPluse_CharData,4,90);
						Y_LeftTrayPluse = Change_Y_LeftTrayPluse.Y_LeftTrayPluse_Data;
				}break; 
				case 0x0D:    //修改托盘从托架往左多走脉冲
				{						
					  dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
						Change_Y_LeftLim_MorePluse.Y_LeftLim_MorePluse_Data = (dataH<<8)|dataL;
						I2C_BufferWrite(Change_Y_LeftLim_MorePluse.Y_LeftLim_MorePluse_CharData,4,100);
						Y_LeftLim_MorePluse = Change_Y_LeftLim_MorePluse.Y_LeftLim_MorePluse_Data;
				}break;
				case 0x0E:    //修改托盘从托架往左少走脉冲
				{						
					  dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
						Change_Y_LeftLim_LessPluse.Y_LeftLim_LessPluse_Data = (dataH<<8)|dataL;
						I2C_BufferWrite(Change_Y_LeftLim_LessPluse.Y_LeftLim_LessPluse_CharData,4,110);
						Y_LeftLim_LessPluse = Change_Y_LeftLim_LessPluse.Y_LeftLim_LessPluse_Data;
				}break;
				case 0x0F:   //修改托盘从托架往右多走脉冲
				{						
					  dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
						Change_Y_RightLim_MorePluse.Y_RightLim_MorePluse_Data = (dataH<<8)|dataL;
						I2C_BufferWrite(Change_Y_RightLim_MorePluse.Y_RightLim_MorePluse_CharData,4,120);
						Y_RightLim_MorePluse = Change_Y_RightLim_MorePluse.Y_RightLim_MorePluse_Data;
				}break;
				case 0x10:    //修改托盘从托架往右少走脉冲
				{						
					  dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
						Change_Y_RightLim_LessPluse.Y_RightLim_LessPluse_Data = (dataH<<8)|dataL;
						I2C_BufferWrite(Change_Y_RightLim_LessPluse.Y_RightLim_LessPluse_CharData,4,130);
						Y_RightLim_LessPluse = Change_Y_RightLim_LessPluse.Y_RightLim_LessPluse_Data;
				}break;				
				case 0x11:    //修改托盘Z轴升降脉冲
				{					
   					dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
						Change_AxisZ_UpDown_Pluse.AxisZ_UpDown_Pluse_Data = (dataH<<8)|dataL;
						I2C_BufferWrite(Change_AxisZ_UpDown_Pluse.AxisZ_UpDown_Pluse_CharData,4,140);
						AxisZ_UpDown_Pluse = Change_AxisZ_UpDown_Pluse.AxisZ_UpDown_Pluse_Data;
				}break;				
				case 0x12:   //修改托盘Z轴升降多走脉冲
				{						
					  dataH = Uart2Buff[right_num].ComData[3];
					  dataL = Uart2Buff[right_num].ComData[4];
						Change_AxisZ_UpDownMore_Pluse.AxisZ_UpDownMore_Pluse_Data = (dataH<<8)|dataL;
						I2C_BufferWrite(Change_AxisZ_UpDownMore_Pluse.AxisZ_UpDownMore_Pluse_CharData,4,150);
						AxisZ_UpDownMore_Pluse = Change_AxisZ_UpDownMore_Pluse.AxisZ_UpDownMore_Pluse_Data;
				}break;				
			}
		}break;
	}
}

//应答回应SCREEN 不要返回
void Uart2AskBytes(unsigned char *str,unsigned int Num) 
{
	unsigned int i;	
	for(i=0;i<Num;i++)
	{  
		while( USART_GetFlagStatus(USART2,USART_FLAG_TC)!= SET);  
		//等待发送完成。   检测 USART_FLAG_TC 是否置1；    //见库函数 P359 介绍
		USART_SendData(USART2,str[i]);	
		if((i+1)%4096==0)
		{
			DelayTimes_ms=10;
			while (DelayTimes_ms);
		}
	}		
}
/************************************************
* 函数名称：void Set_Variable(unsigned char adress, unsigned char data)
* 功能描述：设置液晶屏变量地址中的数值
* 输入参数：地址adress，数据data
* 返回结果：无
*************************************************/
void Set_Variable(unsigned char adressH,unsigned char adressL, 
									unsigned char data0, unsigned char data1, 
									unsigned char data2, unsigned char data3, 
									unsigned char length)
{
	unsigned char sendtemp[10];
	sendtemp[0]=0xA5;
	sendtemp[1]=0x5A;
	sendtemp[2]=0x03+length;
	sendtemp[3]=0x82;
	sendtemp[4]=adressH;
	sendtemp[5]=adressL;
	sendtemp[6]=data0;
	sendtemp[7]=data1;
	sendtemp[8]=data2;
	sendtemp[9]=data3;
	Uart2AskBytes(sendtemp,length+6);			
}

void ChangePage(unsigned char page)
{
  u8 sendbuff[7];
  sendbuff[0] = 0xA5;
  sendbuff[1] = 0x5A;
  sendbuff[2] = 0x04;
  sendbuff[3] = 0x80;
  sendbuff[4] = 0x03;
  sendbuff[5] = 0x00;
  sendbuff[6] = page;
  Uart2AskBytes(sendbuff,7);
}

void PE7OUTPUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);							
	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_9;//PE0：P27_DIR2,PE2：P27_PUL2,PE4：P28_DIR1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 				
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				
	GPIO_Init(GPIOE, &GPIO_InitStructure);									
	GPIO_ResetBits(GPIOE, GPIO_Pin_9);
}

void PE7INPUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);							
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9; //IO		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void All_I2C_Read(void)
{
			I2C_ReadByte(Change_X_Zero_Pluse.X_Zero_Pluse_CharData,4,1);
	    I2C_ReadByte(Change_X_MPP.X_MPP_CharData,8,10);
	    I2C_ReadByte(Change_First_Goods_Height.First_Goods_Height_CharData,4,25);
		  I2C_ReadByte(Change_Z_MPP.Z_MPP_CharData,8,35);
			I2C_ReadByte(Change_Plat_Axis_Z.Plat_Axis_Z_CharData,4,50);
	    I2C_ReadByte(Change_Plat_Axis_X.Plat_Axis_X_CharData,4,60);
	    I2C_ReadByte(Change_Y_RightZero_Pluse.Y_RightZero_Pluse_CharData,4,70);
	    I2C_ReadByte(Change_Y_LeftZero_Pluse.Y_LeftZero_Pluse_CharData,4,80);
	    I2C_ReadByte(Change_Y_LeftTrayPluse.Y_LeftTrayPluse_CharData,4,90);
	    I2C_ReadByte(Change_Y_LeftLim_MorePluse.Y_LeftLim_MorePluse_CharData,4,100);
	    I2C_ReadByte(Change_Y_LeftLim_LessPluse.Y_LeftLim_LessPluse_CharData,4,110);
	    I2C_ReadByte(Change_Y_RightLim_MorePluse.Y_RightLim_MorePluse_CharData,4,120);
	    I2C_ReadByte(Change_Y_RightLim_LessPluse.Y_RightLim_LessPluse_CharData,4,130);
	    I2C_ReadByte(Change_AxisZ_UpDown_Pluse.AxisZ_UpDown_Pluse_CharData,4,140);
	    I2C_ReadByte(Change_AxisZ_UpDownMore_Pluse.AxisZ_UpDownMore_Pluse_CharData,4,150);
}

unsigned char HexToBcd(unsigned int Hex)
{
	Hex&=0xff;
	return (Hex/16*10)+(Hex%16);
}
