#ifndef __PWM_H
#define __PWM_H
#include "sys.h"
#include "Sys_Init.h"
//#define Dir_Y PEout(4)
#define DirFount_Y()    Dir_Y = 1     //从零位向前走
#define DirBack_Y()     Dir_Y = 0   //回零位



extern TIM_OCInitTypeDef  TIM_OCInitStructure;
extern TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
/**************三轴方向*****************/
#define	DIR_X			PCout(5) 
#define	DIR_Y			PAout(4)
#define	DIR_Z			PBout(12)

/**************报警信号*****************/
#define ALARM_Y PAin(6)

/**************传感器*****************/
#define Proximity_3		PBin(5)	//托盘接近开关
#define Proximity_2		PBin(4) //托盘接近开关
#define Proximity_1		PBin(3) //托盘接近开关

#define Axis_Z_Up_Sensor 							P5_IN//Z轴上限位
#define Axis_Z_UpLim_Sensor						PEin(15)//Z轴上限位极限位，如触发则进入异常情况，向上位机报警
#define Axis_Z_Down_Sensor						P6_IN//Z轴下限位 
#define Axis_X_Forward_Sensor					PDin(6) //X轴前限位
#define Axis_X_ForwardLim_Sensor			PCin(13)//X轴前限位极限位，如触发则进入异常情况，向上位机报警
#define Axis_X_Backward_Sensor				PEin(14)//X轴后限位
#define Axis_X_BackwardLim_Sensor			PEin(3)//X轴后限位极限位，如触发则进入异常情况，向上位机报警



#define PWM_PCS       23   //PWM频率=72000000/(PCS+1)*(ARR+1) 
#define Z_Acc_Time_SL 50//超低速模式加速时间
#define Z_Acc_Time_L  200//低速模式加速时间
#define Z_Acc_Time_M  500//中速模式加速时间
#define Z_Acc_Time_H	800//高速模式加速时间

#define X_Acc_Time_L  200//低速模式加速时间
#define X_Acc_Time_M  500//中速模式加速时间
#define X_Acc_Time_H	1000//高速模式加速时间

/*****************************X轴控制参数**************************/
#define X_DIV         					240     //驱动器设置细分
#define X_STAR_FREQ     				100    	//初始频率 提供初始频率进入外部中断
#define X_END_FREQ							100
#define X_PULSE_AFTZERO					10			//到达零点后前进的脉冲数
//#define X_MPP										0.4400		//mm per pulse 毫米每脉冲0.6865
extern double X_MPP;
#define X_DIR_BACK							0
#define X_DIR_FRONT							1
//超高速
#define X_SPEED_FREQ_SH					2600
#define X_SPHIGHSPEED_DISTANCE  3000
#define X_HORL_PULSE_VALVE_SH		(X_SPHIGHSPEED_DISTANCE/X_MPP)
//高速
#define X_SPEED_FREQ_H  				2300   //高速频率 
#define X_HIGHSPEED_DISTANCE		2000  //高速前进距离阈值单位Mm
#define X_HORL_PULSE_VALVE_H   	(X_HIGHSPEED_DISTANCE/X_MPP)  //高速总脉冲数阀值，用来分辨是否高速行进2185
//中速
#define X_SPEED_FREQ_M  				 800   //目标频率 
#define X_MIDDLESPEED_DISTANCE	1500  //中速前进距离阈值单位Mm
#define X_HORL_PULSE_VALVE_M   	(X_MIDDLESPEED_DISTANCE/X_MPP)  //中速总脉冲数阀值，用来分辨是否中速行进1456
//低速
#define X_SPEED_FREQ_L					500    //低速频率 
#define X_LOWSPEED_DISTANCE     500		////低速前进距离阈值单位Mm
#define X_HORL_PULSE_VALVE_L  	(X_LOWSPEED_DISTANCE/X_MPP)  	//低速总脉冲数阀值，用来分辨是否中速行进
//#define	X_Zero_Pluse						25		//X轴到达零点后前进脉冲数
extern u32 X_Zero_Pluse;



/*****************************Z轴控制参数**************************/
#define Z_DIV         					120     //驱动器设置细分
#define Z_STAR_FREQ     				50    	//初始频率 提供初始频率进入外部中断
#define Z_END_FREQ							50
//#define Z_MPP										0.3395		//mm per pulse 毫米每脉冲
extern double Z_MPP;
#define Z_DIR_FRONT							1
#define Z_DIR_BACK							0
#define Z_ZeroUp_Pluse					200//Z轴归位上升脉冲数


//高速
#define Z_SPEED_FREQ_H  				2000  	//高速频率  
#define Z_HIGHSPEED_DISTANCE		1500
#define Z_HORL_PULSE_VALVE_H   	(Z_HIGHSPEED_DISTANCE/Z_MPP)  //高速总脉冲数阀值，用来分辨是否高速行进
//中速
#define Z_SPEED_FREQ_M 					600   //目标频率 � 
#define Z_MIDDLESPEED_DISTANCE	1000
#define Z_HORL_PULSE_VALVE_M   	(Z_MIDDLESPEED_DISTANCE/Z_MPP)  //中速总脉冲数阀值，用来分辨是否中速行进
//低速
#define Z_SPEED_FREQ_L  				200   //低速频率
#define Z_LOWSPEEDDISTANCE			500
#define Z_HORL_PULSE_VALVE_L    (Z_LOWSPEEDDISTANCE/Z_MPP)	//低速总脉冲数阀值，用来分辨是否高速行进
//超低速用来托举
#define Z_SPEED_FREQ_SL         120
#define Z_SLDISTANCE						100
#define Z_HORL_PULSE_VALVE_SL   (Z_SLDISTANCE/Z_MPP)


/*****************************Y轴控制参数**************************/
#define Y_DIV         					120     //驱动器设置细分
#define Y_STAR_FREQ     				100    	//初始频率 提供初始频率进入外部中断
#define Y_END_FREQ							100
#define Y_DIR_LEFT							1//Y轴行走的方向为左
#define Y_DIR_RIGHT							0//Y轴行走的方向为右

//#define Y_LeftLim_MorePluse					2200//Y轴从原点往左走的脉冲数	2180
//#define Y_LeftLim_LessPluse					2165//Y轴从原点往左走的脉冲数	2150
//#define Y_RightLim_MorePluse				2250//Y轴从原点往右走的脉冲数	2250
//#define Y_RightLim_LessPluse				2190//Y轴从原点往右走的脉冲数
extern u32 Y_LeftLim_MorePluse;
extern u32 Y_LeftLim_LessPluse;
extern u32 Y_RightLim_MorePluse;
extern u32 Y_RightLim_LessPluse; 
//#define Y_RightZero_Pluse				60//Y轴从左往右回零继续行走的脉冲数
//#define Y_LeftZero_Pluse				80//Y轴从右往左回零继续行走的脉冲数
extern u32 Y_RightZero_Pluse;
extern u32 Y_LeftZero_Pluse;
//#define Y_LeftTrayPluse					2250
extern u32 Y_LeftTrayPluse;
//高速
#define Y_SPEED_FREQ  				800   	//高速频率 
#define Y_HORL_PULSE_VALVE_H   	3039  //高速总脉冲数阀值，用来分辨是否高速行进
#define Y_ACC_TIME						500
#define Y_MPP								0.09

#define AXIS_X 0
#define AXIS_Z 1
#define AXIS_Y 2


typedef struct 
{
  u8 InCtrl_Flag;           	//控制中标志 0:不受控 1:短距离，不加减速，慢速模式 2:长距离，加减速，快速模式
  u32 NowPulse;               //当前已走脉冲数
  float NowFreq;              //当前频率
  float NowACC;
  u32 Target_Pulse;           //目标脉冲数    
  u32 Timer_Cnt;              //当前已走时间
  u32 PulsePoint[7];          //过程时间点的脉冲数，分别为加加速完成时间点，匀加速完成时间点，减加速完成时间点
  u32 TimerPoint[7];          //过程时间点
  int32_t YCoordinate;        //Y轴坐标
  float FreqPoint[7];        	//加速过程时间点频率
  u16 Time_Delta;             //与上个时间点的时间差
  u8  SpeedChange_Stage;      //加减速过程3个阶段 1：加速过程 2：减速过程
	u16 Target_Fre;							//目标频率
	float Jerk_Speed;						//加加速度
	float Acc_Speed;							//匀速加速度
  int32_t Coordinate;							//当前坐标
	bool Dir;
	bool	CtrlEnd_Flag;
	float Stage4_Acc;
	float Stage1_Acc;
}TYPE_StepMotor;



typedef struct
{
	u8 command;//转工位指令
	u8 tray_position;//托盘属性
	u8 Put_Box_Stage;//放箱动作阶段
	u8 Get_Box_Stage;//取箱动作阶段
	u8 XZRun_Stage;//XZ运动嵌套阶段
	u8 XZRUN_Status_Change;//XZ运动状态改变
	u8 Tray_PutBox_Stage;//托盘放箱嵌套阶段
	u8 Tray_PutBox_Status_Change;//托盘放箱状态改变
	u8 Tray_GetBox_Stage;//托盘取箱阶段
	u8 Tray_GetBox_Status_Change;//托盘取箱状态改变
	u8 PutBox_Cnt;//放箱运行次数，用于偷偷复位
	u8 GetBox_Cnt;//取箱运行次数，用于偷偷复位
	u8 PutBox_Status_Change;//偷偷复位用的
	u8 GetBox_Status_Change;
	bool end_flag;//嵌套动作结束标志
	bool Get_Box_Start_Flag;//抓箱指令开始标志
	bool Put_Box_Start_Flag;//放箱指令开始标志
	bool Move_Alone;//单点运动状态
	u16 Target_X;//X轴坐标，单位mm
	u16 Target_Z;//Z轴坐标，单位mm
	u16 Comend;
	u8 Speed_Grade;//速度档位
	u16 Step_Grade;//步长档位
	u16 PlatZ_Axis;//托架Z坐标
	u16 PlatX_Axis;//托架Z坐标
}TYPEDEF_ACTION;



extern TYPEDEF_ACTION Action;
extern TYPE_StepMotor Axis_X;
extern TYPE_StepMotor Axis_Z;
extern TYPE_StepMotor Axis_Y;
extern u8 Motor_CtrlOver_Flag;		//电机控制结束标志，此标志置1后，上传结束控制应答
extern u8 NowAngle_A;
extern u8 NowAngle_B;
extern bool x_stop_flag;

void PWM_Init(void);
void PWM_Set_X(u32 Arr,u32 Psc);
void PWM_Set_Y(u32 Arr,u32 Psc);
void PWM_Set_Z(u32 Arr,u32 Psc);
void Motor_MoveToPosition(u16 AXIS_X_Mm,u16 AXIS_Z_Mm);
void StepMotorCtrl_Pulse(u32 Target,u8 AXIS,bool Dir);
void StepMotorCtrl_Zero_Pulse(u32 Target ,bool Dir);
void Motor_BackToZero(u8 Axis);
void Motor_Y_BackToLeft(void);
void Tray_Limit_Contr(void);
void Axis_Ultra_Limit(void);
void Uniform_Speed_Advance(u32 Tar_Pluse,u8 Axis,bool dir);

u32 PWMCtrl_ARRCount(u32 pcs, float Fre);


#endif
