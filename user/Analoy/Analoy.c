/********ģ�����*********/

#include "Analoy.h"

struct_ANALOYCTRL Analoy;


//ģ����Ƴ��� 1ms����һ��
void Analoy_Ctrl()
{
  if(Analoy.DelayTimer == 1)
  {
    Analoy.DelayTimer = 0;
    //�ı��豸״̬
    switch(Analoy.Action)
    {
      //��ת
      case 0x80:
      {
        Device[Analoy.CANId].DEVICE_STATUS &=(~0x08); //������
        Device[Analoy.CANId].DEVICE_STATUS |= 0x04;    //�����
      }break;
      //��ת
      case 0x40:
      {
        Device[Analoy.CANId].DEVICE_STATUS |= 0x08;    //����
        Device[Analoy.CANId].DEVICE_STATUS |= 0x04;    //�����
      }break;
      //ֹͣ
      case 0x20:
      {
        Device[Analoy.CANId].DEVICE_STATUS |= 0x08;    //����
        Device[Analoy.CANId].DEVICE_STATUS &= (~0x04); //�ص��
      }break;
      //
      case 0x08:
      {
        if((Analoy.CANId >= 0x10) && (Analoy.CANId <= 0x1f))      //�������-��
        {
          Beep(100,100,2);
          return;
        }
        else if((Analoy.CANId >= 0x20) && (Analoy.CANId <= 0x2f)) //����վ-�����
        {
          Device[Analoy.CANId].DEVICE_STATUS |= 0x40; //���Ϲ����
        }
        else if((Analoy.CANId >= 0x30) && (Analoy.CANId <= 0x3f)) //1T������-��
        {
          Beep(100,100,2);
          return;
        }
        else if((Analoy.CANId >= 0x40) && (Analoy.CANId <= 0x4f)) //2T������-��ת��0�㣨��λ1��
        {
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x80); //0��
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x40); //0��
        }
        else if((Analoy.CANId >= 0x50) && (Analoy.CANId <= 0x5f)) //2P1T������-��
        {
          Beep(100,100,2);
          return;
        }
      }break;
      case 0x04:
      {
        if((Analoy.CANId >= 0x10) && (Analoy.CANId <= 0x1f))      //�������-��
        {
          Beep(100,100,2);
          return;
        }
        else if((Analoy.CANId >= 0x20) && (Analoy.CANId <= 0x2f)) //����վ-���Ϲ����
        {
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x40); //���Ϲ����
        }
        else if((Analoy.CANId >= 0x30) && (Analoy.CANId <= 0x3f)) //1T������-��
        {
          Beep(100,100,2);
          return;
        }
        else if((Analoy.CANId >= 0x40) && (Analoy.CANId <= 0x4f)) //2T������-��ת��90�㣨��λ2��
        {
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x80); //90��
          Device[Analoy.CANId].DEVICE_STATUS |= 0x40; //90��
        }
        else if((Analoy.CANId >= 0x50) && (Analoy.CANId <= 0x5f)) //2P1T������-��
        {
          Beep(100,100,2);
          return;
        }
      }break;
      case 0x02:
      {
        if((Analoy.CANId >= 0x10) && (Analoy.CANId <= 0x1f))      //�������-��
        {
          Beep(100,100,2);
          return;
        }
        else if((Analoy.CANId >= 0x20) && (Analoy.CANId <= 0x2f)) //����վ-�����Ϲ�λ
        {
          Device[Analoy.CANId].DEVICE_STATUS |= 0x80; //�����Ϲ�λ
        }
        else if((Analoy.CANId >= 0x30) && (Analoy.CANId <= 0x3f)) //1T������-��ת��0��
        {
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x80); //0��
        }
        else if((Analoy.CANId >= 0x40) && (Analoy.CANId <= 0x4f)) //2T������-��ת��180�㣨��λ3��
        {
          Device[Analoy.CANId].DEVICE_STATUS |= 0x80;   
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x40);
        }
        else if((Analoy.CANId >= 0x50) && (Analoy.CANId <= 0x5f)) //2P1T������-�ƶ�����λ1
        {
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x80); //��λ1
        }
      }break;
      case 0x01:
      {
        if((Analoy.CANId >= 0x10) && (Analoy.CANId <= 0x1f))      //�������-��
        {
          Beep(100,100,2);
          return;
        }
        else if((Analoy.CANId >= 0x20) && (Analoy.CANId <= 0x2f)) //����վ-�����¹�λ
        {
          Device[Analoy.CANId].DEVICE_STATUS &= (~0x80); //�����Ϲ�λ
        }
        else if((Analoy.CANId >= 0x30) && (Analoy.CANId <= 0x3f)) //1T������-��ת��90��
        {
          Device[Analoy.CANId].DEVICE_STATUS |= 0x80; //0��
        }
        else if((Analoy.CANId >= 0x40) && (Analoy.CANId <= 0x4f)) //2T������-��ת��270�㣨��λ4��
        {
          Device[Analoy.CANId].DEVICE_STATUS |= 0x80; //90��
          Device[Analoy.CANId].DEVICE_STATUS |= 0x40; //90��
        }
        else if((Analoy.CANId >= 0x50) && (Analoy.CANId <= 0x5f)) //2P1T������-�ƶ�����λ1
        {
          Device[Analoy.CANId].DEVICE_STATUS |= 0x80; //��λ1
        }
      }break;
      default:break;
    }
    //���ؿ��ƽ��
    Net_Send(Analoy.CANId , Analoy.Comend , Analoy.Result , 0 , 0 );
  }
}


