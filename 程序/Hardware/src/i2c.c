#include <reg52.h>
#include "i2c.h"
#include "delay.h"

//函数声明
//开始
void start()
{
    sda = 1;
    delay_us();
    scl = 1;
    delay_us();
    sda = 0;
    delay_us();
}

//结束
void stop()
{
    sda = 0;
    delay_us();
    scl = 1;
    delay_us();
    sda = 1;
    delay_us();
}

//等待应答
void respons()
{
    uchar i;
    scl = 1;
    delay_us();

    while((sda==1) && (i<250))
        i++;

    scl = 0;
    delay_us();
}

//写数据
void write_byte(uchar dat)
{
    uchar i,temp;
    temp = dat;
    for(i=0; i<8; i++)
    {
        temp = temp << 1;
        scl = 0;
        delay_us();
        sda = CY;
        delay_us();
        scl = 1;
        delay_us();
    }

    scl = 0;
    delay_us();
    sda = 1;
    delay_us();
}

//读数据
uchar read_byte()
{
    uchar i,k;
    scl = 0;
    delay_us();
    sda = 1;
    delay_us();
    for(i=0; i<8; i++)
    {
        scl = 1;
        delay_us();
        k = (k<<1) | sda;
        scl = 0;
        delay_us();
    }
    return k;
}

//向芯片的指定地址，写入数据
void write_at_addr(uchar addr,uchar dat)
{
    start();
    //设定地址，AT24C02固定地址为1010，再加上设定的地址，
    //我设置为000，最后一位表示读或写，0表示写，所以为1010 0000，即0xa0
    write_byte(0xa0);
    respons();//等待芯片应答
    write_byte(addr);//设置要写入数据的地址
    respons();//等待芯片应答
    write_byte(dat);//写入要保存的数据
    respons();//等待芯片应答
    stop();
}

//读取芯片指定地址的数据
uchar read_at_addr(uchar addr)
{
    uchar dat;
    start();
    write_byte(0xa0);//向芯片发送一个写操作信号
    respons();
    write_byte(addr);//设置要读取的地址
    respons();
    start();
    write_byte(0xa1);//向芯片发送一个读操作信号
    respons();
    dat = read_byte();
    stop();
    return dat;
}
