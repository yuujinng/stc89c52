/***********************************************************
* 实验 09：ADC0804 模数转换并数码管显示
* 现象：3 位数码管显示 ADC0804 当前转换结果（0~255）
* 硬件：P1    = ADC0804 数据口（DB0~DB7）
*       P3.6 = adwr（ADC0804 WR，启动转换）
*       P3.7 = adrd（ADC0804 RD，读取结果）
*       P0    = 数码管段选/位选数据口
*       P2.6 = 段选锁存使能(dula)，P2.7 = 位选锁存使能(wela)
* 晶振：12MHz
*
* 注意：本实验的位选码（0x7E/0x7D/0x7B）与实验 05~07 的
* （0xFE/0xFD/0xFB）不同——两批代码原本就按不同的板子接线
* 写的。请核对你的开发板原理图，用哪个实验就按哪块板核对。
***********************************************************/

#include <reg52.h>
#include <intrins.h>        /* _nop_() 需要 */

sbit dula = P2^6;   /* 数码管段选锁存使能 */
sbit wela = P2^7;   /* 数码管位选锁存使能 */
sbit adwr = P3^6;   /* ADC0804 WR：下降沿启动一次转换 */
sbit adrd = P3^7;   /* ADC0804 RD：低电平时转换结果送到数据口 */

/* 位选码（低电平有效）——按实际板子接线核对！ */
#define SEG_BAI 0x7E    /* 百位 */
#define SEG_SHI 0x7D    /* 十位 */
#define SEG_GE  0x7B    /* 个位 */

/* 共阴极段码表（0~9） */
unsigned char code table[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

/* 约 1ms 软件延时 @12MHz */
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--)
            ;
}

/* 把 0~255 的数值拆成 3 位十进制并动态扫描显示。
   每一位的顺序：熄灭段码 → 锁存位选 → 锁存段码 → 保持
   （先熄灭再换位，避免旧段码在新位置上闪现出残影） */
void display(unsigned char value)
{
    unsigned char bai, shi, ge;

    bai = value / 100;          /* 百位 */
    shi = value % 100 / 10;     /* 十位 */
    ge  = value % 10;           /* 个位 */

    /* --- 百位 --- */
    dula = 1;  P0 = 0x00;  dula = 0;
    wela = 1;  P0 = SEG_BAI;  wela = 0;
    dula = 1;  P0 = table[bai];  dula = 0;
    delay_ms(2);

    /* --- 十位 --- */
    dula = 1;  P0 = 0x00;  dula = 0;
    wela = 1;  P0 = SEG_SHI;  wela = 0;
    dula = 1;  P0 = table[shi];  dula = 0;
    delay_ms(2);

    /* --- 个位 --- */
    dula = 1;  P0 = 0x00;  dula = 0;
    wela = 1;  P0 = SEG_GE;  wela = 0;
    dula = 1;  P0 = table[ge];  dula = 0;
    delay_ms(2);
}

void main(void)
{
    unsigned char a, adval = 0;

    P1 = 0xFF;                  /* P1 作输入口，先写 1（准双向口特性） */

    wela = 1;  P0 = 0xFF;  wela = 0;    /* 先关闭所有位选 */

    while (1)
    {
        /* 启动一次转换：WR 引脚给一个下降沿脉冲 */
        adwr = 1;
        _nop_();
        adwr = 0;
        _nop_();
        adwr = 1;

        /* 等待转换完成。ADC0804 典型转换时间约 100us，
           这里借数码管刷新顺便延时（约 300ms），保守但简单；
           更严谨的做法是用 INTR 引脚（转换完成会拉低）触发读取 */
        for (a = 100; a > 0; a--)
            display(adval);     /* 等待期间刷新上一帧，显示不中断 */

        /* 读取结果：RD 引脚拉低期间，转换值出现在 P1 上 */
        adrd = 1;
        _nop_();
        adrd = 0;
        _nop_();
        adval = P1;             /* 读走转换结果 */
        adrd = 1;

        display(adval);         /* 显示新读到的值 */
    }
}
