#include <regx52.h>
#include "ds1302.h"


unsigned char code tab[] = {0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xff,0xbf,0x9c,0xc6,0xbb,0xa1,0x88};
unsigned char dsbuff[]={10,10,10,10,10,10,10,10};
unsigned char dscom=0;
unsigned char wei=0;
unsigned char s_time[7]={0x28,0x23,0x20,0x05,0x03,0x04,0x17};     //设置时间缓存区
unsigned char g_time[7]={0};                                      //获取时间缓存区
unsigned char time_value[6]={0};                                  //按键 时间 缓存区
unsigned char date_value[6]={0};                                  //按键 日期 缓存区
unsigned char alarm_value[3]={0};                                 // 设置闹钟时间
unsigned char temp_value[6]={0};                                  //按键 闹钟时间  缓存区
sbit DQ = P1^4;
int count=0;
unsigned char wendu_flag=0;                                      //温度标志  主要用来区别是否显示小数点
unsigned char allow_alarm=0;                                     //是否允许闹钟开启
unsigned char alarm_flag=0;                                      //是否响铃


sfr P4 = 0xc0;

sbit r4 = P3^3;
sbit r3 = P3^2;
sbit r2 = P3^1;
sbit r1 = P3^0;


sbit c1 = P4^4;
sbit c2 = P4^2;
sbit c3 = P3^5;
sbit c4 = P3^4;


//关闭蜂鸣器
void close_buzz()
{
		P2=(P2&0x1f)|0xa0;
	  P0 &=0xbf;
	  P2=P2&0x1f;
}

//开启蜂鸣器
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

//开启继电器
void open_jidianqi()
{
	P2=(P2&0x1f)|0xa0;
	P0 |=0x10;
	P2=P2&0x1f;
}

//延时一毫秒
void delay(int n)
{
    char z=110;
    while(n--)
    for(z=0;z<110;z++);
}


//初始化定时器0
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
	if(dscom==3&&wendu_flag==1)
	{
		P0=P0&0x7f;
	}
	P2=P2&0x1f;

	if(++dscom==8)
	{
		dscom=0;
	}
}


//定时器0中断
void smg_initerrupt() interrupt 1
{
	TH0 = (65535-2000)/256;
	TL0 = (65535-2000)%256;
	display();
	if(alarm_flag==1)
	{
			count++;
	}
}

//按键扫描
unsigned char key_scan()
{
	r1=0;
	r2=r3=r4=1;
	c1=c2=c3=c4=1;
	if(!c1){delay(20);if(!c1){while(!c1);delay(20);return 0;}}
  else if(!c2){delay(20);if(!c2){while(!c2);delay(20);return 1;}}
  else if(!c3){delay(20);if(!c3){while(!c3);delay(20);return 2;}}
  else if(!c4){delay(20);if(!c4){while(!c4);delay(20);return 3;}}


	r2=0;
	r1=r3=r4=1;
	c1=c2=c3=c4=1;
	if(!c1){delay(20);if(!c1){while(!c1);delay(20);return 4;}}
  else if(!c2){delay(20);if(!c2){while(!c2);delay(20);return 5;}}
  else if(!c3){delay(20);if(!c3){while(!c3);delay(20);return 6;}}
  else if(!c4){delay(20);if(!c4){while(!c4);delay(20);return 7;}}


	r3=0;
	r1=r2=r4=1;
	c1=c2=c3=c4=1;
	if(!c1){delay(20);if(!c1){while(!c1);delay(20);return 8;}}
  else if(!c2){delay(20);if(!c2){while(!c2);delay(20);return 9;}}
  else if(!c3){delay(20);if(!c3){while(!c3);delay(20);return 10;}}
  else if(!c4){delay(20);if(!c4){while(!c4);delay(20);return 11;}}

	r4=0;
	r1=r2=r3=1;
	c1=c2=c3=c4=1;
	if(!c1){delay(20);if(!c1){while(!c1);delay(20);return 12;}}
  else if(!c2){delay(20);if(!c2){while(!c2);delay(20);return 13;}}
  else if(!c3){delay(20);if(!c3){while(!c3);delay(20);return 14;}}
  else if(!c4){delay(20);if(!c4){while(!c4);delay(20);return 15;}}

	return 16;
}

//单总线 延时
void Delay_OneWire(unsigned int t)
{
  char  z;
  while(t--)
  for(z=0;z<12;z++);
}


//初始化 DS18B20
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

//DS18B20 写一个字节
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

//DS18B20 读取一个字节
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

//读温度
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

//将 高字节和低字节 转化 成int型
temp=(high<<6)|(low>>2);

//将补码转换成反码
if(temp<0)
{
  temp=~(temp-1);
}


//保留2为小数
t = temp*0.25;
temp = t*100+0.5;
return temp;
}


//显示温度
void displaywendu()
{
	unsigned char i;
  int wendu = read_wendu();
	dsbuff[7]=13;
	dsbuff[6]=12;
	i=5;

	//将数值 显示在数码管
	while(wendu>0)
  {
     dsbuff[i]=wendu%10;
     wendu=wendu/10;
     i=i-1;
   }

	 //除了数值显示 其他数码管不亮
   do
	 {
			dsbuff[i]=10;
		  i=i-1;
	 }while(i>0);
	 dsbuff[0]=10;
}

//获取时间和日期
void get_time()
{
    char i;
    for(i=0;i<7;i++)
    {
    g_time[i]=Ds1302_Single_Byte_Read(0x81+i*2);
		delay(30);
    }
}

//设置时间和日期
void set_time()
{
    char i;
    char adr=0x80;
    Ds1302_Single_Byte_Write(0x8e,0x00);
    for(i=0;i<7;i++)
    Ds1302_Single_Byte_Write(adr+i*2,s_time[i]);
    Ds1302_Single_Byte_Write(0x8e,0x80);
}

//显示 时间
void dstime_sfm()
{

    dsbuff[6]=(g_time[0]&0x70)>>4;
    dsbuff[7]=g_time[0]&0x0f;

    dsbuff[5]=11;   //-

    dsbuff[3]=(g_time[1]&0x70)>>4;
    dsbuff[4]=g_time[1]&0x0f;

     dsbuff[2]=11;  //-

    dsbuff[0]=(g_time[2]&0x30)>>4;
    dsbuff[1]=g_time[2]&0x0f;

}

//显示日期
void dstime_nyr()
{

    dsbuff[6]=(g_time[3]&0x30)>>4;
    dsbuff[7]=g_time[3]&0x0f;

    dsbuff[5]=11;

    dsbuff[3]=(g_time[4]&0x10)>>4;
    dsbuff[4]=g_time[4]&0x0f;

     dsbuff[2]=11;

    dsbuff[0]=(g_time[6]&0xf0)>>4;
    dsbuff[1]=g_time[6]&0x0f;
}


//显示闹钟时间
void dstime_alarm()
{
	  dsbuff[6]=(alarm_value[0]&0x70)>>4;
    dsbuff[7]=alarm_value[0]&0x0f;

    dsbuff[5]=11;   //-

    dsbuff[3]=(alarm_value[1]&0x70)>>4;
    dsbuff[4]=alarm_value[1]&0x0f;

     dsbuff[2]=11;  //-

    dsbuff[0]=(alarm_value[2]&0x30)>>4;
    dsbuff[1]=alarm_value[2]&0x0f;
}

//获取时间 转换 成设置时间
void g_time_s_time()
{
	unsigned char i;
  for(i=0;i<8;i++)
	{
		s_time[i] = g_time[i];
	}
}
//清除数码管
void clear_smg()
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		dsbuff[i]=10;
	}
}

// 输入 时间  界面
void input_time_UI()
{
   unsigned char i;
   dsbuff[0]=14;
   for(i=1;i<8;i++)
   {
     dsbuff[i]=10;
   }
}

//输入 日期  界面
void input_date_UI()
{
   unsigned char i;
   dsbuff[0]=15;
   for(i=1;i<8;i++)
   {
     dsbuff[i]=10;
   }
}

//输入闹钟时间  界面
void input_alarm_UI()
{
   unsigned char i;
   dsbuff[0]=16;
   for(i=1;i<8;i++)
   {
     dsbuff[i]=10;
   }
}

//删除  显示界面
unsigned  char  num_setblack_display()
{
	if(wei%3==0&&wei!=0)
	{
		dsbuff[wei-1]=10;
		wei=wei-2;
		dsbuff[wei]=10;
		return 1;
	}
	else if(wei>0)
	{
		wei=wei-1;
		dsbuff[wei]=10;
		return 1;
	}
	return 0;

}

// 输入显示界面
void num_input_display(unsigned char num)
{
  dsbuff[wei]=num;
	if(++wei==8)
	{
		  wei=0;
	}
	if((wei+1)%3==0)
	{
		dsbuff[wei]=11;
		wei=wei+1;
	}
}

//数值 转化 成时间
void num_time()
{
	 s_time[0] = time_value[4]*16+time_value[5];
	 s_time[1] = time_value[2]*16+time_value[3];
	 s_time[2] = time_value[0]*16+time_value[1];
}

//数值 转化 成闹钟时间
void num_alarm()
{
	 alarm_value[0] = temp_value[4]*16+temp_value[5];
	 alarm_value[1] = temp_value[2]*16+temp_value[3];
	 alarm_value[2] = temp_value[0]*16+temp_value[1];
}

//数值 转化成 日期
void num_date()
{
   s_time[3] = date_value[4]*16+date_value[5];
	 s_time[4] = date_value[2]*16+date_value[3];
	 s_time[6] = date_value[0]*16+date_value[1];
}



//判断时间输入是否正确
unsigned char time_ture(unsigned char i,unsigned char num)
{
   if(i==0&&num>2)
	 {
		return 0;
	 }
	 if(i==1&& time_value[0]==2 &&  num >3 )
	 {
			return 0;
	 }
	 if(i==2&&num>5)
	 {
		return 0;
	 }
	 if(i==4&&num>5)
	 {
		return 0;
	 }
	 return 1;
}


//判断日期输入是否正确
unsigned char date_ture(unsigned char i,unsigned char num)
{
	 if(i==2&&num>1)
	 {
		return 0;
	 }
	 if(i==3 && date_value[2]==1  && num>2 )
	 {
		return 0;
	 }
	 if(i==3 && date_value[2]==0  && num==0)
	 {
		return 0;
	 }
	 if(i==4&&num>3)
	 {
		return 0;
	 }
	 if(i==5 && date_value[4]==3  && num>1)
	 {
		return 0;
	 }
	 if(i==5 && date_value[4]==0  && num==0)
	 {
		return 0;
	 }
	 return 1;
}

//输入时间
void num_input_time()
{
	unsigned char num;
	unsigned char i;
	i=0;
	while(i<=6)
	{

		num=key_scan();
	  while(num==16)
    {
       num=key_scan();
		}

		if(num==15 && i!=6)
		{
			wei=0;
			return ;
		}

		if(num>=0 && num<=9 && i!=6)
		{
			if(i==0)clear_smg();
			if(time_ture(i,num))
			{
				time_value[i]=num;
				num_input_display(num);
				i++;
			}
		}
		if(i==6 && num==15)
		{
			get_time();
			g_time_s_time();
			num_time();
			set_time();
			wei=0;
			i++;
		}
		if(num==14&&i>0)
		{
			if(num_setblack_display())
			i--;
		}
	}
}

//输入日期
void num_input_date()
{
	unsigned char num;
	unsigned char i;
	i=0;
	while(i<=6)
	{

		num=key_scan();
	  while(num==16)
    {
       num=key_scan();
		}

		if(num==15 && i!=6)
		{
			wei=0;
			return ;
		}

		if(num>=0 && num<=9 && i!=6)
		{
			if(i==0)clear_smg();
			if(date_ture(i,num))
			{
				date_value[i]=num;
				num_input_display(num);
				i++;
			}
		}
		if(i==6 && num==15)
		{
			get_time();
			g_time_s_time();
			num_date();
			set_time();
			wei=0;
			i++;
		}
		if(num==14&&i>0)
		{
			if(num_setblack_display())
			i--;
		}
	}
}



//设置闹钟时间
void set_alarm()
{
	unsigned char num;
	unsigned char i;
	i=0;
	while(i<=6)
	{

		num=key_scan();
	  while(num==16)
   {
      num=key_scan();
		}

		if(num==15 && i!=6)
		{
			wei=0;
			return ;
		}

		if(num>=0 && num<=9 && i!=6)
		{
			if(i==0)clear_smg();
			if(time_ture(i,num))
			{
				temp_value[i]=num;
				num_input_display(num);
				i++;
			}
		}
		if(i==6 && num==15)
		{
     num_alarm();
			wei=0;
			i++;
		}
		if(num==14&&i>0)
		{
			if(num_setblack_display())
			i--;
		}
	}
}






//检测是否到闹钟时间
void test_alarm()
{
	unsigned  char i;
	for(i=0;i<3;i++)
	{
		if(g_time[i]!=alarm_value[i])
		break;
	}
	if(i==3 && allow_alarm==1)
	{
			alarm_flag = 1;
	}
	return;
}

//开启闹钟
void open_alarm()
{
	if(count<1500)
	{
		open_buzz();
		open_jidianqi();
		delay(100);
		close_buzz();
		close_jidianqi();
	}
	else
	{
		alarm_flag=0;
		count = 0;
	}
}

//闹钟
void alarm()
{
	test_alarm();
	if(alarm_flag==1)
	{
		open_alarm();
	}
}



//按键功能
void key_function()
{
	unsigned char key;
	key=key_scan();
	
	//显示温度
	  if(key==8)
		{
			displaywendu();
			wendu_flag=1;
			delay(10000);
			wendu_flag=0;
		}
		
		//显示日期  和 闹钟时间
		if(key==9)
		{
			dstime_nyr();
			delay(10000);
			dstime_alarm();
			delay(10000);
		}
		
		
		//设置是否允许开启闹钟
		if(key==10)
		{
			if(allow_alarm==1)
			{
				allow_alarm=0;
				clear_smg();
				dsbuff[0]=0;
				delay(10000);
			}
			else
			{
				allow_alarm=1;
				clear_smg();
				dsbuff[0]=1;
				delay(10000);
			}
		}
		
		//设置闹钟
		if(key==11)
		{
			clear_smg();
			input_alarm_UI();
			set_alarm();
		}

		//设置时间
		if(key==12)
		{
			clear_smg();
			input_time_UI();
			num_input_time();
		}
		
		//设置日期
		if(key==13)
		{
			clear_smg();
			input_date_UI();
			num_input_date();
		}

}



void main()
{

	close_buzz();
	init_smg();
  set_time();
  while(1)
  {
		delay(100);
    get_time();
		dstime_sfm();
		key_function();
		alarm();
  }
}
