#include <regx52.h>
#include "IIC.h"
unsigned char code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbf,0xc1};
unsigned char dsbuff[]={10,10,10,10,10,10,10,10};
unsigned char dscom=0;


//关闭蜂鸣器
void close_buzz()
{
		P2=(P2&0x1f)|0xa0;
	  P0 &=0xbf;
	  P2=P2&0x1f;
}

//打开蜂鸣器
void open_buzz()
{
		P2=(P2&0x1f)|0xa0;
	  P0 |=0x40;
	  P2=P2&0x1f;
}

//关闭继电器
void close_jidianqi()
{
	P2=(P2&0x1f)|0xa0;
	P0 &=0xef;
	P2=P2&0x1f;
}

//打开继电器
void open_jidianqi()
{
	P2=(P2&0x1f)|0xa0;
	P0 |=0x10;
	P2=P2&0x1f;
}

//初始化 定时器0
void init_smg()
{
	TMOD|=0x01;
	TH0 = (65535-2000)/256;
	TL0 = (65535-2000)%256;
	ET0=1;
	EA=1;
	TR0=1;
}


//显示数码管
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

	//增加小数点
	//---------
	if(dscom==5)
	{
		P0=P0&0x7f;
	}
	P2=P2&0x1f;
  //----------


	if(++dscom==8)
	{
		dscom=0;
	}
}

//中断设置
void smg_initerrupt() interrupt 1
{
	TH0 = (65535-2000)/256;
	TL0 = (65535-2000)%256;
	display();
}

//延时500ms
void Delay500ms()		//@11.0592MHz
{
	unsigned char i, j, k;
	i = 22;
	j = 3;
	k = 227;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

//延时50ms
void Delay50ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	i = 3;
	j = 207;
	k = 28;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


//IIC  读一个字节
unsigned char ad_read_byte(unsigned char device_addr,unsigned char channel)
{
	unsigned char adc_value;
  IIC_Start();
	IIC_SendByte(device_addr);
	IIC_WaitAck();
	IIC_SendByte(channel);
	IIC_WaitAck();
	IIC_Stop();

	IIC_Start();
	IIC_SendByte(device_addr+1);
	IIC_WaitAck();
	adc_value = IIC_RecByte();
	IIC_Ack(0);
	IIC_Stop();
	return adc_value;
}

//显示数据
void displaydata()
{
	float nums;
	int num;
	num = ad_read_byte(0x90,3);
	nums = ad_read_byte(0x90,3) * 5 / 256;
	num = nums *100;


  dsbuff[4]=(num>=1000) ? (num/1000):10;   num=num%1000;
  dsbuff[5]=(num>=100) ? (num/100):0;     num=num%100;
  dsbuff[6]=(num>=10) ? (num/10):0;       num=num%10;
  dsbuff[7]=num%10;
}

//报警
void baojing()
{
    open_buzz();
		open_jidianqi();
	  Delay50ms();
	  close_jidianqi();
}

//检测光照值
void test_guangmin()
{
	unsigned char zhi=ad_read_byte(0x90,1);
	zhi=ad_read_byte(0x90,1);
  if(zhi<60)
	{
	  baojing();
	}
	else
	{
	   close_buzz();
		 close_jidianqi();
	}
}


void main()
{
	close_buzz();      //关闭蜂鸣器
	init_smg();        //初始化定时器0
	close_jidianqi();  //关闭继电器
	dsbuff[0]=12;      //显示"V"
	while(1)
	{
		 displaydata();  //显示电压
		 test_guangmin();//检测光照值
	   Delay500ms();   //延时500ms
	}
}
