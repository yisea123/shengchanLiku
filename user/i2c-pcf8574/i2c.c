#include "i2c.h"

#define TRUE 1
#define FALSE 0


#define byte unsigned char
#define ADDRESS_CHIP_WRITE 	0xde    
#define ADDRESS_CHIP_READ 	0xdf


// sbit SCL=P1^0;
// sbit SDA=P1^1;


/********************************************************
Name:Delay_5us
Description:This routine delay five microsecond
Input:      None
Output:     None
Calls:      _nop_()
Use Variant:None
Summary:     None
writer:     
********************************************************/

/********************************************************
Name:Delay_5ms
Description:This routine delay five millisecond
Input:      None
Output:     None
Calls:      _nop_()
Use Variant:None
Summary:     None
writer:
********************************************************/
//void Delay_5ms(void)
//{
//    unsigned int Time_Count;
//	uchar i;
//    Time_Count=1000;	
//    while (Time_Count--)
//	{
//		for(i=13;i>0;i--);
//		WDT();
//	}
//	
//}
/********************************************************
Name:Start_I2C
Description:This routine startup I2C BUS
Input:      None
Output:     None
Calls:      Delay_5us();
Use Variant:None
Summary:     None
writer:    
********************************************************/
void Start_I2C(void)
{
	PE7OUTPUT();
	SCL=HIGH;
	delay_us(10);
 	SDA_out=0;//START:when CLK is high,DATA change form high to low 
	delay_us(10);
	SCL = 0;//钳住I2C总线，准备发送或接收数据 
	delay_us(10);
}
/********************************************************
Name:Stop_I2C
Description:This routine stop I2C BUS
Input:      None
Output:     None
Calls:      Delay_5us();
Use Variant:None
Summary:     None
writer: 
********************************************************/
void Stop_I2C(void)
{
    SDA=LOW;
    delay_us(10);
    SCL=HIGH;
    delay_us(10);
    SDA=HIGH;
    delay_us(10);
    SCL=LOW;
    delay_us(10);
}
/********************************************************
Name:Init_I2C
Description:This routine initiate I2C BUS
Input:      None
Output:     None
Calls:      Stop_I2C
Use Variant:None
Summary:     None
writer:     
********************************************************/
// void Init_I2C(void)
// {
    // SCL=LOW;
    // Stop_I2C(0);
// }
/********************************************************
Name:Bit_I2C_Clock
Description:This routine execute one clock
Input:      None
Output:     None
Calls:      Delay_5us
Use Variant:None
Summary:     None
writer:    
********************************************************/
bool Bit_I2C_Clock(void)
{
    bool bitVar;
    SCL=HIGH;
    delay_us(10);
    bitVar=SDA;
    SCL=LOW;
    delay_us(10);
    return(bitVar);
}

bool Bit_I2C_Clock_Out(void)
{
    bool bitVar;
    SCL=HIGH;
    delay_us(10);
    //bitVar=SDA_out;
    SCL=LOW;
    delay_us(10);
    return(bitVar);
}
/********************************************************
Name:Send_Byte
Description:This routine sends one char to I2C BUS
Input:      Data_Sended
Output:     Success or False
Calls:      Bit_I2C_Clock()
Use Variant:None
Summary:     None
writer:    
********************************************************/
bool bolSend_Byte(unsigned char Data_Sended)
{
    unsigned char Count;
    for(Count=0;Count<8;Count++)
    {
        SDA_out = (bool)(Data_Sended&0x80);
        Data_Sended=Data_Sended<<1;
        Bit_I2C_Clock_Out();
    }
    SDA_out=HIGH;
    return(~Bit_I2C_Clock());
}
/********************************************************
Name:Recieve_Byte
Description:This routine receives one char from I2C BUS
Input:      None
Output:     Receive char(unsigned)
Calls:      bit_I2C_Clock
Use Variant:None
Summary:     None
writer:   
********************************************************/
unsigned char bytRec_Byte(unsigned char ack)
{
    unsigned char Data_Recieved;
    unsigned char Count;
		PE7INPUT();
    for(Count=0;Count<8;Count++)
    {
        Data_Recieved*=2;
        if(Bit_I2C_Clock()) Data_Recieved++;
    }
return(Data_Recieved);
		
//	unsigned char i,receive=0;
//	PE7INPUT();
//    for(i=0;i<8;i++ )
//	{
//        SCL=0; 
//        delay_us(10);
//				SCL=1;
//        receive<<=1;
//        if(READ_SDA)receive++;   
//		delay_us(10); 
//    }					 
////    if (!ack)
////        IIC_NAck();//发送nACK
////    else
////        IIC_Ack(); //发送ACK   
//    return receive;
}


