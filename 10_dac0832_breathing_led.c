/***********************************************************
* 实验 10：DAC0832 输出电压实现 LED 呼吸灯
* 现象：LED 亮度从暗到亮、再从亮到暗循环（呼吸效果），
*       到达最亮/最暗时蜂鸣器短响一声
* 硬件：P0    = DAC0832 数据口（DI0~DI7）
*       P3.6 = dawr（DAC0832 WR）
*       P3.2 = dacs（DAC0832 CS）
*       DAC 输出接 LED；蜂鸣器接 P2.3；P2.7 = 数码管位选锁存
* 晶振：12MHz
* 原理：CS=0 且 WR=0 时 DAC0832 处于直通模式，P0 上的数值
*       直接转换为输出电流，数值越大 LED 越亮。
*       让数值 0→250→0 循环变化，即得到呼吸灯效果。
*
* 优化说明：旧版 flag 是未初始化的局部变量（Keil C51 不会
* 自动清零局部变量），上电后第一次先渐亮还是先渐暗是随机
* 的。现改为显式初始化的 bit 变量 rising，并补上蜂鸣器
* 初始为关闭状态。
***********************************************************/

#include <reg52.h>

sbit wela = P2^7;   /* 数码管位选锁存使能（本实验用它关掉数码管） */
sbit dawr = P3^6;   /* DAC0832 WR */
sbit dacs = P3^2;   /* DAC0832 CS */
sbit beep = P2^3;   /* 蜂鸣器（低电平响） */

/* ====================== 配置区 ====================== */
#define DAC_STEP    5       /* 每级亮度增减的步进 */
#define DAC_MAX     250     /* 最亮对应的数值（5 的倍数，保证能恰好到达） */
#define LEVEL_MS    50      /* 每级停留时间：越小呼吸越快 */
#define BEEP_MS     100     /* 蜂鸣器响的时长 */
/* ==================================================== */

/* 约 1ms 软件延时 @12MHz */
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--)
            ;
}

void main(void)
{
    unsigned char daval = 0;    /* 当前输出值：0(最暗) ~ DAC_MAX(最亮) */
    bit rising = 1;             /* 1 = 渐亮，0 = 渐暗 */

    /* 关掉数码管位选，避免 P0 上的 DAC 数据让数码管乱亮 */
    wela = 1;  P0 = 0xFF;  wela = 0;

    beep = 1;                   /* 关闭蜂鸣器 */

    /* 拉低 CS 和 WR → DAC0832 直通模式，P0 即为 DAC 输入 */
    dacs = 0;
    dawr = 0;

    P0 = daval;                 /* 从最暗开始 */

    while (1)
    {
        if (rising)
        {
            daval += DAC_STEP;
            if (daval >= DAC_MAX)
            {
                daval = DAC_MAX;
                rising = 0;
                beep = 0;  delay_ms(BEEP_MS);  beep = 1;
            }
        }
        else
        {
            daval -= DAC_STEP;
            if (daval == 0)
            {
                rising = 1;
                beep = 0;  delay_ms(BEEP_MS);  beep = 1;
            }
        }

        P0 = daval;             /* 直通模式下立即生效 */
        delay_ms(LEVEL_MS);
    }
}
