#include"EEPROM.h"

unsigned char code tab[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00,0x54};
int dsbuff[]={16,16,16,16,16,16,16,16};
unsigned char dscom=0;
int wei=0;
sfr P4 = 0xc0;

sbit r4 = P3^3;
sbit r3 = P3^2;
sbit r2 = P3^1;
sbit r1 = P3^0;


sbit c1 = P4^4;
sbit c2 = P4^2;
sbit c3 = P3^5;
sbit c4 = P3^4;

//数码管显示
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


//定时器0中断
void smg_initerrupt() interrupt 1
{
	TH0 = (65535-2000)/256;
	TL0 = (65535-2000)%256;
	display();
 }




//延时15ms
void Delay15ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 216;
	j = 109;
	do
	{
		while (--j);
	} while (--i);
}



//按键扫描
unsigned char key_scan()
{
	r1=0;
	r2=r3=r4=1;
	c1=c2=c3=c4=1;
	if(!c1){Delay15ms();if(!c1){while(!c1);Delay15ms();return 0;}}
  else if(!c2){Delay15ms();if(!c2){while(!c2);Delay15ms();return 1;}}
  else if(!c3){Delay15ms();if(!c3){while(!c3);Delay15ms();return 2;}}
  else if(!c4){Delay15ms();if(!c4){while(!c4);Delay15ms();return 3;}}


	r2=0;
	r1=r3=r4=1;
	c1=c2=c3=c4=1;
	if(!c1){Delay15ms();if(!c1){while(!c1);Delay15ms();return 4;}}
  else if(!c2){Delay15ms();if(!c2){while(!c2);Delay15ms();return 5;}}
  else if(!c3){Delay15ms();if(!c3){while(!c3);Delay15ms();return 6;}}
  else if(!c4){Delay15ms();if(!c4){while(!c4);Delay15ms();return 7;}}


	r3=0;
	r1=r2=r4=1;
	c1=c2=c3=c4=1;
	if(!c1){Delay15ms();if(!c1){while(!c1);Delay15ms();return 8;}}
  else if(!c2){Delay15ms();if(!c2){while(!c2);Delay15ms();return 9;}}
  else if(!c3){Delay15ms();if(!c3){while(!c3);Delay15ms();return 10;}}
  else if(!c4){Delay15ms();if(!c4){while(!c4);Delay15ms();return 11;}}

	r4=0;
	r1=r2=r3=1;
	c1=c2=c3=c4=1;
	if(!c1){Delay15ms();if(!c1){while(!c1);Delay15ms();return 12;}}
  else if(!c2){Delay15ms();if(!c2){while(!c2);Delay15ms();return 13;}}
  else if(!c3){Delay15ms();if(!c3){while(!c3);Delay15ms();return 14;}}
  else if(!c4){Delay15ms();if(!c4){while(!c4);Delay15ms();return 15;}}

	return 16;
}

//关闭蜂鸣器
void close_buzz()
{
		P2=(P2&0x1f)|0xa0;
	  P0=0x00;
	  P2=P2&0x1f;
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


void clear_smg()
{
	unsigned char i;
	for(i=0;i<8;i++)
	dsbuff[i]=16;
}
//设置结束界面
void end_UI()
{
	unsigned char i;
	for(i=3;i<8;i++)
	{
		dsbuff[i]=16;
	}
	dsbuff[0]=14;
	dsbuff[1]=17;
	dsbuff[2]=13;
}


//设置输入界面
void init_UI()
{
    unsigned char i;
    dsbuff[0]=1;
	  dsbuff[1]=17;
    for(i=2;i<8;i++)
    {
      dsbuff[i]=16;
    }
}

//显示输入数据  和  缓存键值
void num_input_display(unsigned char num)
{
	  if(wei==8)
		{
			clear_smg();
			wei=0;
		}
	  dsbuff[wei]=num;
		wei++;
	
}

//保存键值到EEPROM
void save()
{
	unsigned char i;
  for(i=0;i<8;i++)
  write_eeprom(0x00+i,dsbuff[i]);
	write_eeprom(0x08,wei);
}

//读取EEPROM数据  并显示到数码管
void read()
{
	wei = read_eeprom(0x08) - 1;
	while(wei!=0)
	{
		dsbuff[wei]=read_eeprom(0x00+wei);
		wei--;
	}
}

//数字输入 按F 确认
void num_input()
{
	unsigned char num;
	while(1)
	{
		num=key_scan();
		while(num==16)
		{
		  num=key_scan();
		}
		if(num==15)
		{
			save();
			end_UI();
			break;
		}
		if(num<=9&&num>=0)
		{
			if(wei==0)
			{
				clear_smg();
			}
			num_input_display(num);
		}
	}
}


//按键功能
void key_function()
{
	unsigned char num;
	//确定按键被按下
	//------------
	num=key_scan();
	while(num==16)
	{
	  num=key_scan();
	}
	//--------------

	//按F键  进入输入界面  输入完 并保存
	if(num==15)
	{
		wei=0;
		init_UI();
		num_input();
	}
	//按E键  进入读取界面
	if(num==14)
	{
			read();
	}
}
void main()
{
	close_buzz();    //关闭蜂鸣器
	init_smg();      //初始化定时器0
  while(1)
	{
      key_function();  //进入按键功能
  }
}
