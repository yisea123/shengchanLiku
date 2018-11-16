/********模拟程序*********/

#include "Analoy.h"

struct_ANALOYCTRL Analoy;


//模拟控制程序 1ms调用一次
void Analoy_Ctrl()
{
  if(Analoy.DelayTimer == 1)
  {
    Analoy.DelayTimer = 0;
    //改变设备状态
    switch(Analoy.Action)
    {
      //正转
      case 0x80:
      {
        Device[Analoy.CANId].DEVICE_STATUS &=(~0x08); //方向正
        Device[Analoy.CANId].DEVICE_STATUS |= 0x04;    //开电机
      }break;
      //反转
      case 0x40:
      {
        Device[Analoy.CANId].DEVICE_STATUS |= 0x08;    //方向反
        Device[Analoy.CANId].DEVICE_STATUS |= 0x04;    //开电机
      }break;
      //停止
      case 0x20:
      {
        Device[Analoy.CANId].DEVICE_STATUS |= 0x08;    //方向反
        Device[Analoy.CANId].DEVICE_STATUS &= (~0x04); //关电机
      }break;
      //
      case 0x08:
      {
        if((Analoy.CANId >= 0x10) && (Analoy.CANId <= 0x1f))      //动力轨道-无
        {
          Beep(100,100,2);
          return;
        }
        else if((Analoy.CANId >= 0x20) && (Analoy.CANId <= 0x2f)) //工作站-轨道开
        {
          Device[Analoy.CANId].DEVICE_STATUS |= 0x40; //开合轨道合
        }
        else if((Analoy.CANId >= 0x30) && (Analoy.CANId <= 0x3f)) //1T旋轨器-无
        {
          Beep(100,100,2);
          return;
        }
        else if((Analoy.CANId >= 0x40) && (Analoy.CANId <= 0x4f)) //2T旋轨器-旋转到0°（工位1）
        {
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x80); //0°
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x40); //0°
        }
        else if((Analoy.CANId >= 0x50) && (Analoy.CANId <= 0x5f)) //2P1T换轨器-无
        {
          Beep(100,100,2);
          return;
        }
      }break;
      case 0x04:
      {
        if((Analoy.CANId >= 0x10) && (Analoy.CANId <= 0x1f))      //动力轨道-无
        {
          Beep(100,100,2);
          return;
        }
        else if((Analoy.CANId >= 0x20) && (Analoy.CANId <= 0x2f)) //工作站-开合轨道合
        {
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x40); //开合轨道开
        }
        else if((Analoy.CANId >= 0x30) && (Analoy.CANId <= 0x3f)) //1T旋轨器-无
        {
          Beep(100,100,2);
          return;
        }
        else if((Analoy.CANId >= 0x40) && (Analoy.CANId <= 0x4f)) //2T旋轨器-旋转到90°（工位2）
        {
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x80); //90°
          Device[Analoy.CANId].DEVICE_STATUS |= 0x40; //90°
        }
        else if((Analoy.CANId >= 0x50) && (Analoy.CANId <= 0x5f)) //2P1T换轨器-无
        {
          Beep(100,100,2);
          return;
        }
      }break;
      case 0x02:
      {
        if((Analoy.CANId >= 0x10) && (Analoy.CANId <= 0x1f))      //动力轨道-无
        {
          Beep(100,100,2);
          return;
        }
        else if((Analoy.CANId >= 0x20) && (Analoy.CANId <= 0x2f)) //工作站-托盘上工位
        {
          Device[Analoy.CANId].DEVICE_STATUS |= 0x80; //托盘上工位
        }
        else if((Analoy.CANId >= 0x30) && (Analoy.CANId <= 0x3f)) //1T旋轨器-旋转到0°
        {
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x80); //0°
        }
        else if((Analoy.CANId >= 0x40) && (Analoy.CANId <= 0x4f)) //2T旋轨器-旋转到180°（工位3）
        {
          Device[Analoy.CANId].DEVICE_STATUS |= 0x80;   
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x40);
        }
        else if((Analoy.CANId >= 0x50) && (Analoy.CANId <= 0x5f)) //2P1T换轨器-移动到工位1
        {
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x80); //工位1
        }
      }break;
      case 0x01:
      {
        if((Analoy.CANId >= 0x10) && (Analoy.CANId <= 0x1f))      //动力轨道-无
        {
          Beep(100,100,2);
          return;
        }
        else if((Analoy.CANId >= 0x20) && (Analoy.CANId <= 0x2f)) //工作站-托盘下工位
        {
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x80); //托盘上工位
        }
        else if((Analoy.CANId >= 0x30) && (Analoy.CANId <= 0x3f)) //1T旋轨器-旋转到90°
        {
          Device[Analoy.CANId].DEVICE_STATUS |= 0x80; //0°
        }
        else if((Analoy.CANId >= 0x40) && (Analoy.CANId <= 0x4f)) //2T旋轨器-旋转到270°（工位4）
        {
          Device[Analoy.CANId].DEVICE_STATUS |= 0x80; //90°
          Device[Analoy.CANId].DEVICE_STATUS |= 0x40; //90°
        }
        else if((Analoy.CANId >= 0x50) && (Analoy.CANId <= 0x5f)) //2P1T换轨器-移动到工位1
        {
          Device[Analoy.CANId].DEVICE_STATUS |= 0x80; //工位1
        }
      }break;
      default:break;
    }
    //返回控制结果
    Net_Send(Analoy.CANId , Analoy.Comend , Analoy.Result , 0 , 0 );
  }
}


