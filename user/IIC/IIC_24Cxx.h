#ifndef __24CXX_H
#define __24CXX_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdbool.h>

#define SCL_H         GPIOB->BSRR = GPIO_Pin_8
#define SCL_L         GPIOB->BRR  = GPIO_Pin_8 
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_9
#define SDA_L         GPIOB->BRR  = GPIO_Pin_9

#define SCL_read      GPIOB->IDR  & GPIO_Pin_8
#define SDA_read      GPIOB->IDR  & GPIO_Pin_9

#define I2C_PageSize  8  //24C02Ã¿Ò³8×Ö½Ú

void IIC_Init(void);
bool I2C_WriteByte(u8 SendByte, u16 WriteAddress, u8 DeviceAddress);
bool I2C_BufferWrite(u8* pBuffer, u8 length, u16 WriteAddress, u8 DeviceAddress);
void I2C_PageWrite(u8* pBuffer, u8 length, u16 WriteAddress, u8 DeviceAddress);
bool I2C_ReadByte(u8* pBuffer, u8 length, u16 ReadAddress, u8 DeviceAddress);

#endif 



