//*****************************************************************************
//
//  ACLK = LFXT1 = 32768Hz, MCLK = SMCLK = default DCO = 32 x ACLK = 1048576Hz
//  //* An external watch crystal between XIN & XOUT is required for ACLK *//	
//
//
//  M. Buccini / H. Grewal
//  Texas Instruments Inc.
//  Aug 2009
//  Built with CCE Version: 3.2.0 and IAR Embedded Workbench Version: 3.21A
//*****************************************************************************
#include  <msp430x43x.h>
#include <math.h>
#include "FDC.h"
const unsigned char digit[16] =
{
  0x7D,  /* "0" LCD segments a+b+c+d+e+f */
  0x60,  /* "1" */
  0x3E,  /* "2" */
  0x7A,  /* "3" */
  0x63,  /* "4" */
  0x5B,  /* "5" */
  0x5F,  /* "6" */
  0x70,  /* "7" */
  0x7F,  /* "8" */
  0x7B,  /* "9" */
  0x02,  /* "-" */
  0x80,  /* "." */
  0x6E,  /* "d" */
  0xEE,  /* "d." */
  0x0D,  /* "L" */
  0x00   /* " " */
};
// These need to be global in this example. Otherwise, the
// compiler removes them because they are not used

unsigned long int ADC0_Bias;
unsigned long int ADC5_Avg1;
unsigned long int ADC4_Avg2;
unsigned long int ADC3_Batt;
unsigned int ADC_Offset;
unsigned char Sample_Cnt;
unsigned char Cycle_Cnt;
unsigned char Key_Value;
unsigned char dB_dBm_Flag=0;
unsigned char Auto_Off_Flag;
unsigned char Shut_Down_Flag;
//unsigned char Low_Power_Flag;
unsigned long int Auto_Off_Cnt;
float Opm_Power;
float Opm_Power_Cnt;
float TIA_RES;
float Ref_Power;
unsigned int On_Off_Cnt;
unsigned int dB_dBm_Cnt;
unsigned int Freq_Mod_Cnt;
unsigned int Freq_Count;
unsigned char Freq_Mod_Flag;
unsigned char Direct_Flag;
float Keep_Opm_Power;
unsigned char Keep_Mod_Flag;
unsigned char Keep_Direct_Flag;
unsigned char Test_Down_Flag;
unsigned char Test_Up_Flag;
unsigned int Test_Up_Cnt;
unsigned char Back_Light_Flag;
unsigned long Cap1 = 0;
unsigned long Cap2 = 0;
unsigned long Cap3 = 0;
unsigned long Cap4 = 0;
unsigned int Cap = 0;
unsigned long int CC = 0;
unsigned int Gain1 = 0;
unsigned int M_ID = 0;
unsigned int D_ID = 0;
unsigned int Test_Cnt=0;
unsigned char Time_Cnt=0;
unsigned char Test_Flag=0;
unsigned char Send_Flag=0;
unsigned char Sent_String[4];
union  Myunion{
                 float  f;
                 unsigned char c[4];
               }f1;


#define CPU_F ((double)4000000)
#define delay_us(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define delay_ms(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))
void ADC12_Init(void);
void TimerA_Init(void);
void Disp_Low(void);
void Disp_Hig(void);
void Disp_Set(void);
void Display(unsigned int c);
void Disp_Clear(void);
void Write_Opm_Float (void);
void Read_Opm_Float (void);

/////////////////////////FDC1004 IIC////////////////////////////
void Start(void);
void Stop(void);
void Answer(void);
void NAnswer(void);
unsigned char Ack_Master(void);
unsigned char Nack_Master(void);
void SAck_Master(void);
void SNack_Master(void);
unsigned char RAnswer(void);
void WriteData(unsigned char data);
void Write_Pointer(unsigned char data);
void FDC1004_Write_Addr(unsigned char data);
void FDC1004_Read_Addr(unsigned char data);
unsigned char ReadData(void);
void Config_FDC1004(unsigned char result,unsigned char address);
void Triggering_FDC1004(unsigned char address);
void WaitFor_FDC1004(unsigned char address);
unsigned int ReadID_FDC1004(unsigned char address);
unsigned int ReadData_FDC1004(unsigned char address);
unsigned int ReadData_Avg(unsigned char result,unsigned char address);
void Uart_Send_Data(unsigned char dat);


void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    // by compiler optimization

  P1DIR  = 0xE0;
  P1OUT |= 0x00;
  P6DIR  = 0x01;
  P6OUT |= 0x00;
  
  //FCTL2 = FWKEY + FSSEL0 + FN0;             // MCLK/2 for Flash Timing Generator
  SCFQCTL = SCFQ_4M;                        //set fMCLK = 128*fACLK = 4MHz
  //FLL_CTL0 |= XCAP14PF;                     // Configure load caps
  SCFI0|=FN_2;
  //ADC12CTL0 = REFON | REF2_5V;
  //SCFQCTL = SCFQ_1M;
  FLL_CTL0|=DCOPLUS;
  //delay_ms(1000);
   //while(1);
  delay_ms(500);
  
  BTCTL = BT_fLCD_DIV32 | BT_fCLK2_DIV64
  | BT_fCLK2_ACLK_DIV256;                   // LCD clock freq is ACLK/128
  LCDCTL = LCDON+LCD4MUX + LCDSG0_1;      // LCD on, 4-Mux, segments S0-S15;
  FCTL2 = FWKEY + FSSEL0 + FN0;             // MCLK/2 for Flash Timing Generator 
  
  //while(1);
  P5SEL  = 0xFC;                             // Select P5.2-7 as Com and Rxx
  P1DIR  = 0xE0;
  P1OUT |= 0x00;
  P2DIR |= 0xC9;
  P2OUT &= 0xC0;
  P6DIR  = 0x01;
  //P6SEL |= 0x3F;                             // Enable A/D channel inputs
  P3DIR |= 0x19;
  P6OUT |= 0x00;
  /////////////////////////////////////////////////////////////////////////////
  //FLL_CTL0 |= XCAP18PF;                     // Configure load caps
  P2SEL |= 0x30;                            // P2.4,5 = USART0 TXD/RXD
  ME1 |= UTXE0 + URXE0;                     // Enable USART0 TXD/RXD
  UCTL0 |= CHAR;                            // 8-bit character
  UTCTL0 |= SSEL1;                          // UCLK = SMCLK
  UBR00 = 0x3C;                             // 1MHz 9600
  UBR10 = 0x03;                             // 1MHz 9600
  UMCTL0 = 0x2C;                            // modulation
  UCTL0 &= ~SWRST;                          // Initialize USART state machine
  IE1 |= URXIE0;                            // Enable USART0 RX interrupt
  P2DIR |= 0x10;                            // P2.4 output direction
  //////////////////////////////////////////////////////////////////////////////////
//      U0CTL=CHAR;
//      U0TCTL = SSEL1;                       // UCLK = SMCLK
//      U0BR0 = 0x45;                         // 8Mhz/115200 - 69.44
//      U0BR1 = 0x00;                         //
//      U0MCTL = 0x2C;                        // modulation
//      //////////
//      
//      ME1|=UTXE0;//+URXE0;                      //打开串行中断0
  /////////////////////////////////////////////////////////////////////////////////
  //while(1);
  //delay_ms(10);
  LCDMEM[0] = 0x00;
  LCDMEM[1] = 0xFF;
  LCDMEM[2] = 0xFF;
  LCDMEM[3] = 0xFF;
  LCDMEM[4] = 0xFF;
  LCDMEM[5] = 0x00;
  LCDMEM[6] = 0x00;
  LCDMEM[7] = 0x00; 
  Auto_Off_Flag = 1;
  Auto_Off_Cnt = 0;
  Back_Light_Flag=1;
  Back_Light_Off;
  Cap = 0;
  CC = 0;
  Time_Cnt = 0;
	Test_Cnt = 0;
	Test_Flag = 0;
  //485 out
  P2DIR|=0x08;
  P2OUT |= 0x08;
  //ADC12_Init();
  TimerA_Init();
  _EINT();
  delay_ms(200);
  M_ID = ReadID_FDC1004(0xFE);
  D_ID = ReadID_FDC1004(0xFF);
 	Config_FDC1004(4,0x08);//配置FDC1004
	Triggering_FDC1004(0x0C);//触发测量      	
	WaitFor_FDC1004(0x0C);
  Cap1 = ReadData_FDC1004(0x00);//读取电容值
	Cap2 = ReadData_FDC1004(0x02);//读取电容值
	Cap3 = ReadData_FDC1004(0x04);//读取电容值
	Cap4 = ReadData_FDC1004(0x06);//读取电容值

  LCDMEM[1] = 0x7D;
  LCDMEM[2] = 0x7D;
  LCDMEM[3] = 0x7D;
  LCDMEM[4] = 0x7D;

      	//delay_ms(1000);
      	//WaitFor_FDC1004(0x0C);//等待测量结束
  while(1)
  {
    switch(Key_Value)
    {
       case ON_OFF_UP:
         Shut_Down_Flag = 1;
         break;
      case BL_SET_UP:
      	Back_Light_On;
        Key_Value = NO_KEY;
        Auto_Off_Cnt= 0;
        break;
      case STORE_SET_UP:
      	Back_Light_Off;
      	Key_Value = NO_KEY;
      	Auto_Off_Cnt= 0;
      	break;
      case UPLOAD_SET_UP:
        Auto_Off_Cnt= 0;
        Key_Value = NO_KEY;
        break;
      case ZERO_SET_UP:
      	WaitFor_FDC1004(0x0C);
		    Cap1 = ReadData_FDC1004(0x00);//读取电容值
		  	Cap2 = ReadData_FDC1004(0x02);//读取电容值
		  	Cap3 = ReadData_FDC1004(0x04);//读取电容值
		  	Cap4 = ReadData_FDC1004(0x06);//读取电容值
		  	Cap = (Cap1+Cap2+Cap3+Cap4)/4;
		  	Display(Cap);
	  		Sent_String[0] = Cap/10000 + 0x30;
	  		Uart_Send_Data(Sent_String[0]);
	  		Sent_String[1] = Cap/1000%10 + 0x30;
	  		Uart_Send_Data(Sent_String[1]);
	  		Sent_String[2] = Cap/100%10 + 0x30;
	  		Uart_Send_Data(Sent_String[2]);
	  		Sent_String[3] = Cap/10%10 + 0x30;
	  		Uart_Send_Data(Sent_String[3]);
	  		Uart_Send_Data(0x0D);
	  		Uart_Send_Data(0x0A);
      	Key_Value = NO_KEY;
      	Auto_Off_Cnt= 0;
      	break;
      case ENTER_SET_UP:
        Key_Value = NO_KEY;
        Auto_Off_Cnt= 0;
        break;
      default:
        //Key_Value = NO_KEY;
        break;
    }
    //Disp_Low();
    if(Test_Flag==1)
    {
    	Test_Flag=0;
    	WaitFor_FDC1004(0x0C);
	    Cap1 = ReadData_FDC1004(0x00);//读取电容值
	  	Cap2 = ReadData_FDC1004(0x02);//读取电容值
	  	Cap3 = ReadData_FDC1004(0x04);//读取电容值
	  	Cap4 = ReadData_FDC1004(0x06);//读取电容值
	  	Cap = (Cap1+Cap2+Cap3+Cap4)/4;
	  	CC += Cap;
                Display(Cap);
	  	Time_Cnt++;
	  	if(Time_Cnt>=5)
	  	{
	  		Time_Cnt=0;
	  		CC = CC/5;
//	  		Sent_String[0] = CC/10000 + 0x30;
//	  		Uart_Send_Data(Sent_String[0]);
//	  		Sent_String[1] = CC/1000%10 + 0x30;
//	  		Uart_Send_Data(Sent_String[1]);
//	  		Sent_String[2] = CC/100%10 + 0x30;
//	  		Uart_Send_Data(Sent_String[2]);
//	  		Sent_String[3] = CC/10%10 + 0x30;
//	  		Uart_Send_Data(Sent_String[3]);
//	  		Uart_Send_Data(0x0D);
//	  		Uart_Send_Data(0x0A);
	  		CC=0;
	  	}
  	}
  }
}
void TimerA_Init(void)
{
  CCTL0 = CCIE;                             // CCR0 interrupt enabled
  CCR0 = 41800;                            //20ms
  TACTL = TASSEL_2 + MC_1;                  // SMCLK, up mode
}

// Timer A0 interrupt service routine
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
{
  if( (P3IN&0x80)==0x80 )//ON_OFF Press Down
  {
    On_Off_Cnt++;
    if(On_Off_Cnt>=6)
    {Key_Value = ON_OFF_DOWN;}
    else
    {Key_Value = NO_KEY;}
    if(On_Off_Cnt>=60000)
    {On_Off_Cnt = 0;}
  }
  else if( ((P1IN&0x01)==0x00) )//Back Light Press Down
  {
    On_Off_Cnt++;
    if(On_Off_Cnt>=6)
    {Key_Value = BL_SET_DOWN;}
    else
    {Key_Value = NO_KEY;}
    if(On_Off_Cnt>=60000)
    {On_Off_Cnt = 0;}
  }
  else if( (P1IN&0x02)==0x00 ) //Store Press Down
  {
    On_Off_Cnt++;
    if(On_Off_Cnt>=6)
    {Key_Value = STORE_SET_DOWN;}
    else
    {Key_Value = NO_KEY;}
    if(On_Off_Cnt>=60000)
    {On_Off_Cnt = 0;}
  }  	
  else if( ((P1IN&0x04)==0x00) )//Upload Press Down
  {
    On_Off_Cnt++;
    if(On_Off_Cnt>=6)
    {Key_Value = UPLOAD_SET_DOWN;}
    else
    {Key_Value = NO_KEY;}
    if(On_Off_Cnt>=60000)
    {On_Off_Cnt = 0;}
  }
  else if( (P1IN&0x08)==0x00 ) //Zero Press Down
  {
    On_Off_Cnt++;
    if(On_Off_Cnt>=6)
    {Key_Value = ZERO_SET_DOWN;}
    else
    {Key_Value = NO_KEY;}
    if(On_Off_Cnt>=60000)
    {On_Off_Cnt = 0;}
  }  	
  else if( ((P1IN&0x10)==0x00) )//Enter Press Down
  {
    On_Off_Cnt++;
    if(On_Off_Cnt>=6)
    {Key_Value = ENTER_SET_DOWN;}
    else
    {Key_Value = NO_KEY;}
    if(On_Off_Cnt>=60000)
    {On_Off_Cnt = 0;}
  }
  else
  {
    On_Off_Cnt = 0;
  }
  if( ( (P3IN&0x80)==0x80 ) && (Key_Value == ON_OFF_DOWN) )//ON_OFF Press Up
  {Key_Value = ON_OFF_UP;}
  if( ( (P1IN&0x01)==0x01 ) && (Key_Value == BL_SET_DOWN) )//BL Press Up
  {Key_Value = BL_SET_UP;}
  if( ( (P1IN&0x02)==0x02 ) && (Key_Value == STORE_SET_DOWN) )//STORE Press Up
  {Key_Value = STORE_SET_UP;}
  if( ( (P1IN&0x04)==0x04 ) && (Key_Value == UPLOAD_SET_DOWN) )//UPLOAD Press Up
  {Key_Value = UPLOAD_SET_UP;}
  if( ( (P1IN&0x08)==0x08 ) && (Key_Value == ZERO_SET_DOWN) )//ZERO Press Up
  {Key_Value = ZERO_SET_UP;}
  if( ( (P1IN&0x10)==0x10 ) && (Key_Value == ENTER_SET_DOWN) )//ENTER Press Up
  {Key_Value = ENTER_SET_UP;}
  //if( ((P1IN&0x02)==0x00) && (Low_Power_Flag == 1) )
  //{
    //Low_Power_Flag = 0;
    //_BIC_SR_IRQ(LPM0_bits);                 // Clear LPM3 bits from 0(SR)
  //}
  if(Auto_Off_Flag==1)
  {Auto_Off_Cnt++;}
  Test_Cnt++;
  if(Test_Cnt>=200)//12000  =  60s
  {
  	Test_Cnt = 0;
  	Test_Flag = 1;
  }
  //P3OUT ^= 0x08;
}
void ADC12_Init(void)
{
  ADC12CTL0 = ADC12ON+MSC+SHT0_2+REFON+REF2_5V; 
                                            // Turn on ADC12, extend sampling time
                                            // to avoid overflow of results
  ADC12CTL1 = SHP+CONSEQ_1;                 // Use sampling timer, repeated sequence
  ADC12MCTL0 = INCH_0;                      // ref+=AVcc, channel = A0
  ADC12MCTL1 = INCH_1;                      // ref+=AVcc, channel = A1
  ADC12MCTL2 = INCH_2;                      // ref+=AVcc, channel = A2
  ADC12MCTL3 = INCH_3;                      // ref+=AVcc, channel = A3
  ADC12MCTL4 = INCH_4;                      // ref+=AVcc, channel = A4
  ADC12MCTL5 = INCH_5+EOS;                  // ref+=AVcc, channel = A5, end seq.
  ADC12IE = 0x08;                           // Enable ADC12IFG.3
  ADC12CTL0 |= ENC;                         // Enable conversions
  //ADC12CTL0 |= ADC12SC;                     // Start conversion
}
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
  ADC3_Batt = ADC12MEM3;

}
void Disp_Low(void)
{
  LCDMEM[0] = 0x00;
  LCDMEM[1] = 0x00;
  LCDMEM[2] = 0x00;
  LCDMEM[3] = 0x00;
  LCDMEM[4] = 0x8D;
  LCDMEM[5] = 0x7D;
  LCDMEM[6] = 0x02;
  if(dB_dBm_Flag == 0)
  {LCDMEM[7] &= 0x87;LCDMEM[7] |= 0x48;}
  else
  {LCDMEM[7] &= 0x87;LCDMEM[7] |= 0x40;}
  P6OUT |= 0x80;
}
void Disp_Hig(void)
{
	
  LCDMEM[0] = 0x00;
  LCDMEM[1] = 0x00;
  LCDMEM[2] = 0x00;
  LCDMEM[3] = 0x00;
  LCDMEM[4] = 0xE7;
  LCDMEM[5] = 0x7D;
  LCDMEM[6] = 0x02;
  if(dB_dBm_Flag == 0)
  {LCDMEM[7] &= 0x87;LCDMEM[7] |= 0x48;}
  else
  {LCDMEM[7] &= 0x87;LCDMEM[7] |= 0x40;}
  P6OUT |= 0x80;
}
void Disp_Set(void)
{
  LCDMEM[0] = 0x00;
  LCDMEM[1] = 0x00;
  LCDMEM[2] = 0x00;	
  LCDMEM[3] = 0x00;
  LCDMEM[4] = 0x5B;
  LCDMEM[5] = 0x1F;
  LCDMEM[6] = 0x70;
  P6OUT |= 0x80;
}
void Display(unsigned int c)
{
  unsigned char i;
  unsigned char Disp1[4];
  i = c/10000;
	Disp1[0] = digit[i];
	i = c/1000%10;
	Disp1[1] = digit[i];
	i = c/100%10;
	Disp1[2] = digit[i];
	i = c/10%10;
	Disp1[3] = digit[i];
  for(i=0;i<4;i++)
  {LCDMEM[i+1] = Disp1[i];}
}
void Disp_Clear(void)
{
  LCDMEM[0] = 0x00;
  LCDMEM[1] = 0x00;
  LCDMEM[2] = 0x00;
  LCDMEM[3] = 0x00;
  LCDMEM[4] = 0x00;
  LCDMEM[5] = 0x00;
  LCDMEM[6] = 0x00;
}
void Write_Opm_Float(void)
{
  unsigned char *StartAdr=(unsigned char *)0x01000;
  int *ptr=(int*)0x01000;
//  erase flash
  FCTL1=0x0A502;            //Erease=1
  FCTL3=0x0A500;            //LOCK=0
  *ptr=0;
//  
  FCTL1=0X0A540;            //WRT=1
  //存储量程 
  *StartAdr=f1.c[0];
  *(StartAdr+1)=f1.c[1];
  *(StartAdr+2)=f1.c[2];
  *(StartAdr+3)=f1.c[3];
  *(StartAdr+4)=dB_dBm_Flag;
}
void Read_Opm_Float(void)
{
 unsigned char *StartAdr=(unsigned char*)0x01000; 
  f1.c[0]=*StartAdr;
  f1.c[1]=*(StartAdr+1);
  f1.c[2]=*(StartAdr+2);
  f1.c[3]=*(StartAdr+3);
  dB_dBm_Flag=*(StartAdr+4);
}
/*******************************************************************************
IO口模拟IIC，读写AD7416
********************************************************************************/
/*******************************************************************************
* 函数名        :  start
* 功能描述	  	:  
* 入口函数      :  
* 返回值  	  	: 
*******************************************************************************/
void Start(void)
{
    FDC1004_SDA_OUT;
    FDC1004_SCL_OUT;
    FDC1004_SDA_HIG;
		delay_us(10);
    FDC1004_SCL_HIG;
		delay_us(10);
    FDC1004_SDA_LOW;
		delay_us(10);
    FDC1004_SCL_LOW;
 }
 /*******************************************************************************
* 函数名        :  start
* 功能描述	  	:  
* 入口函数      :  
* 返回值  	  	: 
*******************************************************************************/
void Stop(void)
{
    FDC1004_SDA_OUT;
    FDC1004_SDA_LOW;
		delay_us(10); 
    FDC1004_SCL_HIG;
    delay_us(10);
    FDC1004_SDA_HIG;
 }
  /*******************************************************************************
* 函数名        :  Answer
* 功能描述	  	:  
* 入口函数      :  
* 返回值  	  	: 
*******************************************************************************/
void Answer(void)
{
    FDC1004_SDA_OUT;
    FDC1004_SCL_LOW;
		delay_us(10); 
    FDC1004_SDA_OUT;
    FDC1004_SDA_LOW;
    delay_us(10);
    FDC1004_SCL_HIG;
    delay_us(10);
    FDC1004_SCL_LOW;
    FDC1004_SDA_HIG;
  }
 /*******************************************************************************
* 函数名        :  NAnswer
* 功能描述	  	:  
* 入口函数      :  
* 返回值  	  	: 
*******************************************************************************/
void NAnswer(void)
{
    FDC1004_SDA_OUT;
    FDC1004_SDA_HIG;
		delay_us(10); 
    FDC1004_SCL_HIG;
    delay_us(10);
    FDC1004_SCL_LOW;
}
 /*******************************************************************************
* 函数名        :  ACK MASTER
* 功能描述	  	:  
* 入口函数      :  
* 返回值  	  	: 
*******************************************************************************/
unsigned char Ack_Master(void)
{ 
//	FDC1004_SDA_OUT;
//	delay_us(10);
//	FDC1004_SDA_LOW;
//	delay_us(10);
//  FDC1004_SCL_HIG;
//	delay_us(10);
//  FDC1004_SCL_LOW;
    FDC1004_SDA_IN;
    delay_us(10);
    FDC1004_SCL_HIG;
    delay_us(10);
    if((P2IN&0x80)==0x00)
    { 
        FDC1004_SCL_LOW;
        FDC1004_SDA_OUT;
        return(0);    
    }
    else
    {  
        FDC1004_SCL_LOW;
        FDC1004_SDA_OUT;
        return(1);
    }
}
void SAck_Master(void)
{ 
	FDC1004_SDA_OUT;
	delay_us(10);
	FDC1004_SDA_LOW;
	delay_us(10);
  FDC1004_SCL_HIG;
	delay_us(10);
  FDC1004_SCL_LOW;
}
 /*******************************************************************************
* 函数名        :  NACK MASTER
* 功能描述	  	:  
* 入口函数      :  
* 返回值  	  	: 
*******************************************************************************/
unsigned char Nack_Master(void)
{
//    FDC1004_SDA_OUT;
//    FDC1004_SDA_HIG;
//		delay_us(10); 
//    FDC1004_SCL_HIG;
//    delay_us(10);
//    FDC1004_SCL_LOW;
    //FDC1004_SDA_HIG;
    FDC1004_SDA_IN;
    delay_us(10);
    FDC1004_SCL_HIG;
    delay_us(10);
    if((P2IN&0x80)==0x00)
    { 
        FDC1004_SCL_LOW;
        FDC1004_SDA_OUT;
        return(0);    
    }
    else
    {  
        FDC1004_SCL_LOW;
        FDC1004_SDA_OUT;
        return(1);
    }
}
void SNack_Master(void)
{
    FDC1004_SDA_OUT;
    FDC1004_SDA_HIG;
		delay_us(10); 
    FDC1004_SCL_HIG;
    delay_us(10);
    FDC1004_SCL_LOW;
}
 /*******************************************************************************
* 函数名        :  RAnswer
* 功能描述	  	:  
* 入口函数      :  
* 返回值  	  	: 
*******************************************************************************/
unsigned char RAnswer(void)
{
    //FDC1004_SDA_HIG;
    FDC1004_SDA_IN;
    delay_us(10);
    FDC1004_SCL_HIG;
    delay_us(10);
    if((P2IN&0x80)==0x00)
    { 
        FDC1004_SCL_LOW;
        FDC1004_SDA_OUT;
        return(0);    
    }
    else
    {  
        FDC1004_SCL_LOW;
        FDC1004_SDA_OUT;   
        return(1);
    }
  }
/*******************************************************************************
* 函数名        :  start
* 功能描述	  	:  
* 入口函数      :  
* 返回值  	  	: 
*******************************************************************************/ 
void WriteData(unsigned char data)
{   unsigned char i;
    FDC1004_SDA_OUT;
    FDC1004_SCL_LOW;
		delay_us(10); 
    for(i=0;i<8;i++)
    {
          if ((data&0x80)==0x80)          //1
            {
            	FDC1004_SDA_HIG;
            }        
          else                          //0
            {
            	FDC1004_SDA_LOW;
            }
		  delay_us(10);
          FDC1004_SCL_HIG;
		  delay_us(10);
          FDC1004_SCL_LOW;	
          data=data<<1;
     }
		delay_us(10);
//		FDC1004_SDA_LOW;
//		delay_us(10);
//    FDC1004_SCL_HIG;
//		delay_us(10);
//    FDC1004_SCL_LOW;		    
    if(RAnswer()==1)
    {Stop();} 
}
/*******************************************************************************
* 函数名        :  start
* 功能描述	  	:  
* 入口函数      :  
* 返回值  	  	: 
*******************************************************************************/ 
void Write_Pointer(unsigned char data)
{   unsigned char i;
    FDC1004_SDA_OUT;
    FDC1004_SCL_LOW;
		delay_us(10); 
    for(i=0;i<8;i++)
    {
          if ((data&0x80)==0x80)          //1
            {
            	FDC1004_SDA_HIG;
            }        
          else                          //0
            {
            	FDC1004_SDA_LOW;
            }
		  delay_us(10);
          FDC1004_SCL_HIG;
		  delay_us(10);
          FDC1004_SCL_LOW;	
          data=data<<1;
     }
		delay_us(10);
		FDC1004_SDA_LOW;
		delay_us(10);
    FDC1004_SCL_HIG;
		delay_us(10);
    FDC1004_SCL_LOW;
		delay_us(10);
    FDC1004_SDA_HIG;
		delay_us(10);
    FDC1004_SCL_HIG;		    
    //if(RAnswer()==1)
    //{Stop();} 
}
void FDC1004_Write_Addr(unsigned char data)
{   
	unsigned char i;
	
	data = data*2;//7位地址补齐成8位
    FDC1004_SDA_OUT;
    FDC1004_SCL_LOW;
		delay_us(10); 
    for(i=0;i<8;i++)
    {
          if ((data&0x80)==0x80)          //1
            {
            	FDC1004_SDA_HIG;
            }        
          else                          //0
            {
            	FDC1004_SDA_LOW;
            }
		  delay_us(10);
          FDC1004_SCL_HIG;
		  delay_us(10);
          FDC1004_SCL_LOW;	
          data=data<<1;
     }
		delay_us(10);
//		FDC1004_SDA_LOW;
//		delay_us(10);
//    FDC1004_SCL_HIG;
//		delay_us(10);
//    FDC1004_SCL_LOW;  
    if(RAnswer()==1)
    {Stop();} 
}
void FDC1004_Read_Addr(unsigned char data)
{   
	unsigned char i;
	
	data = data*2+1;//7位地址补齐成8位,最后一位变高
    FDC1004_SDA_OUT;
    FDC1004_SCL_LOW;
		delay_us(10); 
    for(i=0;i<8;i++)
    {
          if ((data&0x80)==0x80)          //1
            {
            	FDC1004_SDA_HIG;
            }        
          else                          //0
            {
            	FDC1004_SDA_LOW;
            }
		  delay_us(10);
          FDC1004_SCL_HIG;
		  delay_us(10);
          FDC1004_SCL_LOW;	
          data=data<<1;
     }
		delay_us(10);
//		FDC1004_SDA_LOW;
//		delay_us(10);
//    FDC1004_SCL_HIG;
//		delay_us(10);
//    FDC1004_SCL_LOW; 
    if(RAnswer()==1)
    {Stop();} 
}

/*******************************************************************************
* 函数名        :  start
* 功能描述	  	:  
* 入口函数      :  
* 返回值  	  	: 
*******************************************************************************/
unsigned char ReadData(void)
{
	unsigned char tmp=0;
	unsigned char i;
	FDC1004_SDA_IN;
	FDC1004_SCL_LOW;
	for(i=0;i<8;i++)
	{
	 tmp=tmp<<1;
	 FDC1004_SCL_HIG;
	 delay_us(10);
	
	 if((P2IN&0x80)==0x80) 
	 {tmp|=0x01;}
	  FDC1004_SCL_LOW;
	  delay_us(10);
	}
//	FDC1004_SDA_OUT;
//		delay_us(10);
//		FDC1004_SDA_LOW;
//		delay_us(10);
//    FDC1004_SCL_HIG;
//		delay_us(10);
//    FDC1004_SCL_LOW;
return(tmp);
}

void Config_FDC1004(unsigned char result,unsigned char address)//配置FDC1004
{
	unsigned char i;
	for(i=0;i<result;i++)
	{
    Start();
    FDC1004_Write_Addr(0x50);//first write address
    WriteData(address);          // pointer register 0x08
    WriteData(0x08);          // high bit data
    WriteData(0x00);          //low bit data 
    Stop();	
    delay_us(100);
    address++;
	}
}
void Triggering_FDC1004(unsigned char address)//触发FDC1004
{
    Start();
    FDC1004_Write_Addr(0x50);//first write address
    WriteData(address);          // pointer register 0x0C
    WriteData(0x05);          // high bit data
    WriteData(0xF0);          //low bit data
    Stop();
	
}
void WaitFor_FDC1004(unsigned char address)//等待测量结束FDC1004
{
	unsigned int rdata;
	unsigned char data;
	while(1)
	{
    Start();
    FDC1004_Write_Addr(0x50);//first write address
    Write_Pointer(address);          // pointer register 0x0C  
    Start();  
    FDC1004_Read_Addr(0x50);
    rdata = ReadData();
    SAck_Master();
    //if(Ack_Master()==1)
  	//{Stop();}
    data = ReadData();
    SNack_Master();
    Stop();
    //if(Nack_Master()==1)
  	//{Stop();}
  	rdata = rdata*256 + data;
    if((rdata&0x000F)==0x000F)
    {break;}
    delay_ms(100);
	}
}
//////////////read from FDC1004 by one byte///////////////
unsigned int ReadID_FDC1004(unsigned char address)               /*单字节*/
{   
    unsigned int rdata;
    unsigned char data;
    Start();
    FDC1004_Write_Addr(0x50);//first write address
    Write_Pointer(address);          // pointer register 0x00 
    Start();   
    FDC1004_Read_Addr(0x50);
    rdata = ReadData();
    SAck_Master();
    //if(Ack_Master()==1)
  	//{Stop();}
    data = ReadData();
    SNack_Master();
    Stop();
    //if(Nack_Master()==1)
    //{Stop();}
    rdata = rdata*256 + data;
    
    return(rdata);
}
//////////////read from FDC1004 by one byte///////////////
unsigned int ReadData_FDC1004(unsigned char address)               /*单字节*/
{   
    unsigned int rdata1,rdata2;
    unsigned int rdata=0;
    unsigned char data;
    Start();
    FDC1004_Write_Addr(0x50);//first write address
    Write_Pointer(address);          // pointer register 0x00    
    Start();
    FDC1004_Read_Addr(0x50);
    rdata1 = ReadData();
    SAck_Master();
    //if(Ack_Master()==1)
  	//{Stop();}
    data = ReadData();
    SNack_Master();
    Stop();
    //if(Nack_Master()==1)
    //{Stop();}
    rdata = rdata1*256 + data;
    
    delay_us(100);
    Start();
    FDC1004_Write_Addr(0x50);//first write address
    WriteData(address+1);          // pointer register 0x00    
    FDC1004_Read_Addr(0x50);
    rdata2 = ReadData();
    rdata2 = 0;
    SAck_Master();
    //if(Ack_Master()==1)
  	//{Stop();}
    data = ReadData();
    SNack_Master();
    Stop();
    //if(Nack_Master()==1)
    //{Stop();}
    //rdata2 = rdata2*256 + data;
    rdata = rdata + rdata2;
    return(rdata);
}
unsigned int ReadData_Avg(unsigned char result,unsigned char address)
{
	unsigned int avg = 0;
	unsigned char i;
	address = 0x00;
	for(i=0;i<result;i++)
	{
		avg += ReadData_FDC1004(address);
		address +=2;
	}
	return(avg);
}
void Uart_Send_Data(unsigned char dat)
{
	unsigned int j;
  //while (!(IFG1 & UTXIFG0));                // USART0 TX buffer ready?
  //TXBUF0 = dat;                          // RXBUF0 to TXBUF0	
  TXBUF0=dat;
  j=0;
  while((UTCTL0&0X01)==0)
  {
   j++;
   if(j>10000) 
    {break;}
  } 
}