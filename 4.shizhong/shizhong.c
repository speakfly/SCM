#include <regx52.h>
#include "ds1302.h"


unsigned char code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbf};
unsigned char dsbuff[]={10,10,10,10,10,10,10,10};
unsigned char dscom=0;
unsigned char s_time[7]={0x28,0x23,0x20,0x05,0x03,0x04,0x17};//设置时间数组
unsigned char g_time[7]={0};  //保存时间数组

int count=0;


void buzz()
{
		P2=(P2&0x1f)|0xa0;
	  P0=0x00;
	  P2=P2&0x1f;
}

void Delay100ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	i = 29;
	j = 14;
	k = 11;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

void delay(int n)  
{  
    char z=110;  
    while(n--)  
    for(z=0;z<110;z++);  
}  
void get_time()   //获取时间
{
    char i;
    for(i=0;i<7;i++)
    {
    g_time[i]=Ds1302_Single_Byte_Read(0x81+i*2);
		delay(30);
    }
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


void set_time()     //设置时间
{
    char i;
    char adr=0x80;   //写地址
    Ds1302_Single_Byte_Write(0x8e,0x00);    //写保护可写
    for(i=0;i<7;i++)
    Ds1302_Single_Byte_Write(adr+i*2,s_time[i]);
    Ds1302_Single_Byte_Write(0x8e,0x80);   //写保护不可写
}


void dstime_sfm()    //显示时间
{
    //BCD转码
    dsbuff[6]=(g_time[0]&0x70)>>4;//秒十位
    dsbuff[7]=g_time[0]&0x0f;     //秒各位

    dsbuff[5]=11;   //-

    dsbuff[3]=(g_time[1]&0x70)>>4;//分十位
    dsbuff[4]=g_time[1]&0x0f;     //分各位

     dsbuff[2]=11;  //-

    dsbuff[0]=(g_time[2]&0x30)>>4;//时十位
    dsbuff[1]=g_time[2]&0x0f;     //时各位

}

//void dstime_nyr()         //显示年月日
//{
//    //BCD转码  
//    dsbuff[6]=(g_time[3]&0x30)>>4;//日十位
//    dsbuff[7]=g_time[3]&0x0f;     //日各位

//    dsbuff[5]=11;   //-

//    dsbuff[3]=(g_time[4]&0x10)>>4;//月十位
//    dsbuff[4]=g_time[4]&0x0f;     //月各位

//     dsbuff[2]=11;  //-

//    dsbuff[0]=(g_time[6]&0xf0)>>4;//年十位
//    dsbuff[1]=g_time[6]&0x0f;     //年各位
//}

void smg_initerrupt() interrupt 1
{
	TH0 = (65535-2000)/256;
	TL0 = (65535-2000)%256;
	display();
}

void main()
{
	buzz();
	init_smg();
  set_time();
  while(1)
  {
		Delay100ms();
    get_time();
		dstime_sfm();
  }
}
