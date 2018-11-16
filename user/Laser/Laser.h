#ifndef __LASER_H
#define __LASER_H

#include "sys.h"
#include "delay.h"
#include "Sys_Init.h"
enum	UART2_MODE{LCD = 1,Laser =2};
extern enum	UART2_MODE Uart2_Mode;
extern u8 distance_data[2];//发送数据显示格式 CCC.M cm ; distance_data[0] = CCC; distance_data[1] = M;

extern u8 Lrecflag;
extern u8 distance_data[2];//发送数据显示格式 CCC.M cm ; distance_data[0] = CCC; distance_data[1] = M;
extern u8 rec_num;
extern u8 rec_len;
extern u8 recdata[4];//接收缓存


void Laser_Init(void);				//激光模块初始化
int Read_Laser(void);				//读取测量距离
int SamplingAverage(int *Sampling);//取平均


#endif

