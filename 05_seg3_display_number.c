/***********************************************************
* 实验 05：三位数码管显示数字（动态扫描）
* 现象：3 位数码管显示 "888"（全亮，检查每一段是否正常）
* 硬件：P0    = 数码管段选/位选数据口
*       P2.6 = 段选锁存使能(dula)，P2.7 = 位选锁存使能(wela)
*       位选低电平有效，0xFE/0xFD/0xFB 分别对应个/十/百位
* 晶振：12MHz
*
* 用法：改 display_buffer[] 直接换显示内容，
*       或调 display_number() 显示 0~999 的整数。
* 数码管类型：默认共阴极；共阳极管把 COMMON_CATHODE 换成
*             COMMON_ANODE 即可（段码表已自动切换）。
***********************************************************/

#include <reg52.h>

sbit dula = P2^6;   /* 段选锁存使能 */
sbit wela = P2^7;   /* 位选锁存使能 */

/* ====================== 配置区 ====================== */
#define COMMON_CATHODE  1           /* 共阴极数码管；共阳极改为注释并启用下一行 */
/* #define COMMON_ANODE  1 */

/* 位选码：低电平选中对应位（个、十、百位）。
   若显示顺序不对，调换这里即可，例如 {0xFB, 0xFD, 0xFE} */
unsigned char code BIT_SELECT[3] = {0xFE, 0xFD, 0xFB};

unsigned char display_buffer[3] = {8, 8, 8};    /* 待显示内容（见段码表索引） */

#ifdef COMMON_CATHODE
/* 共阴极段码表（0-9, 10='-', 11=熄灭） */
unsigned char code SEG_CODE[] = {
    0x3F,   /* 0 */
    0x06,   /* 1 */
    0x5B,   /* 2 */
    0x4F,   /* 3 */
    0x66,   /* 4 */
    0x6D,   /* 5 */
    0x7D,   /* 6 */
    0x07,   /* 7 */
    0x7F,   /* 8 */
    0x6F,   /* 9 */
    0x40,   /* 10: '-' */
    0x00    /* 11: 熄灭 */
};
#endif

#ifdef COMMON_ANODE
/* 共阳极段码表（共阴极段码按位取反） */
unsigned char code SEG_CODE[] = {
    0xC0,   /* 0 */
    0xF9,   /* 1 */
    0xA4,   /* 2 */
    0xB0,   /* 3 */
    0x99,   /* 4 */
    0x92,   /* 5 */
    0x82,   /* 6 */
    0xF8,   /* 7 */
    0x80,   /* 8 */
    0x90,   /* 9 */
    0xBF,   /* 10: '-' */
    0xFF    /* 11: 熄灭 */
};
#endif
/* ==================================================== */

#define SEG_INDEX_MAX   11          /* 段码表最大下标 */

/* 约 1ms 软件延时 @12MHz */
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--)
            ;
}

/* 在指定位置显示一个字符
   pos：0=个位 1=十位 2=百位；num：段码表索引 0~11
   扫描顺序：先熄灭段码 → 锁存位选 → 锁存段码 → 保持，
   这样切换位置时旧段码不会在新位置上闪现（消残影） */
void display_digit(unsigned char pos, unsigned char num)
{
    if (pos > 2)        return;     /* 位置保护 */
    if (num > SEG_INDEX_MAX) num = SEG_INDEX_MAX;   /* 索引保护 */

    dula = 1;  P0 = 0x00;  dula = 0;                /* 1. 先熄灭段码 */
    wela = 1;  P0 = BIT_SELECT[pos];  wela = 0;     /* 2. 选中目标位 */
    dula = 1;  P0 = SEG_CODE[num];  dula = 0;       /* 3. 输出段码 */

    delay_ms(2);                                    /* 4. 保持亮度 */
}

/* 刷新一帧（3 位各显示一次） */
void display_all(void)
{
    unsigned char i;
    for (i = 0; i < 3; i++)
        display_digit(i, display_buffer[i]);
}

/* 清屏 */
void clear_display(void)
{
    unsigned char i;
    for (i = 0; i < 3; i++)
    {
        dula = 1;  P0 = 0x00;  dula = 0;
        wela = 1;  P0 = BIT_SELECT[i];  wela = 0;
    }
}

/* 按十进制位直接设置三个显示字符（十位/个位顺序见 display_number） */
void set_display(unsigned char hundreds, unsigned char tens, unsigned char ones)
{
    display_buffer[2] = hundreds;
    display_buffer[1] = tens;
    display_buffer[0] = ones;
}

/* 显示 0~999 的整数（超出范围按 999 显示） */
void display_number(unsigned int value)
{
    if (value > 999) value = 999;
    display_buffer[2] = value / 100;            /* 百位 */
    display_buffer[1] = (value % 100) / 10;     /* 十位 */
    display_buffer[0] = value % 10;             /* 个位 */
}

void main(void)
{
    clear_display();

    /* 示例 1：显示 "888"，检查所有段 */
    display_buffer[0] = 8;
    display_buffer[1] = 8;
    display_buffer[2] = 8;

    /* 示例 2：显示一个 0~999 计数器（去掉注释即可用）
    unsigned int count = 0;
    ... 在 while(1) 里调用 display_number(count); count++; */

    while (1)
    {
        display_all();          /* 不停刷新，动态扫描 */
    }
}
