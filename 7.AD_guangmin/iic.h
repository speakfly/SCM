#ifndef _IIC_H
#define _IIC_H

#include "regx52.h"
#include "intrins.h"

#define somenop {unsigned char i = 3; do{_nop_();}while(i--);}
#define SlaveAddrW 0xA0
#define SlaveAddrR 0xA1

//�������Ŷ���
sbit SDA = P2^1;  /* ������ */
sbit SCL = P2^0;  /* ʱ���� */

//��������
void IIC_Start(void);
void IIC_Stop(void);
void IIC_Ack(unsigned char ackbit);
void IIC_SendByte(unsigned char byt);
bit IIC_WaitAck(void);
unsigned char IIC_RecByte(void);

#endif
