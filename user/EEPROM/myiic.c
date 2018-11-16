#include "myiic.h"
#include "delay.h"
 
//��ʼ��IIC
void IIC_init(void)
{					     	
	RCC->APB2ENR|=1<<3;//��ʹ������IO PORTBʱ�� 							 
	GPIOB->CRH&=0XFFFFFF00;//PB8/9 �������
	GPIOB->CRH|=0X00000033;	   
	GPIOB->ODR|=3<<8;     //PB10,11 �����
}
void My_IIC_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7 | GPIO_Pin_9;//PE7��I2Cʱ����,PE9��I2C������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 				//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;				//IO���ٶ�Ϊ50MHz
 	GPIO_Init(GPIOE, &GPIO_InitStructure);									//��ʼ��GPIOEA
	GPIO_SetBits(GPIOE, GPIO_Pin_7 | GPIO_Pin_9);				//IO���0
}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
 	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	 

//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	SCL=0;
	PE7OUTPUT();
	SDA_out=0;
	delay_us(2);
	SCL=1;
	delay_us(2);
	SCL=0;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	SCL=0;
	PE7OUTPUT();
	SDA_out=1;
	delay_us(2);
	SCL=1;
	delay_us(2);
	SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			IIC_SDA=1;
		else
			IIC_SDA=0;
		txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        IIC_SCL=0; 
        delay_us(2);
		IIC_SCL=1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}

//��ʼ��IIC�ӿ�
void AT24CXX_Init(void)
{
	IIC_Init();
}

//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ  
//����ֵ  :����������
u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{				  
	u8 temp=0;		  	    																 
  IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	   //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
//		IIC_Wait_Ack();		 
	}else IIC_Send_Byte(0XA0+((ReadAddr/256)<<1));   //����������ַ0XA0,д���� 	 

	IIC_Wait_Ack(); 
  IIC_Send_Byte(ReadAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack();	    
	IIC_Start();  	 	   
	IIC_Send_Byte(0XA1);           //�������ģʽ			   
	IIC_Wait_Ack();	 
  temp=IIC_Read_Byte(0);		   
  IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}

////////////////////////////////////////////////////////////////////////
//��ָ����ַ�������� 4���ֽ�
//Sadd:��ʼ��ַ
//n:��ȡ�ֽ���
u32 AT24CXX_ReadNByte(u16 Sadd, u8 n)
{
	u8 i;
	u32 Temp = 0;
	for(i=0; i<n; i++)
	{
		AT24CXX_ReadOneByte(Sadd + i);
		Temp |= ((AT24CXX_ReadOneByte(Sadd + i))<<(0+8*i));
	}
	
	return Temp;
}

//��ָ��λ�����ֽ�д�� 4���ֽ�
//Sadd:��ʼ��ַ
//n:д���ֽ���
void AT24CXX_WriteNByte0(u16 Sadd, u8 n, u8 data)
{
	u8 i;
//	u32 Temp = 0;
	for(i=0; i<n; i++)
	{
		AT24CXX_WriteOneByte(Sadd + i, data);
//		Temp |= (AT24CXX_ReadOneByte(Sadd + i))<<(0+8*i);
	}
//	return Temp;
}

//��һ��32λ��(4���ֽ�)д��EEPROMָ����ַ
//Sadd:��ʼ��ַ
void AT24CXX_Write4Byte(u16 Sadd, u32 data)
{	
//����1	
//	AT24CXX_WriteOneByte(Sadd+0, temp/(256*256*256));			
////	AT24CXX_WriteOneByte(Sadd+1, (temp%256)/(256*256)); 
//	AT24CXX_WriteOneByte(Sadd+1, (temp&0x00ffffff)/(256*256));
//	AT24CXX_WriteOneByte(Sadd+2, temp%(256*256)/(256));
//	AT24CXX_WriteOneByte(Sadd+3, temp%(256)%(256)%(256)%(256));

//����2	
//	AT24CXX_WriteOneByte(Sadd+0, (temp&0xff000000)/(256*256*256));  ��λ
//	AT24CXX_WriteOneByte(Sadd+1, (temp&0x00ff0000)/(256*256));
//	AT24CXX_WriteOneByte(Sadd+2, (temp&0x0000ff00)/256);
//	AT24CXX_WriteOneByte(Sadd+3, (temp&0x000000ff)%256);	//��λ

	AT24CXX_WriteOneByte(Sadd+0, (data&0x000000ff)%256);
	AT24CXX_WriteOneByte(Sadd+1, (data&0x0000ff00)/256);
	AT24CXX_WriteOneByte(Sadd+2, (data&0x00ff0000)/(256*256));
	AT24CXX_WriteOneByte(Sadd+3, (data&0xff000000)/(256*256*256));
}
//////////////////////////////////////////////////////////////////////////

//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ    
//DataToWrite:Ҫд�������
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{				   	  	    																 
  IIC_Start();  
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0);	    //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
 	}
	else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1));   //����������ַ0XA0,д���� 
	}	 
	IIC_Wait_Ack();	   
  IIC_Send_Byte(WriteAddr%256);   //���͵͵�ַ
	IIC_Wait_Ack(); 	 										  		   
	IIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	IIC_Wait_Ack();  		    	   
  IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}

//��AT24CXX�����ָ����ַ��ʼд�볤��ΪLen������
//�ú�������д��16bit����32bit������.
//WriteAddr  :��ʼд��ĵ�ַ  
//DataToWrite:���������׵�ַ
//Len        :Ҫд�����ݵĳ���2,4
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{  	
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	}												    
}

//��AT24CXX�����ָ����ַ��ʼ��������ΪLen������
//�ú������ڶ���16bit����32bit������.
//ReadAddr   :��ʼ�����ĵ�ַ 
//����ֵ     :����
//Len        :Ҫ�������ݵĳ���2,4
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{  	
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 	 				   
	}
	return temp;												    
}
//���AT24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(32767);//����ÿ�ο�����дAT24CXX			   
	if(temp==0X55)return 0;		   
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(32767,0X55);
	    temp=AT24CXX_ReadOneByte(32767);	  
		if(temp==0X55)return 0;
	}
	return 1;											  
}

//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++);	
		NumToRead--;
	}
}  
//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}


