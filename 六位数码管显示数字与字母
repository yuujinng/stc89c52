/***********************************************************
* 文件：（优化内存版）
* 功能：可显示任意6个字符（数字+字母）
* 硬件：P2.6=段选锁存(dula)，P2.7=位选锁存(wela)
***********************************************************/

#include<reg51.h>

// 引脚定义
sbit dula = P2^6;  // 段选锁存
sbit wela = P2^7;  // 位选锁存

// 函数声明
void delay_ms(unsigned int ms);
void display_digit(unsigned char pos, unsigned char ch);
void display_all(void);
void clear_display(void);
void set_display_string(char* str);
unsigned char get_char_index(char ch);

// ====================== 配置区 ======================
// 使用code关键字将大数组存储在程序存储器中
unsigned char data display_buffer[6] = {13, 16, 13, 16, 12, 5};  // 在RAM中

// 位选码定义（存储在CODE区）
unsigned char code BIT_SELECT[6] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf};
// 对应：第1位, 第2位, 第3位, 第4位, 第5位, 第6位
// ===================================================

// 简化段码表（只保留必要字符，存储在CODE区）
unsigned char code SEG_CODE[] = {
    // 数字 0-9
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,
    
    // 大写字母 A-F, H, L, O, P, S
    0x77,  // A
    0x7c,  // b
    0x39,  // C
    0x5e,  // d
    0x79,  // E
    0x71,  // F
    0x3d,  // G
    0x76,  // H
    0x30,  // I
    0x1e,  // J
    0x00,  // K (不支持)
    0x38,  // L
    0x00,  // M (不支持)
    0x37,  // n
    0x3f,  // O
    0x73,  // P
    0x00,  // Q (不支持)
    0x33,  // r
    0x6d,  // S
    0x31,  // T
    0x3e,  // U
    0x00,  // V (不支持)
    0x00,  // W (不支持)
    0x00,  // X (不支持)
    0x6e,  // Y
    0x00,  // Z (不支持)
    
    // 特殊符号
    0x40,  // '-' 横杠
    0x00,  // 熄灭/空格
};

/***********************************************************
* 功能：毫秒级延时函数
***********************************************************/
void delay_ms(unsigned int ms)
{
    unsigned int i, j;
    for(i = ms; i > 0; i--)
        for(j = 110; j > 0; j--);
}

/***********************************************************
* 功能：获取字符对应的段码索引
***********************************************************/
unsigned char get_char_index(char ch)
{
    if(ch >= '0' && ch <= '9')
        return ch - '0';
        
    else if(ch >= 'A' && ch <= 'Z')
    {
        // 只支持部分常用大写字母
        switch(ch)
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
            default:  return 26;  // 不支持的显示熄灭
        }
    }
    else if(ch >= 'a' && ch <= 'z')
    {
        // 支持部分小写字母
        switch(ch)
        {
            case 'a': return 10;  // 用大写A
            case 'b': return 11;  // 用大写B
            case 'c': return 12;  // 用大写C
            case 'd': return 13;  // 用大写D
            case 'e': return 14;  // 用大写E
            case 'f': return 15;  // 用大写F
            case 'h': return 17;  // 用大写H
            case 'i': return 18;  // 用大写I
            case 'l': return 21;  // 用大写L
            case 'o': return 24;  // 用大写O
            case 'p': return 25;  // 用大写P
            case 'r': return 27;  // 用小写r
            case 's': return 28;  // 用大写S
            case 't': return 29;  // 用大写T
            case 'u': return 30;  // 用大写U
            default:  return 26;  // 不支持的显示熄灭
        }
    }
    else
    {
        // 特殊字符
        switch(ch)
        {
            case '-': return 36;
            case ' ': return 26;  // 空格=熄灭
            default:  return 26;
        }
    }
}

/***********************************************************
* 功能：显示单个字符到指定位置
***********************************************************/
void display_digit(unsigned char pos, unsigned char ch_index)
{
    if(pos > 5) return;
    
    // 1. 位选：选择要显示的位置
    wela = 1;
    P0 = BIT_SELECT[pos];
    wela = 0;
    
    // 2. 段选：输出字符对应的段码
    dula = 1;
    P0 = SEG_CODE[ch_index];
    dula = 0;
    
    // 3. 短暂延时
    delay_ms(1);
    
    // 4. 消隐
    dula = 1;
    P0 = 0x00;
    dula = 0;
}

/***********************************************************
* 功能：刷新显示整个6个字符
***********************************************************/
void display_all(void)
{
    unsigned char i;
    
    for(i = 0; i < 6; i++)
    {
        display_digit(i, display_buffer[i]);
    }
}

/***********************************************************
* 功能：清空数码管显示
***********************************************************/
void clear_display(void)
{
    unsigned char i;
    
    for(i = 0; i < 6; i++)
    {
        wela = 1;
        P0 = BIT_SELECT[i];
        wela = 0;
        
        dula = 1;
        P0 = 0x00;
        dula = 0;
    }
}

/***********************************************************
* 功能：设置要显示的6个字符
***********************************************************/
void set_display_string(char* str)
{
    unsigned char i;
    
    for(i = 0; i < 6; i++)
    {
        if(str[i] == '\0')
            display_buffer[i] = 26;  // 熄灭
        else
            display_buffer[i] = get_char_index(str[i]);
    }
}

/***********************************************************
* 主函数
***********************************************************/
void main(void)
{
    // 初始化
    clear_display();
    
    // 设置要显示的字符
    set_display_string("HELLO ");  // 显示"HELLO"
    
    // 或者显示其他内容：
    // set_display_string("123456");  // 显示数字
    // set_display_string("ABCDEF");  // 显示字母
    // set_display_string("TEST  ");  // 显示"TEST"
    
    while(1)
    {
        // 不断刷新显示
        display_all();
        
        // 如果需要动态效果，可以在这里添加代码
        // 注意：不要在循环内定义大数组或调用太多函数
    }
}
