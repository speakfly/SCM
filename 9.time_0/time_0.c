#include<regx52.h>


unsigned char code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbf};
unsigned char dsbuff[]={10,10,10,10,10,10,10,10};
unsigned char dscom=0;
int count=0;
void buzz()
{
		P2=(P2&0x1f)|0xa0;
	  P0=0x00;
	  P2=P2&0x1f;
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

//定时器
void time1_init()
{
	TMOD|=0x10;
	TH1 = (65535-2000)/256;
	TL1 = (65535-2000)%256;
	ET1=1;
	EA=1;
	TR1=1;
}

//计数器
void time0_init()
{

  TMOD|=0x05;
	TH0 = 0;
	TL0 = 0;
	ET0=1;
	TR0=1;
	IT0=1;
}

//显示1s 计数器0  统计的时间
void display_count()
{
	int num;
	int i;
	i=7;
  if(count==500)
	{
		count=0;
		num=TH0*256+TL0;      //计算统计的时间

		//计数器清0
		TH0=0;
		TL0=0;

		//显示数值
		while(num>0)
		{
			dsbuff[i]=num%10;
			num=num/10;
			i--;
		}
		do
		{
			dsbuff[i]=10;
			i--;
		}while(i>0);
		dsbuff[0]=110;
	}
}

//定时器1中断
void time1_interrupt()  interrupt 3
{
	TH1 = (65535-2000)/256;
  TL1 = (65535-2000)%256;
	display();
	count++;
}


void main()
{
	buzz();
	time0_init();
  time1_init();
	while(1)
	{
	 display_count();
	}
}
