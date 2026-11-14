/***********************************************************
* 实验 07：数码管 000000 → FFFFFF 循环（定时器控制节拍）
* 现象：6 位数码管同时显示同一个十六进制字符，
*       每 500ms 从 0 递增到 F，到 F 后回到 0 循环
* 硬件：P0    = 数码管段选/位选数据口
*       P2.6 = 段选锁存使能(dula)，P2.7 = 位选锁存使能(wela)
* 晶振：12MHz
*
* 优化说明：
* 1. bit_sel / seg_code 加 code 关键字存入 ROM
*    （旧版占用了 20+ 字节内部 RAM，idata 一共只有 128 字节）；
* 2. 显示缓冲区的更新从 ISR 移到主循环：ISR 里只置标志位，
*    保持中断服务函数尽量短；
* 3. 头文件 reg51.h → reg52.h，与本仓库其他实验统一。
***********************************************************/

#include <reg52.h>

sbit dula = P2^6;   /* 段选锁存使能 */
sbit wela = P2^7;   /* 位选锁存使能 */

/* T0 方式 1 重装初值：65536 - 1000，1ms @12MHz */
#define T0_RELOAD_1MS (65536 - 1000)

unsigned char code bit_sel[6] = {0xDF, 0xEF, 0xF7, 0xFB, 0xFD, 0xFE};
                                        /* 第1位(左) ~ 第6位(右)，低电平有效 */

/* 段码表：0-9、A-F、16=熄灭 */
unsigned char code seg_code[17] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,       /* 0 1 2 3 4 */
    0x6D, 0x7D, 0x07, 0x7F, 0x6F,       /* 5 6 7 8 9 */
    0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, /* A b C d E F */
    0x00                                /* 熄灭 */
};

unsigned char display_buf[6];           /* 显示缓冲区 */
unsigned char current_number = 0;       /* 当前显示的字符（0~15） */

unsigned int flash_counter = 0;         /* 只在 ISR 里使用的毫秒计数器 */
volatile unsigned char update_flag = 0; /* ISR → 主循环：该换数字了 */

/* 约 1ms 软件延时 @12MHz（每一位的点亮时间） */
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--)
            ;
}

/* 在指定位置显示一个字符（消残影扫描顺序） */
void display_digit(unsigned char pos, unsigned char num)
{
    if (pos > 5) return;
    if (num > 16) num = 16;

    dula = 1;  P0 = 0x00;  dula = 0;                /* 1. 熄灭段码 */
    wela = 1;  P0 = bit_sel[pos];  wela = 0;        /* 2. 选中目标位 */
    dula = 1;  P0 = seg_code[num];  dula = 0;       /* 3. 输出段码 */

    delay_ms(2);                                    /* 4. 保持亮度 */
}

/* 刷新一帧 */
void display_all(void)
{
    unsigned char i;
    for (i = 0; i < 6; i++)
        display_digit(i, display_buf[i]);
}

/* 把 6 位都设成当前数字 */
void update_display(void)
{
    unsigned char i;
    for (i = 0; i < 6; i++)
        display_buf[i] = current_number;
}

void timer0_init(void)
{
    TMOD = 0x01;                    /* T0 方式 1（16 位定时） */
    TH0  = T0_RELOAD_1MS / 256;
    TL0  = T0_RELOAD_1MS % 256;
    ET0  = 1;                       /* 允许 T0 中断 */
    EA   = 1;                       /* 开总中断 */
    TR0  = 1;                       /* 启动 T0 */
}

void main(void)
{
    timer0_init();

    current_number = 0;
    update_display();

    while (1)
    {
        /* 节拍由定时器中断给出，缓冲区更新放在主循环里做 */
        if (update_flag)
        {
            update_flag = 0;

            current_number++;
            if (current_number > 15)    /* F 之后回到 0 */
                current_number = 0;

            update_display();
        }

        display_all();                  /* 不停刷新，动态扫描 */
    }
}

/* T0 中断服务函数：每 1ms 进入一次，只计时和置标志，保持简短 */
void timer0_isr(void) interrupt 1
{
    TH0 = T0_RELOAD_1MS / 256;          /* 方式 1 需手动重装初值 */
    TL0 = T0_RELOAD_1MS % 256;

    if (++flash_counter >= 500)         /* 500ms 换一个字符 */
    {
        flash_counter = 0;
        update_flag = 1;
    }
}
