#include<reg52.h>
#include<intrins.h>

// 引脚定义
sbit dula = P2^6;  // 数码管段选控制
sbit wela = P2^7;  // 数码管位选控制
sbit adwr = P3^6;  // ADC写控制（启动转换）
sbit adrd = P3^7;  // ADC读控制（读取结果）

// 共阴极数码管段码表（0~F）
unsigned char code table[] = {
    0x3f, 0x06, 0x5b, 0x4f, 0x66,
    0x6d, 0x7d, 0x07, 0x7f, 0x6f,
    0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71
};

// 函数声明
void delay(unsigned char z);
void display(unsigned char temp);

// 延时函数（简单软件延时）
void delay(unsigned char z) {
    unsigned int i, j;
    for(i = z; i > 0; i--)
        for(j = 110; j > 0; j--);
}

// 数码管显示函数（拆分三位数字，动态扫描显示）
void display(unsigned char temp) {
    unsigned char bai, shi, ge;
    bai = temp / 100;    // 百位
    shi = temp % 100 / 10; // 十位
    ge = temp % 10;      // 个位

    // 显示百位（假设位选0x7e对应百位）
    dula = 1;
    P0 = table[bai];
    dula = 0;
    P0 = 0xff;  // 消隐

    wela = 1;
    P0 = 0x7e;  // 位选百位（假设0x7e是百位数码管使能）
    wela = 0;
    delay(1);   // 保持显示

    // 显示十位（假设位选0x7d对应十位）
    dula = 1;
    P0 = table[shi];
    dula = 0;
    P0 = 0xff;  // 消隐

    wela = 1;
    P0 = 0x7d;  // 位选十位（假设0x7d是十位数码管使能）
    wela = 0;
    delay(1);   // 保持显示

    // 显示个位（假设位选0x7b对应个位）
    dula = 1;
    P0 = table[ge];
    dula = 0;
    P0 = 0xff;  // 消隐

    wela = 1;
    P0 = 0x7b;  // 位选个位（假设0x7b是个位数码管使能）
    wela = 0;
    delay(1);   // 保持显示
}

void main() {
    unsigned char a, adval;
    adval = 0;

    // 初始化位选（假设0x7f选中某一位，需根据实际硬件调整）
    wela = 1;
    P0 = 0x7f;
    wela = 0;

    while(1) {
        P1 = 0xff;  // 初始化P1口（ADC输入）

        // 启动ADC转换（ADWR时序）
        adwr = 1;
        _nop_();    // 短暂延时
        adwr = 0;
        _nop_();
        adwr = 1;

        // 延时等待转换完成（通过多次调用display实现粗略延时）
        for(a = 100; a > 0; a--) {
            display(adval);
        }

        // 读取ADC结果（ADRD时序）
        adrd = 1;
        _nop_();
        adrd = 0;
        _nop_();
        adval = P1;  // 从P1口读取转换值
        adrd = 1;

        // 显示读取到的ADC值
        display(adval);
    }
}
