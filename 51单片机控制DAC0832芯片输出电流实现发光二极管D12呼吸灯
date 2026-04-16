#include<reg52.h>

sbit wela=P2^7;
sbit dawr = P3^6;  
sbit dacs = P3^2; 
sbit beep=P2^3;

void delay(unsigned int z);

void main(){
unsigned char daval,flag;
    daval=0;
    wela=1;
    P0=0xff;
    wela=0;
    dacs=0;
    dawr=0;
    P0=0;
    while(1){
            if(flag == 0)  
        {
            daval += 5;       
            P0 = daval;       
            if(daval == 250)
            {
                flag = 1;     
                beep = 0;      
                delay(100);    
                beep = 1;      
            }
            delay(50);       
        }
        else  
        {
            daval -= 5;      
            P0 = daval;               
            if(daval == 0)
            {
                flag = 0;      
                beep = 0;      
                delay(100);    
                beep = 1;     
            }
            delay(50);        
        }
    }
}

void delay(unsigned int z)
{
    unsigned int i, j;
    for(i = z; i > 0; i--)   
        for(j = 110; j > 0; j--);  
}
