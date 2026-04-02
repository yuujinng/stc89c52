#include<reg52.h>
sbit led1=P1^0;
unsigned int num;

void main()
{
    num=0;
    TMOD=0x01;
    TH0=(65536-10000)/256;
    TL0=(65536-10000)%256;
    EA=1;
    ET0=1;
    TR0=1;
    while(1);
}

void Timer0() interrupt 1
{
    TH0=(65536-10000)/256;
    TL0=(65536-10000)%256;
    num++;
    if(num==50)
    {
        num=0;
        led1=~led1;
    }
}
