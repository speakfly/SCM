#include<reg52.h>
unsigned char code tab[]={0x7f,0xbf,0xDF,0xEF,0xf7,0xfb,0xfd,0xfe,0xff,0xfe,0xfd,0xfb,0xf7,0xEF,0xDF,0xbf,0x7f,0xff,0xe7,0xdb,0xbd,0x7e,0xff,0x7e,0xbd,0xdb,0xe7,0xff}; 
void close_buzz()
{
	P2=(P2&0x1f)|0xa0;        //choose  Y5
	P0= 0x00;                 //close   buzz
	P2=P2&0x1f;               //close   Y5
}


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


void led_1()
{
	unsigned char i;
	P2=(P2&0x1f)|0x80;        //choose   Y4
	for(i=0;i<28;i++)
	{
		P0= tab[i];
		Delay500ms();
	}
	P2=P2&0x1f;               //close    Y5
}



void main()
{
	close_buzz();
	led_1();
}