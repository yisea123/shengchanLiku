#include "Main_Gateway.h"

void MainWhile_Gateway()
{
	u8 status;
	u8 OldSensorData;
	u8 SensorData;
	bool Sensor_Lock_save;
	//上电初始化
	Init_Gateway();

	while(1)
	{	
		/********按键读取	*******/
		Key_Deal();		
		
		/********解析CAN数据************/
		SURE_REC_Analyze_Gateway();      	
					
		/*******读取传感器状态***********/
		if(ReadSensorTime_100ms==0)
		{
			ReadSensorTime_100ms=11;
			SensorData=Station_Sensor_Read();
			if(SensorData!=OldSensorData)//有传感器发生变化
			{
				OldSensorData=SensorData;
				
				CANTXMEGSS.CANTXBuff[0]=0x81;
				CANTXMEGSS.CANTXBuff[1]=0x01;//目地地址
				CANTXMEGSS.CANTXBuff[2]=Device_Prop.DEVICE_ID;//源地址
				CANTXMEGSS.CANTXBuff[3]=0x13;//表示是SCU/DCU/ACU发往CCU/TCU的状态帧
				CANTXMEGSS.CANTXBuff[4]=0x02<<3;//
				CANTXMEGSS.CANTXBuff[5]=OldSensorData;//返回设备传感器状态		
				SendMessage(CANTXMEGSS.CANTXBuff);
			}
		}
  }
}

void Init_Gateway()
{

}

void Interrupt_Timer_1ms_Gateway()
{

}
