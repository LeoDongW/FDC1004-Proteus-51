#ifndef __LCD12864_H__
#define __LCD12864_H__

extern void          _nop_     (void);
// LCD128*64 数据线
#define LCD12864DataPort P1
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif


//// LCD128*64 I/O 信号管脚
sbit di = P2^2; // 数据\指令 选择
sbit rw = P2^1; // 读\写 选择
sbit en = P2^0; // 读\写使能
sbit cs1= P3^0; // 片选1,低有效(前64列)
sbit cs2= P3^1; // 片选2,低有效(后64列)


void nop(void);
void CheckState(void);
void WriteByte(uchar dat);
void lcd_cmd_wr(uchar command);
void SendCommandToLCD(uchar command);
void SetLine(uchar line);
void ClearScreen(uchar screen);
void ShowNumber(uchar lin,uchar column,uchar num);
void ShowChina(uchar lin,uchar column,uchar num);
void InitLCD(void);
void digplaystart();
void DisP_parameters(int x,int y,int m);
extern void          _nop_     (void);
#endif