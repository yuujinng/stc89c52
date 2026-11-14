/***********************************************************
* 实验 06：六位数码管显示数字与字母
* 现象：6 位数码管显示 "HELLO "（可改 set_display_string 参数）
* 硬件：P0    = 数码管段选/位选数据口
*       P2.6 = 段选锁存使能(dula)，P2.7 = 位选锁存使能(wela)
* 晶振：12MHz
*
* 要点：
* 1. 段码表、位选表加 code 关键字 → 存进程序存储器 ROM，
*    不占用只有 128 字节的内部 RAM；
* 2. 字符串参数用 char code * 指针，直接指向 ROM 中的字符串，
*    避免拷贝开销；
* 3. 扫描顺序：熄灭段码 → 位选 → 段码（消残影）。
***********************************************************/

#include <reg52.h>

sbit dula = P2^6;   /* 段选锁存使能 */
sbit wela = P2^7;   /* 位选锁存使能 */

/* 段码表中"熄灭"的下标 */
#define SEG_BLANK 37

/* 显示缓冲区（RAM，6 个待显示字符的段码索引） */
unsigned char data display_buffer[6] = {
    SEG_BLANK, SEG_BLANK, SEG_BLANK, SEG_BLANK, SEG_BLANK, SEG_BLANK
};

/* 位选码：第 1 位(左) ~ 第 6 位(右)，低电平有效 */
unsigned char code BIT_SELECT[6] = {0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF};

/* 段码表：数字 0-9 + 部分大写字母 + 少量符号（共 38 项，code 区）
   下标 0~9    → 数字 0~9
   下标 10~35  → A b C d E F G H I J K L M n O P Q r S T U V W X Y Z
   下标 36     → '-'
   下标 37     → 熄灭
   不支持造型的字母（K M Q V W X Z 等）直接给熄灭 */
unsigned char code SEG_CODE[] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,           /* 0 1 2 3 4 */
    0x6D, 0x7D, 0x07, 0x7F, 0x6F,           /* 5 6 7 8 9 */
    0x77,   /* 10: A */
    0x7C,   /* 11: b */
    0x39,   /* 12: C */
    0x5E,   /* 13: d */
    0x79,   /* 14: E */
    0x71,   /* 15: F */
    0x3D,   /* 16: G */
    0x76,   /* 17: H */
    0x30,   /* 18: I */
    0x1E,   /* 19: J */
    0x00,   /* 20: K（无对应造型，熄灭） */
    0x38,   /* 21: L */
    0x00,   /* 22: M（无对应造型，熄灭） */
    0x37,   /* 23: n */
    0x3F,   /* 24: O */
    0x73,   /* 25: P */
    0x00,   /* 26: Q（无对应造型，熄灭） */
    0x33,   /* 27: r */
    0x6D,   /* 28: S */
    0x31,   /* 29: T */
    0x3E,   /* 30: U */
    0x00,   /* 31: V（无对应造型，熄灭） */
    0x00,   /* 32: W（无对应造型，熄灭） */
    0x00,   /* 33: X（无对应造型，熄灭） */
    0x6E,   /* 34: Y */
    0x00,   /* 35: Z（无对应造型，熄灭） */
    0x40,   /* 36: '-' */
    0x00    /* 37: 熄灭 */
};

/* 约 1ms 软件延时 @12MHz */
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for (i = ms; i > 0; i--)
        for (j = 110; j > 0; j--)
            ;
}

/* 字符 → 段码表索引；不支持的字符返回熄灭 */
unsigned char get_char_index(char ch)
{
    if (ch >= '0' && ch <= '9')
        return ch - '0';

    if (ch >= 'A' && ch <= 'Z')
    {
        switch (ch)
        {
            case 'A': return 10;
            case 'B': return 11;
            case 'C': return 12;
            case 'D': return 13;
            case 'E': return 14;
            case 'F': return 15;
            case 'G': return 16;
            case 'H': return 17;
            case 'I': return 18;
            case 'J': return 19;
            case 'L': return 21;
            case 'O': return 24;
            case 'P': return 25;
            case 'S': return 28;
            case 'T': return 29;
            case 'U': return 30;
            case 'Y': return 34;
            default:  return SEG_BLANK;
        }
    }

    if (ch >= 'a' && ch <= 'z')
    {
        /* 小写字母尽量映射到对应造型（多数借用大写段码） */
        switch (ch)
        {
            case 'a': return 10;
            case 'b': return 11;
            case 'c': return 12;
            case 'd': return 13;
            case 'e': return 14;
            case 'f': return 15;
            case 'h': return 17;
            case 'i': return 18;
            case 'l': return 21;
            case 'o': return 24;
            case 'p': return 25;
            case 'r': return 27;
            case 's': return 28;
            case 't': return 29;
            case 'u': return 30;
            default:  return SEG_BLANK;
        }
    }

    switch (ch)
    {
        case '-': return 36;
        case ' ': return SEG_BLANK;
        default:  return SEG_BLANK;
    }
}

/* 把字符串写入显示缓冲区（最多 6 个字符，不足的位熄灭） */
void set_display_string(char code *str)
{
    unsigned char i;
    for (i = 0; i < 6; i++)
    {
        if (str[i] == '\0')
            display_buffer[i] = SEG_BLANK;
        else
            display_buffer[i] = get_char_index(str[i]);
    }
}

/* 在指定位置显示一个字符（含消残影的扫描顺序） */
void display_digit(unsigned char pos, unsigned char seg_index)
{
    if (pos > 5) return;

    dula = 1;  P0 = 0x00;  dula = 0;                    /* 1. 熄灭段码 */
    wela = 1;  P0 = BIT_SELECT[pos];  wela = 0;         /* 2. 选中目标位 */
    dula = 1;  P0 = SEG_CODE[seg_index];  dula = 0;     /* 3. 输出段码 */

    delay_ms(1);                                        /* 4. 保持亮度 */
}

/* 刷新一帧（6 位各显示一次，主循环不停调用） */
void display_all(void)
{
    unsigned char i;
    for (i = 0; i < 6; i++)
        display_digit(i, display_buffer[i]);
}

/* 清屏 */
void clear_display(void)
{
    unsigned char i;
    for (i = 0; i < 6; i++)
    {
        dula = 1;  P0 = 0x00;  dula = 0;
        wela = 1;  P0 = BIT_SELECT[i];  wela = 0;
    }
}

void main(void)
{
    clear_display();

    /* 改这里换显示内容（最多 6 个字符） */
    set_display_string("HELLO ");
    /* set_display_string("123456"); */
    /* set_display_string("TEST"); */

    while (1)
    {
        display_all();      /* 不停刷新，动态扫描 */
    }
}
