#include "Main_Gateway.h"

void MainWhile_Gateway()
{
	u8 status;
	u8 OldSensorData;
	u8 SensorData;
	bool Sensor_Lock_save;
	//�ϵ��ʼ��
	Init_Gateway();

	while(1)
	{	
		/********������ȡ	*******/
		Key_Deal();		
		
		/********����CAN����************/
		SURE_REC_Analyze_Gateway();      	
					
		/*******��ȡ������״̬***********/
		if(ReadSensorTime_100ms==0)
		{
			ReadSensorTime_100ms=11;
			SensorData=Station_Sensor_Read();
			if(SensorData!=OldSensorData)//�д����������仯
			{
				OldSensorData=SensorData;
				
				CANTXMEGSS.CANTXBuff[0]=0x81;
				CANTXMEGSS.CANTXBuff[1]=0x01;//Ŀ�ص�ַ
				CANTXMEGSS.CANTXBuff[2]=Device_Prop.DEVICE_ID;//Դ��ַ
				CANTXMEGSS.CANTXBuff[3]=0x13;//��ʾ��SCU/DCU/ACU����CCU/TCU��״̬֡
				CANTXMEGSS.CANTXBuff[4]=0x02<<3;//
				CANTXMEGSS.CANTXBuff[5]=OldSensorData;//�����豸������״̬		
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
