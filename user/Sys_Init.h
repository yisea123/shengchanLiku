#ifndef __SYS_INIT
#define __SYS_INIT

#define GATEWAY_VERSION 		0x12  //���ذ汾 ���ԣ�1.2
#define TERMINAL_VERSION 		0x10  //�ն˰汾 ���ԣ�1.0(������ͬʱ���ƣ��������ط���)

#include "sys.h"
#include "delay.h"
#include "mydata.h"
#include "stm32f10x_exti.h"
#include "Beep.h"
#include "Key.h"
#include "led.h"
#include "Relay.h"
#include "timer.h"
#include "pwm.h"
#include "output.h"
#include "input.h"
#include "Action_Status.h"
#include "network.h"
#include "wdg.h"
#include "LCD_RS232.h"
#include "i2c.h"
#include <string.h>
#include <stdio.h>
#include "24c256.h"
#include "Auto_Locate.h"
#include "Laser.h"
void Sys_Init(void);

#endif

