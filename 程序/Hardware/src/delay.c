#include <reg52.h>
#include "delay.h"

void delayms(unsigned int n)  //��ʱnms
{
    unsigned int i,j;
    for(j=n;j>0;j--)
        for(i=112;i>0;i--);

}

//��ʱ10*n΢��
void delayus(unsigned int n)
{
    while(n--);
}

void delay10ms(void) //��ʱ10ms
{
    unsigned char i,j;
    for(i=0; i<255; i++)
        for(j=0; j<200; j++);
}
void delay5ms(void)   //���0us
{
    unsigned char a,b;
    for(b=19; b>0; b--)
        for(a=130; a>0; a--);
}

//��ʱ
void delay(uchar t)
{
    uchar i,j;
    for(i=0; i<t; i++)
        for(j=0; j<110; j++)
            ;
}

//��ʱ4-5΢��
void delay_us()
{
    ;;;
}

