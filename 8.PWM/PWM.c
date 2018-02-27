/**
功能:P3.4引脚输出30%的占空比
实现：中断
**/
#include<regx52.h>
sbit PWM=P3^4;
unsigned char count=0;
void close_buzz()
{
	P2=(P2&0x1f)|0xa0;        //choose  Y5
	P0= 0x00;                 //close   buzz
	P2=P2&0x1f;               //close   Y5
}

void time0_init()
{
	TMOD |= 0x01;
	TH0=(65535-10)/256;
	TL0=(65535-10)%256;
	ET0=1;
	EA=1;
	TR0=1;
}

void time0_interrupt() interrupt 1
{
	TH0=(65535-10)/256;
	TL0=(65535-10)%256;
	count++;
	if(count>=100) count=0;
	if(count<=30) PWM=1;
	else PWM=0;
}

void main()
{
	close_buzz();
	time0_init();
  while(1)
	{}
}
