#include"iic.h"
#include"EEPROM.h"

void delay(int n)  
{  
    char z=110;  
    while(n--)  
    for(z=0;z<110;z++);  
} 
unsigned char read_eeprom(unsigned char add)
{
  unsigned char d;
IIC_Start();  
IIC_SendByte(0xa0); //发送器件地址
IIC_WaitAck();
IIC_SendByte(add);//发送要操作的地址
IIC_WaitAck();
IIC_Start();
IIC_SendByte(0xa1);//发送读操作
IIC_WaitAck();
d=IIC_RecByte();//读一字节
IIC_Ack(0);
IIC_Stop();
return d;
}
void write_eeprom(unsigned char add,unsigned char d)
{
  IIC_Start();
  IIC_SendByte(0xa0);//发送器件地址
  IIC_WaitAck();
  IIC_SendByte(add);//发送操作地址
  IIC_WaitAck();
  IIC_SendByte(d);//写一字节
  IIC_WaitAck();
  IIC_Stop();
  delay(10); //写好后要延时一会
}
