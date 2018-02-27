#include<reg51.h>
unsigned char code tab[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00,0x54};
unsigned char dsbuff[]={16,16,16,16,16,16,16,16};
unsigned char dscom=0;
unsigned char wei=0;
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


//void key_input()
//{
//    unsigned char i;
//    unsigned char num;
//    for(i=0;i<8;i++)
//    {
//      num=key_scan();
//      while(num==16)
//      {
//        num=key_scan();
//      }
//      dsbuff[i]=num;
//			if(i==0)
//			{
//				dsbuff[1]=16;
//			}
//    }
//		
//		num=key_scan();
//    while(num==16)
//    {
//       num=key_scan();
//    }
//		
//		end_UI();
//}
void num_input(unsigned char num)
{
	dsbuff[wei]=num;
	if(++wei==8)wei=0;
}

void set_black()
{
	if(wei>0)
	{
	 wei=wei-1;
	 dsbuff[wei]=16;
	}
	if(wei==0)
	{
		wei=7;
		dsbuff[wei]=16;
	}
}

void clear_smg()
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		dsbuff[i]=16;
	}
}

void num_alldel()
{
	unsigned char i;
	for(i=0;i<8;i++)
	dsbuff[i]=16;
	wei=0;
}
void key_function()
{
	  unsigned char num;
	  init_UI();
	  
	
	  do
    {
		  num=key_scan();
	    while(num==16)
      {
        num=key_scan();
			}
			
			if(num==12)
			{
				set_black();
//					wei=wei-1;
//	        dsbuff[wei]=16;
			}
			
			if(0<=num && num<=9)
			{
				if(wei==0)
				{
					dsbuff[1]=16;
				}
				num_input(num);
				
			}
			if(num==14)
			{
				clear_smg();
			}
			
			if(num == 13)
			{
					num_alldel();
			}
		}while(num!=15);
		end_UI();
		wei=0;
	
//	  num=key_scan();
//	  while(num==16)
//    {
//      num=key_scan();
//    }
//		
//		
//		
//		while(1)
//		{
//			if(0<=num<=9)
//			{
//			  num_input(num);
//			}
//			if(num==15)
//			{
////				setting_key();
//				end_UI();
//				wei=0;
//				break;
//			}
//			if(num==14)
//			{
//				set_black();
//			}
//			num=key_scan();
//			while(num==16)
//      {
//        num=key_scan();
//      }
//		}
}

void main()
{
	unsigned char num;
	buzz();
	init_smg();
  while(1)
	{
		num=key_scan();
    if(num==15)                  //15   settings
    {
      key_function();
//			dsbuff[0]=1;
    }
	}
}
