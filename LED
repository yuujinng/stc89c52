#include <reg52.h> 


void Delay() {
    unsigned char i, j, k;
    i = 4;
    j = 129;
    k = 119;
    do {
        do {
            while (--k);
        } while (--j);
    } while (--i);
}

void main() {
    while(1) {  
        P1 = 0xFE;    
        Delay(); 
        P1 = 0xFF;    
        Delay(); 
    }
}
