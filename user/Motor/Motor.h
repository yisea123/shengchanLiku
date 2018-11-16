#ifndef __MOTOR_H
#define __MOTOR_H

#include "sys.h"
#include <stdbool.h>


/**********宏定义**********/
#define STEPMOTO_X  0
#define STEPMOTO_Y  1
#define STEPMOTO_Z  2
#define STEPMOTO_W  3
#define STEPMOTO_V  4

#define _FAS_DISTANCE	     110000  //自动存样
#define POSITION_ZERO      0      //零位坐标值
#define POSITION_SEND      110000 //发送位坐标值
#define POSITION_BOTTLECAP 110010 //用于移动到瓶盖检测位的步数，应大于小瓶瓶盖的步数
                                  //为了避免意外计算到该值，设置为大于最大步数的特殊值
																	
#define AES16A_DISTANCE    165000  //自动取样工作站(有滑动阀)行程			

#define AES16D_DISTANCE    200000  //自动取样工作站(无滑动阀)行程																	
#define AES16D_Suction     40000   //自动取样工作站吸气动作滑台位置											
																	
#define AES16F_DISTANCE    61500
#define AES16F_LAYER_1     61500
#define AES16F_LAYER_2     40800	
#define AES16F_LAYER_3     19900
#define AES16F_Suction     10000   //自动取样工作站吸气动作滑台位置											
#define AES16F_LAYER_SEND  0
	
	
#define DISTANCE_CAP_TO_CAPTEST 25000 //从瓶盖位置到瓶盖检测点应走的距离
#define AMpulnum			     5600	  //瓶子判断区间上下限
#define RFID_Read_Block    0x08  //RFID读取块区域

/*********全局变量声明**********/
extern u32 Addnum;						//步数(脉冲累计)
extern u32 Pulplus;	
extern u16 BadBottle_Reject_timer; //不良瓶剔除超时时间
extern bool First_Bootlecaptest; //瓶盖测试

//===================== 滑台步进电机控制器IO口定义区 =============================//

#define SM_EN			PBout(13)	//电机使能端	PB.13
#define SM_DIR		PBout(14)	//方向				PB.14
#define SM_PUL		PBout(15)	//脉冲				PB.15

#define SM_ENH		SM_EN = 1
#define SM_ENL		SM_EN = 0

#define SM_DIRH		SM_DIR = 1	
#define SM_DIRL		SM_DIR = 0

#define SM_PULH		SM_PUL = 1
#define SM_PULL		SM_PUL = 0

#define AES16F_BRAKE_H   LED_R = 1   //AES16F 刹车用红灯端口控制
#define AES16F_BRAKE_L   LED_R = 0   
//======================== 皮带电机IO口定义区 ==============================//

//驱动器L6203控制引脚	
#define MO_EN 		GPIO_Pin_12		//PE12	使能		
#define MO_SE			GPIO_Pin_12		//PC12	过流检测
#define MO_IN1 		GPIO_Pin_14		//PE14
#define MO_IN2 		GPIO_Pin_15		//PE15

//电机引脚控制  
#define MENH      GPIO_WriteBit(GPIOE, MO_EN, Bit_SET)
#define MENL      GPIO_WriteBit(GPIOE, MO_EN,	Bit_RESET)
#define MIN1H     GPIO_WriteBit(GPIOE, MO_IN1, Bit_SET)
#define MIN1L     GPIO_WriteBit(GPIOE, MO_IN1, Bit_RESET)
#define MIN2H     GPIO_WriteBit(GPIOE, MO_IN2, Bit_SET)
#define MIN2L     GPIO_WriteBit(GPIOE, MO_IN2, Bit_RESET)
#define Read_OC   GPIO_ReadInputDataBit(GPIOC, MO_SE)		//过流检测

//电机控制
#define Motor_F   {MIN1H; MIN2L; MENH;} //电机正转 F
#define Motor_R   {MIN1L; MIN2H; MENH;} //电机反转 R
#define Motor_S   {MIN1L; MIN2L; MENH;} //停转（并刹车）
#define Motor_OFF	{MIN1L; MIN2L; MENL;} //停转（无刹车）


//======================== 滑动阀减速电机IO口定义区 ==============================//
#define MotoGobackPul_NORMAL  4000        //回零位继续走步数1CM
#define MotoGobackPul_AES16F  400        //回零位继续走步数1CM

//#define SPEED             35
#define SPEED_AES16F      140

#define DERICT_FRONT      0
#define DERICT_BACK       1

#define ROD_PUSH          1   //推杆推
#define ROD_BACK          2   //推杆收

#define VALVE_CLOSE       1   //滑动阀关
#define VALVE_OPEN        2   //滑动阀开
//============================== 函数定义区 ======================================//
void Motor_Init(void);				//电机IO初始化
unsigned char StepperMoto_42(unsigned long StepNum,unsigned char Derict);
void MoveToPosition(u32 Position);
u8 BadBottle_Reject(void);
u8 PushRod_Ctrl(u8 Position, u8 Speed);
u8 Valve_Ctrl(u8 Position);
u16 Motor_SpeedCtrl(u32 OverallStep,u32 NowStep);
u8 Rod_Ctrl_AES16F(u8 Position);
void Rod_Ctrl_PUSH_AES16F(void);
#endif

