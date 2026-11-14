#include<reg52.h>

sbit D1 = P1^0;
sbit D2 = P1^1;
sbit D3 = P1^2;
sbit D4 = P1^3;

void matrixkeyscan();
void delay(unsigned char z);

void main()
{
    P1 = 0xFF;  
    while(1)
    {
        matrixkeyscan();
    }
}

void matrixkeyscan()
{
    unsigned char temp;
    
    P3 = 0xFE;  
    temp = P3;
    if(temp != 0xFE)
    {
        delay(10);  
        temp = P3;
        if(temp != 0xFE)
        {
            temp = P3;
            switch(temp)
            {
                case 0xEE: D1 = 0; break;  
                case 0xDE: D2 = 0; break;  
                case 0xBE: D3 = 0; break;  
                case 0x7E: D4 = 0; break; 
            }
            while(temp != 0xFE)
            {
                temp = P3;
            }
            P1 = 0xFF; 
        }
    }
    
    P3 = 0xFD;  
    temp = P3;
    if(temp != 0xFD)
    {
        delay(10);
        temp = P3;
        if(temp != 0xFD)
        {
            temp = P3;
            switch(temp)
            {
                case 0xED: D1 = 0; break;  
                case 0xDD: D2 = 0; break;  
                case 0xBD: D3 = 0; break;  
                case 0x7D: D4 = 0; break;  
            }
            while(temp != 0xFD)
            {
                temp = P3;
            }
            P1 = 0xFF;
        }
    }
    
    P3 = 0xFB; 
    temp = P3;
    if(temp != 0xFB)
    {
        delay(10);
        temp = P3;
        if(temp != 0xFB)
        {
            temp = P3;
            switch(temp)
            {
                case 0xEB: D1 = 0; break;  
                case 0xDB: D2 = 0; break; 
                case 0xBB: D3 = 0; break; 
                case 0x7B: D4 = 0; break; 
            }
            while(temp != 0xFB)
            {
                temp = P3;
            }
            P1 = 0xFF;
        }
    }
    

    P3 = 0xF7; 
    temp = P3;
    if(temp != 0xF7)
    {
        delay(10);
        temp = P3;
        if(temp != 0xF7)
        {
            temp = P3;
            switch(temp)
            {
                case 0xE7: D1 = 0; break;  
                case 0xD7: D2 = 0; break;  
                case 0xB7: D3 = 0; break;  
                case 0x77: D4 = 0; break;  
            }
            while(temp != 0xF7)
            {
                temp = P3;
            }
            P1 = 0xFF;
        }
    }
}

void delay(unsigned char z)
{
    unsigned char i, j;
    for(i = z; i > 0; i--)
    {
        for(j = 110; j > 0; j--);
    }
}
