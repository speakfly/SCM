#include<regx52.h>
unsigned char code tab[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00};
unsigned char dsbuff[]={16,16,16,16,16,16,16,16};
unsigned char dscom=0;
sbit DQ = P1^4;

void Delay_OneWire(unsigned int t)
{
  char  z;
  while(t--)
  for(z=0;z<12;z++);
}

void Init_DS18B20()
{
	bit initflag = 0;
	DQ = 1;
	Delay_OneWire(12);
	DQ = 0;
	Delay_OneWire(80);
	DQ = 1;
	Delay_OneWire(10);
	initflag = DQ;
	Delay_OneWire(5);
	return;
}

//ͨ����������DS18B20дһ���ֽ�
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_OneWire(5);
		DQ = 1;
		dat >>= 1;
	}
	Delay_OneWire(5);
}

//��DS18B20��ȡһ���ֽ�
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;

	for(i=0;i<8;i++)
	{
		DQ = 0;
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}
		Delay_OneWire(5);
	}
	return dat;
}

void buzz()
{
  P2 = (P2&0x1f)|0xa0;
  P0=0x00;
  P2= P2&0x1f;
}

void display()
{
  P2 = (P2&0x1f)|0xe0;
  P0=0xff;
  P2= P2&0x1f;

  P2 = (P2&0x1f)|0xc0;
  P0=1<<dscom;
  P2= P2&0x1f;

  P2 = (P2&0x1f)|0xe0;
  P0= ~tab[dsbuff[dscom]];
	if(dscom==5)
	{
		P0=P0&0x7f;
	}
  P2= P2&0x1f;

  if(++dscom==8)
  {
    dscom=0;
  }
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



void init_time1()
{
  TMOD |= 0x01;
  TH0 = (65535-2000)/256;
  TL0 = (65535-2000)%256;
  ET0=1;
  EA=1;
  TR0=1;
}
void Time1() interrupt 1
{
  TH0 = (65535-2000)/256;
  TL0 = (65535-2000)%256;
  display();
}

int read_wendu()
{
unsigned char low,high;
int temp;
float t;
Init_DS18B20();
Write_DS18B20(0xcc);
Write_DS18B20(0x44);
Delay_OneWire(200);
Init_DS18B20();
Write_DS18B20(0xcc);
Write_DS18B20(0xbe);
low=Read_DS18B20();
high=Read_DS18B20();
// temp=(high<<8)|low;
// if(temp<0){
// temp=~(temp-1);
// }
// t=temp*0.0625;
// temp=t*100+0.5;
// return temp;

temp=(high<<6)|(low>>2);
if(temp<0)
{
  temp=~(temp-1);
}
t = temp*0.25;
temp = t*100+0.5;
return temp;
}

void displaywendu()
{
	unsigned char i;
  int wendu = read_wendu();
//  dsbuff[4]=(wendu>=1000) ? (wendu/1000):16;   wendu=wendu%1000;
//  dsbuff[5]=(wendu>=100) ? (wendu/100):0;     wendu=wendu%100;
//  dsbuff[6]=(wendu>=10) ? (wendu/10):0;       wendu=wendu%10;
//  dsbuff[7]=wendu%10;
	i=7;
	while(wendu>0)
  {
     dsbuff[i]=wendu%10;
     wendu=wendu/10;
     i=i-1;
   }
   do
	 {
			dsbuff[i]=16;
		  i=i-1;
	 }while(i>0);
}


void main()
{
  buzz();
	init_time1();
  while(1)
  {
		displaywendu();
    Delay500ms();
		Delay500ms();
  }
}
