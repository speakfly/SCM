/**
功能：按下矩阵按键就在数码管显示对应的键值  例如 S12
实现:key_scan用于键盘扫描获取键值，display用来显示数码管
**/
#include<reg51.h>
unsigned char code tab[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00};
unsigned char dsbuff[]={16,16,16,16,16,16,16,16};
unsigned char dscom=0;
sfr P4 = 0xc0;

sbit c1 = P3^3;
sbit c2 = P3^2;
sbit c3 = P3^1;
sbit c4 = P3^0;


sbit r1 = P4^4;
sbit r2 = P4^2;
sbit r3 = P3^5;
sbit r4 = P3^4;
//sbit r1 = P3^7;
//sbit r2 = P3^6;
//sbit r3 = P3^5;
//sbit r4 = P3^4;

void Delay5ms()		//@11.0592MHz
{
	unsigned char i, j;
	i = 72;
	j = 205;
	do
	{
		while (--j);
	} while (--i);
}


unsigned char key_scan()
{
	r1=0;
	r2=r3=r4=1;
	c1=c2=c3=c4=1;
	if(!c1){Delay5ms();if(!c1){while(!c1);return 0;}}
  else if(!c2){Delay5ms();if(!c2){while(!c2);return 1;}}
  else if(!c3){Delay5ms();if(!c3){while(!c3);return 2;}}
  else if(!c4){Delay5ms();if(!c4){while(!c4);return 3;}}


	r2=0;
	r1=r3=r4=1;
	c1=c2=c3=c4=1;
	if(!c1){Delay5ms();if(!c1){while(!c1);return 4;}}
  else if(!c2){Delay5ms();if(!c2){while(!c2);return 5;}}
  else if(!c3){Delay5ms();if(!c3){while(!c3);return 6;}}
  else if(!c4){Delay5ms();if(!c4){while(!c4);return 7;}}


	r3=0;
	r1=r2=r4=1;
	c1=c2=c3=c4=1;
	if(!c1){Delay5ms();if(!c1){while(!c1);return 8;}}
  else if(!c2){Delay5ms();if(!c2){while(!c2);return 9;}}
  else if(!c3){Delay5ms();if(!c3){while(!c3);return 10;}}
  else if(!c4){Delay5ms();if(!c4){while(!c4);return 11;}}

	r4=0;
	r1=r2=r3=1;
	c1=c2=c3=c4=1;
	if(!c1){Delay5ms();if(!c1){while(!c1);return 12;}}
  else if(!c2){Delay5ms();if(!c2){while(!c2);return 13;}}
  else if(!c3){Delay5ms();if(!c3){while(!c3);return 14;}}
  else if(!c4){Delay5ms();if(!c4){while(!c4);return 15;}}

	return 16;
}
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
	P0=~tab[dsbuff[dscom]];
	P2=P2&0x1f;

	if(++dscom==8)
	{
		dscom=0;
	}
}

void smg_initerrupt() interrupt 1
{
	TH0 = (65535-2000)/256;
	TL0 = (65535-2000)%256;
	display();
}

void main()
{
	unsigned char num;
	buzz();
	init_smg();
  while(1)
	{
		num=key_scan()+4;
		if(num<=19)
		{
			if(num>=10)
			{
				dsbuff[7]=num%10;
				dsbuff[6]=num/10;
				dsbuff[5]=5;
			}
			else
			{
				dsbuff[7]=num;
				dsbuff[6]=5;
				dsbuff[5]=16;
			}
		}
	}
}
