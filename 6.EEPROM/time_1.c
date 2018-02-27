/**
功能：从eeprom中读取  计数  把数值存到eeprom
实现：write_eeprom,read_eeprom,kai_com
**/
#include"EEPROM.h"
unsigned char code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbf};
unsigned char dsbuff[]={10,10,10,10,10,10,10,10};
unsigned char dscom=0;
unsigned char com;
void buzz()
{
		P2=(P2&0x1f)|0xa0;
	  P0=0x00;
	  P2=P2&0x1f;
}

void init_smg()
{
	TMOD|=0x01;
	TH0 = (65535-2000)/256;
	TL0 = (65535-2000)%256;
	ET0=1;
	EA=1;
	TR0=1;
}

void display()
{
	P2=(P2&0x1f)|0xe0;
	P0=0xff;
	P2=P2&0x1f;

	P2=(P2&0x1f)|0xc0;
	P0=1<<dscom;
	P2=P2&0x1f;

	P2=(P2&0x1f)|0xe0;
	P0=tab[dsbuff[dscom]];
	P2=P2&0x1f;

	if(++dscom==8)
	{
		dscom=0;
	}
}

void kai_com()  //开机计数函数
{
   com=read_eeprom(0x00);//从0x00地址读取数据
   dsbuff[6]=com/10%10; //数码管显示2位
   dsbuff[7]=com%10;
   com++;
   write_eeprom(0x00,com); //写到0x00
}

//void test()
//{
//	dsbuff[0]=1;
//}

void smg_initerrupt() interrupt 1
{
	TH0 = (65535-2000)/256;
	TL0 = (65535-2000)%256;
	display();
 }


 void Delay500ms()		//@11.0592MHz
 {
 	unsigned char i, j, k;

 	i = 29;
 	j = 14;
 	k = 54;
 	do
 	{
 		do
 		{
 			while (--k);
 		} while (--j);
 	} while (--i);
 }


void main()
{
	buzz();
	init_smg();
  while(1)
	{
		Delay500ms();
		Delay500ms();
		kai_com();
	};
}
