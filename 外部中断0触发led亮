#include<reg52.h>
sbit led2=P1^0;
void delay(unsigned int z)
{
    unsigned int i,j;
    for(i=z;i>0;i--)
        for(j=110;j>0;j--);
}
void main()
{
    EA=1;
    EX0=1;
    IT0=1;
    P3=0xff;
    //P3=0xfb;
    while(1)
        P1=0xff;
}
void int0() interrupt 0
{
    led2=0;
    delay(5);
}
