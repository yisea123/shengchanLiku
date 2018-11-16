 
#include "Sys_Init.h"	
int main(void)
{
	/*******上电初始化*******/
	Sys_Init();		

	/***************************主循环****************************/
	while(1)
	{
//		Axis_Ultra_Limit();//XZ轴坐标限位
//		Tray_Limit_Contr();//托盘限位
		
		if(Timer_1ms_flag == 1)//1ms轮询，执行需要定时的功能   发送数据/模拟控制
		{
			//asdasd();
		//	Uart_Analysis();//网络串口解析
			Timer_1ms_flag = 0;
			//XYZ_BackZero();//三轴复位阶段轮询
			Key_Deal();//键盘扫描
//			SURE_REC_UART2_Analyze();     //迪文屏串口解析
//			Get_Box(Action.Target_X,Action.Target_Z);//取箱阶段轮询
//			Put_Box(Action.Target_X,Action.Target_Z);//放箱阶段轮询
//			if(Uart3[Cache_Cnt].RecError ==1)//接收数据错误，丢弃本次数据，并关闭串口5毫秒
//			{
//				Beep(50,50,3);
//				Uart3[Cache_Cnt].RecError = 0;				
//				memset(&Uart3[Cache_Cnt].Buffer,0,sizeof(Uart3[Cache_Cnt].Buffer));//清空数组
//				Uart_Send(0,Analysis_Error,Local_Ip,0x61,Analysis_Error,0);//返回报警信息	
//				USART_Cmd(USART3, DISABLE);//关闭串口
//				Enable_Uart3_Flg = 1;
//			}
//			if(Reset_Flag ==1)//报警复位，清除标志位
//			{
//				PAout(5) = 0;
//				Reset_Flag=0;
//			}
		}
	}
}


int fputc(int ch, FILE *f)
{
	USART_SendData(USART3, (unsigned char) ch);
	while (!(USART3->SR & USART_FLAG_TXE));
	return (ch);

}

