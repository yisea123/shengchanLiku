#include "24c256.h"



unsigned char IDcard_data;
/***********************************************************************
* ���� : I2C_delay()
* ���� : С��ʱ
* ���� : ��
* ��� : ��
***********************************************************************/
void I2C_delay(void)
{	
    u16 i=6; //��������Ż��ٶȣ���������͵�5����д��
    while(i)
    {
        i--;
    }
}

/***********************************************************************
* ���� : IIC_Init()
* ���� : ��ʼ��
* ���� : ��
* ��� : ��
***********************************************************************/
void IIC_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// ----------IIC ���Ŷ���---------------
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/***********************************************************************
* ���� : I2C_Start()
* ���� : ����һ����ʼ�ź�
* ���� : ��
* ��� : ��
***********************************************************************/
unsigned char I2C_Start(void)
{
    SDA_H;
		I2C_delay();
    SCL_H;
    I2C_delay();
//    if(!SDA_read) return 0;	//SDA��Ϊ�͵�ƽ������æ,�˳�
    SDA_L;
    I2C_delay();
//    if(SDA_read)  return 0;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
    SCL_L;
    I2C_delay();
    return 1;
}


/***********************************************************************
* ���� : I2C_Stop()
* ���� : ����һ��ֹͣ�ź�
* ���� : ��
* ��� : ��
***********************************************************************/
void I2C_Stop(void)
{
    SCL_L;
    I2C_delay();
    SDA_L;
    I2C_delay();
    SCL_H;
    I2C_delay();
    SDA_H;
    I2C_delay();
}

/***********************************************************************
* ���� : I2C_Ack()
* ���� : ����һ��Ӧ���ź�
* ���� : ��
* ��� : ��
***********************************************************************/
void I2C_Ack(void)
{	
    SDA_L;
    I2C_delay();
	
    SCL_H;
    I2C_delay();
		SCL_L;
    I2C_delay();
	
//    SDA_H;
//    I2C_delay();
}

/***********************************************************************
* ���� : I2C_NoAck()
* ���� : ����һ����Ӧ���ź�
* ���� : ��
* ��� : ��
***********************************************************************/
void I2C_NoAck(void)
{	
	  I2C_delay();
	
    SDA_H;
    I2C_delay();

//		SCL_L;
//    I2C_delay();
    SCL_H;
    I2C_delay();
	
    SCL_L;
    I2C_delay();
}

/***********************************************************************
* ���� : I2C_WaitAck()
* ���� : ��һ��Ӧ���ź�
* ���� : ��
* ��� : ����ΪTRUE��ACK,����FALSE��ACK
***********************************************************************/
unsigned char I2C_WaitAck(void) 	
{
    SCL_L;
    I2C_delay();
    SDA_H;			
    I2C_delay();
    SCL_H;
    I2C_delay();
    if(SDA_read)
    {
        SCL_L;
        return 0;
    }
    SCL_L;
		I2C_delay();
    return 1;
}

/***********************************************************************
* ���� : I2C_SendByte()
* ���� : ����һ���ֽ�
* ���� : ��
* ��� : ��
***********************************************************************/
void I2C_SendByte(u8 SendByte) //���ݴӸ�λ����λ
{
    u8 i=8;
    while(i--)
    {       
        I2C_delay();
        if(SendByte&0x80) SDA_H;
        else SDA_L;
        SendByte <<= 1;
        I2C_delay();
        SCL_H;
        I2C_delay();
				SCL_L;
    }
    I2C_delay();
}

/***********************************************************************
* ���� : I2C_ReceiveByte()
* ���� : ����һ���ֽ�
* ���� : ��
* ��� : ���յ�������
***********************************************************************/
unsigned char I2C_ReceiveByte(void)  //���ݴӸ�λ����λ
{
    u8 i=8;
    u8 ReceiveByte=0;

    SDA_H;				
    while(i--)
    {
        ReceiveByte<<=1;  
				I2C_delay();     
        SCL_H;
        I2C_delay();	
        if(SDA_read)
        {
            ReceiveByte|=0x01;
        }
				I2C_delay();
				SCL_L;			
    }
    I2C_delay();
    return ReceiveByte;
}

/***********************************************************************
* ���� : I2C_WriteByte()
* ���� : ��EEPROMдһ���ֽ�
* ���� : SendByteҪ���͵����ݣ�WriteAddressҪд��ĵ�ַ��DeviceAddress��������ַ
* ��� : TRUE��ȷ��FALSE����ʧ��
***********************************************************************/
//unsigned char I2C_WriteByte(u8 SendByte, u16 WriteAddress, u8 DeviceAddress)
//{		
//    if(!I2C_Start())return 0;
//    I2C_SendByte(((WriteAddress & 0x0700) >>7) | DeviceAddress & 0xFFFE);//���ø���ʼ��ַ+������ַ
//    if(!I2C_WaitAck()){I2C_Stop(); return 0;}
//    I2C_SendByte((u8)(WriteAddress & 0x00FF));   //���õ���ʼ��ַ
//    I2C_WaitAck();	
//    I2C_SendByte(SendByte);
//    I2C_WaitAck();
//    I2C_Stop();
//	//ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms)
//    //Systick_Delay_1ms(10);
//	delay_ms(10);
//    return 1;
//}
unsigned char I2C_BufferWrite_Onebyte(u8 *pBuffer, u16 WriteAddress)
{
	static unsigned char WriteTimes=0;
	
write1:	
	WriteTimes++;
  I2C_Start();
	I2C_SendByte(0xa0);//���ø���ʼ��ַ+������ַ
  if(!I2C_WaitAck())
	{
		if(WriteTimes<200)goto write1;//?? 
		else return 0;
	}
  I2C_SendByte(WriteAddress>>8);   //���ø���ʼ��ַ
  if(!I2C_WaitAck())
	{
		if(WriteTimes<200)goto write1;//?? 
		else return 0;
	}
  I2C_SendByte(WriteAddress & 0x00FF);   //���õ���ʼ��ַ
  if(!I2C_WaitAck())
	{
		if(WriteTimes<200)goto write1;//?? 
		else return 0;
	}
	I2C_SendByte(0xff - *pBuffer);
	if(!I2C_WaitAck())
	{
		if(WriteTimes<200)goto write1;//?? 
		 else return 0;
	}
//	pBuffer++;
//	WriteAddress++;
	I2C_Stop();
	
	//ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms)
	//Systick_Delay_1ms(10);
	DelayTimes_ms=10;
	while (DelayTimes_ms);
	return 1;
}
/***********************************************************************
* ���� : I2C_BufferWrite()
* ���� : ��EEPROMҳдһ���ֽڣ����ܿ�ҳд��һҳ��8���ֽ�����
* ���� : * pBufferҪ���͵����ݣ�length�������ݵĸ�����
         WriteAddressҪд��ĵ�ַ��DeviceAddress��������ַ
* ��� : TRUE��ȷ��FALSE����ʧ��
***********************************************************************/
unsigned char I2C_BufferWrite(u8 *pBuffer, u16 length, u16 WriteAddress)
{
	static unsigned char WriteTimes=0;
	static unsigned int length_temp=0;
	static unsigned int  WriteAddress_temp;
	length_temp=length;
	WriteAddress_temp=WriteAddress;
write1:	
	if(length>=length_temp)WriteAddress_temp=WriteAddress_temp+(length-length_temp);
	WriteTimes++;
  I2C_Start();
	I2C_SendByte(0xa0);//���ø���ʼ��ַ+������ַ
  if(!I2C_WaitAck())
	{
		if(WriteTimes<200)goto write1;//?? 
		else return 0;
	}
  I2C_SendByte(WriteAddress_temp>>8);   //���ø���ʼ��ַ
  if(!I2C_WaitAck())
	{
		if(WriteTimes<200)goto write1;//?? 
		else return 0;
	}
  I2C_SendByte(WriteAddress_temp & 0x00FF);   //���õ���ʼ��ַ
  if(!I2C_WaitAck())
	{
		if(WriteTimes<200)goto write1;//?? 
		else return 0;
	}
	while(length_temp)
	{
		I2C_SendByte(0xff - *pBuffer);
		if(!I2C_WaitAck())
		{
//			if(WriteTimes<200)goto write1;//?? 
//			 else return 0;
		}
		pBuffer++;
		length_temp--;
	}
	I2C_Stop();
	
	//ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms)
	//Systick_Delay_1ms(10);
	DelayTimes_ms=10;
	while (DelayTimes_ms);
	return 1;

//	unsigned int i;
//	
//	for(i=0;i<length;i++)
//	{
//		I2C_BufferWrite_Onebyte(pBuffer+i, WriteAddress+i);
//	}

//	return 1;

}

/***********************************************************************
* ���� : I2C_PageWrite()
* ���� : ��EEPROMҳдһ���ֽڣ�����ҳд
* ���� : * pBufferҪ���͵����ݣ�length�������ݵĸ�����
         WriteAddressҪд��ĵ�ַ��DeviceAddress��������ַ
         24C256   page 64byte
				 24C512   page 128byte******************************************
         24C1024  page 256byte
* ��� : TRUE��ȷ��FALSE����ʧ��
***********************************************************************/
void I2C_PageWrite(u8* pBuffer, u16 length, u16 WriteAddress)
{
	unsigned int I2C_PageSize=64;
	unsigned char NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0;
	Addr  = WriteAddress % I2C_PageSize;      //д���ַ�ǿ�ʼҳ�ĵڼ�λ
	count = I2C_PageSize - Addr;					    //�ڿ�ʼҳҪд��ĸ���(��д���ַд��д��ҳ�ĸ���)
	NumOfPage   =  length / I2C_PageSize;     //Ҫд���ҳ��
	NumOfSingle =  length % I2C_PageSize;     //����һҳ�ĸ���

	if(Addr == 0)         //д���ַ��ҳ�Ŀ�ʼ
	{
		if(NumOfPage == 0)  //����С��һҳ
		{
			I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress);   //д����һҳ������
			DelayTimes_ms=12;
			while (DelayTimes_ms);
		}
		else		            //���ݴ��ڵ���һҳ
		{
			while(NumOfPage)//Ҫд���ҳ��
			{
				I2C_BufferWrite(pBuffer,I2C_PageSize,WriteAddress);//дһҳ������
				WriteAddress +=  I2C_PageSize;
				pBuffer      +=  I2C_PageSize;
				NumOfPage--;
				DelayTimes_ms=12;
				while (DelayTimes_ms);
			}
			if(NumOfSingle!=0)//ʣ������С��һҳ
			{
				I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress); //д����һҳ������
				DelayTimes_ms=12;
				while (DelayTimes_ms);
			}
		}
	}
	else                  //д���ַ����ҳ�Ŀ�ʼ
	{
		if(NumOfPage== 0)   //����С��һҳ
		{
			if(NumOfSingle<=count)I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress);   //д�����ݲ��ᳬ��ҳ�߽�
			else                                                                       //д�����ݻᳬ��ҳ�߽�
			{
				I2C_BufferWrite(pBuffer,count,WriteAddress);      //����ʼ�ĵ�ַд��һҳ  count=�ӿ�ʼҳҪд��ĸ���(��д���ַд��д��ҳ�ĸ���)
				WriteAddress += count;
				pBuffer      += count;
				DelayTimes_ms=12;
				while (DelayTimes_ms);
				I2C_BufferWrite(pBuffer,NumOfSingle-count,WriteAddress);   //д��ҳ��ʣ������
			}
			DelayTimes_ms=12;
			while (DelayTimes_ms);
		}
		else                //���ݴ��ڵ���һҳ
		{
			length       -= count;
			NumOfPage     = length / I2C_PageSize;  //���¼���Ҫд���ҳ��
			NumOfSingle   = length % I2C_PageSize;  //���¼��㲻��һҳ�ĸ���	

			if(count != 0)
			{
				I2C_BufferWrite(pBuffer,count,WriteAddress);      //����ʼ�Ŀռ�д��һҳ
				WriteAddress += count;
				pBuffer      += count;
				DelayTimes_ms=12;
				while (DelayTimes_ms);
			}

			while(NumOfPage--)  //Ҫд���ҳ��
			{
				I2C_BufferWrite(pBuffer,I2C_PageSize,WriteAddress);//дһҳ������
				WriteAddress +=  I2C_PageSize;
				pBuffer      +=  I2C_PageSize;
				DelayTimes_ms=12;
				while (DelayTimes_ms);
			}
			if(NumOfSingle != 0)//ʣ������С��һҳ
			{
				I2C_BufferWrite(pBuffer,NumOfSingle,WriteAddress); //д����һҳ������
				DelayTimes_ms=12;
				while (DelayTimes_ms);
			}
		}
	}

//I2C_BufferWrite(pBuffer, length, WriteAddress);
}

/***********************************************************************
* ���� : I2C_PageWrite()
* ���� : ��ȡEEPROM����
* ���� : *pBuffer���յ������ݵ�ַָ�룬length�������ݵĸ�����
         WriteAddressҪ�����ݵĵ�ַ��DeviceAddress��������ַ
* ��� : TRUE����ȷ��FALSE��ȡʧ��
* ʹ�� ��I2C_ReadByte(&i, 1, 0x0002, 0xa0);  &iΪ����i�ĵ�ַ
***********************************************************************/
unsigned char I2C_ReadByte(u8* pBuffer,   u16 length,     u16 ReadAddress)
{	
		static unsigned char ReadTimes=0;
	
read1:  
		ReadTimes++;	
		I2C_Start();
		I2C_SendByte(0xa0);//���ø���ʼ��ַ+������ַ
		if(!I2C_WaitAck())
		{
			if(ReadTimes<200)goto read1;//?? 
			else return 0;
		}
    I2C_SendByte(ReadAddress>>8);                           //���ø���ʼ��ַ
		if(!I2C_WaitAck())
		{
			if(ReadTimes<200)goto read1;//?? 
			else return 0;
		}
    I2C_SendByte(ReadAddress & 0x00FF);                           //���õ���ʼ��ַ
		if(!I2C_WaitAck())
		{
			if(ReadTimes<200)goto read1;//?? 
			else return 0;
		}

    I2C_Start();
    I2C_SendByte(0xa1);
		if(!I2C_WaitAck())
		{
			if(ReadTimes<200)goto read1;//?? 
			else return 0;
		}
    while(length)
    {
		*pBuffer = 0xff-I2C_ReceiveByte();
		if(length == 1)I2C_NoAck();
		else I2C_Ack();
		pBuffer++;
		length--;
    }
    I2C_Stop();
//	delay_ms(2);
    return 1;
}






