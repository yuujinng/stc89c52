/***********************************************************
* 实验 08：4×4 矩阵键盘扫描
* 现象：按下任一按键，该键所在"列"对应的 LED 点亮
*       （第 1 列→D1，第 2 列→D2，第 3 列→D3，第 4 列→D4），
*       松手后 LED 熄灭
* 硬件：键盘 4 行接 P3.0~P3.3（行线，输出低电平扫描）
*       键盘 4 列接 P3.4~P3.7（列线，读取）
*       D1~D4 接 P1.0~P1.3，低电平点亮
* 原理：逐行拉低，读回列线；某列被行线拉低说明该行列
*       交叉点的键被按下
*
* 优化说明：旧版把同一段扫描代码复制了 4 遍（约 120 行），
* 现改为循环扫描 4 行（约 40 行），逻辑不变，更好维护。
***********************************************************/

#include <reg52.h>

/* 行扫描码：每行轮流输出低电平 */
unsigned char code row_scan[4] = {0xFE, 0xFD, 0xFB, 0xF7};

/* 约 1ms 软件延时 @12MHz */
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--)
            ;
}

/* 按列号点亮 LED：col=0~3 → D1~D4（低电平点亮） */
void show_key(unsigned char col)
{
    P1 = ~(0x01 << col);    /* 只有 P1.col 为 0，其余为 1 */
}

/* 扫描一遍全部 4 行，处理按下的键（消抖 + 等待松手） */
void matrixkeyscan(void)
{
    unsigned char row, col, temp;

    for (row = 0; row < 4; row++)
    {
        P3 = row_scan[row];             /* 当前行拉低，其余行保持高 */
        temp = P3;                      /* 读回端口：低 4 位是行线，高 4 位是列线 */
        if ((temp & 0xF0) == 0xF0)      /* 高 4 位全为 1 → 本行无键按下 */
            continue;

        delay_ms(10);                   /* 消抖：延时后再确认一次 */
        temp = P3;
        if ((temp & 0xF0) == 0xF0)
            continue;                   /* 抖动造成的假按下，忽略 */

        /* 高 4 位中哪一位被拉低 → 键在哪一列 */
        switch (temp & 0xF0)
        {
            case 0xE0: col = 0; break;  /* P3.4 → 第 1 列 */
            case 0xD0: col = 1; break;  /* P3.5 → 第 2 列 */
            case 0xB0: col = 2; break;  /* P3.6 → 第 3 列 */
            case 0x70: col = 3; break;  /* P3.7 → 第 4 列 */
            default:   continue;        /* 多键同时按下，忽略 */
        }

        show_key(col);

        /* 等待松手（阻塞式写法，简单可靠；
           改进方向：不等待、记录键值由主循环处理状态机） */
        while ((P3 & 0xF0) != 0xF0)
            ;

        P1 = 0xFF;                      /* 松手后熄灭全部 LED */
    }
}

void main(void)
{
    P1 = 0xFF;                          /* 上电熄灭全部 LED */
    while (1)
    {
        matrixkeyscan();
    }
}
