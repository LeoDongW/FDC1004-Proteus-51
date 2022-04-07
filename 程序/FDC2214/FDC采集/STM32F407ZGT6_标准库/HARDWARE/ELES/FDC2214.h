#ifndef _FDC2214_H_
#define _FDC2214_H_

#include "myiic.h"

typedef   char            fdc_s8;
typedef   short           fdc_s16;
typedef   int             fdc_s32;

typedef unsigned char     fdc_u8;
typedef unsigned short    fdc_u16;
typedef unsigned int      fdc_u32;

#define CH0   Channel_Ch0
#define CH1   Channel_Ch1
#define CH2   Channel_Ch2
#define CH3   Channel_Ch3



/****************define iic  function********************/
#define    fdc_iic_init()                       IIC_Init()
#define    fdc_iic_start()                      IIC_Start()
#define    fdc_iic_stop()                       IIC_Stop()
#define    fdc_iic_send_byte(u8_txd)            IIC_Send_Byte(u8_txd)
#define    fdc_iic_read_byte(which_ack)         IIC_Read_Byte(which_ack)
#define    fdc_iic_wait_ack()                   IIC_Wait_Ack()
#define    fdc_iic_send_ack()                   IIC_Ack()
#define    fdc_iic_send_nack()                  IIC_NAck()

//define  others  init function. them will be used in FDC_Init. 
#define    FDC_GPIO_INIT()                   FDC_Gpio_Init() 
#define    FDC_IIC_INIT()                    fdc_iic_init() 
#define    FDC_SET_ADDR()                               
#define    FDC_SET_SDPIN()                                    

/*I2C Address selection pin: when ADDR=L, I2C address = 0x2A, when ADDR=H, I2C address = 0x2B.*/
#define    FDC_I2C_ADRR    0x2A //为2A则ADRR引脚接地,为2B则接高

#define    FDC_I2C_WADDR   (FDC_I2C_ADRR<<1)    
#define    FDC_I2C_RADDR   ((FDC_I2C_ADRR<<1) | 0x01) 

#define    External_Src    40         //40MHz
#define    PARA_SENSOR_C   33         //33pF
#define    PARA_SENSOR_L   18         //18uH

/***************************ADDR REGISTER *************************************/
typedef enum
{
    /* DATA */
    FDC_DATA_MSB_CH0  = 0x00 ,/*Channel 0 MSB Conversion Result and status*/
    FDC_DATA_LSB_CH0  = 0x01 ,/*Channel 0 LSB Conversion Result. Must be read after Register address 0x00*/
    FDC_DATA_MSB_CH1  = 0x02 ,
    FDC_DATA_LSB_CH1  = 0x03 ,
    FDC_DATA_MSB_CH2  = 0x04 ,
    FDC_DATA_LSB_CH2  = 0x05 ,
    FDC_DATA_MSB_CH3  = 0x06 ,
    FDC_DATA_LSB_CH3  = 0x07 ,
    /*Channel n Reference Count Conversion Interval Time*/
    /*0x0000-0x00FF: Reserved*/
    /*0x0100-0xFFFF: Conversion Time (tCn) =(CHn_RCOUNT*16)/fREFn*/
    FDC_RCOUNT_CH0    = 0x08,/*Reference Count setting for Channel 0*/
    FDC_RCOUNT_CH1     ,
    FDC_RCOUNT_CH2     ,
    FDC_RCOUNT_CH3     ,  
    /* Channel n Conversion Settling
    The FDC will use this settling time to allow the LC sensor to
    stabilize before initiation of a conversion on Channel 0.
    If the amplitude has not settled prior to the conversion start, an
    Amplitude warning will be generated if reporting of this type of
    warning is enabled.
    b0000 0000 0000 0000: Settle Time (tSn)= 32 ÷ fREFn
    b0000 0000 0000 0001: Settle Time (tSn)= 32 ÷ fREFn
    b0000 0000 0000 0010 - b1111 1111 1111 1111: 
    Settle Time (tSn)= (CHn_SETTLECOUNT*16) ÷ fREFn */
    FDC_SETTLECOUNT_CH0    = 0x10,/*Channel 0 Settling Reference Count*/
    FDC_SETTLECOUNT_CH1     ,
    FDC_SETTLECOUNT_CH2     ,
    FDC_SETTLECOUNT_CH3     ,  
    /*13:12*/
    /*Channel n Sensor frequency select
    for differential sensor configuration:
    b01: divide by 1. Choose for sensor frequencies between 0.01MHz and 8.75MHz
    b10: divide by 2. Choose for sensor frequencies between 5MHz and 10MHz
    for single-ended sensor configuration:
    b10: divide by 2. Choose for sensor frequencies between 0.01MHz and 10MHz*/
    /*9:0*/
    /*Channel n Reference Divider Sets the divider for Channel n
    reference. Use this to scale the maximum conversion frequency.
    b00’0000’0000: Reserved. Do not use.
    CH0_FREF_DIVIDER≥b00’0000’0001: fREF0 =  fCLK/CH0_FREF_DIVIDER*/
    FDC_CLOCK_DIVIDERS_CH0 = 0x14,/*Reference divider settings for Channel 0*/
    FDC_CLOCK_DIVIDERS_CH1  ,
    FDC_CLOCK_DIVIDERS_CH2  ,
    FDC_CLOCK_DIVIDERS_CH3  ,  
    FDC_STATUS             = 0x18,/*Device Status Reporting*/
    FDC_STATUS_CONFIG      = 0x19,/*Device Status Reporting Configuration*/
    FDC_CONFIG             = 0x1a,/*Conversion Configuration*/
    FDC_MUX_CONFIG         = 0x1b,/*Channel Multiplexing Configuration*/
    FDC_RESET_DEV          = 0x1c,/*Reset Device*/     
    /*Channel 2 Sensor drive current
    This field defines the Drive Current to be used during the settling
    + conversion time of Channel 2 sensor clock. Set such that 1.2V
    ≤ sensor oscillation amplitude (pk) ≤ 1.8V
    00000: 0.016mA
    00001: 0.018mA
    00010: 0.021mA
    ...
    11111: 1.571mA*/
    FDC_DRIVE_CURRENT_CH0  = 0x1e,/*Channel 0 sensor current drive configuration*/
    FDC_DRIVE_CURRENT_CH1   ,
    FDC_DRIVE_CURRENT_CH2   ,
    FDC_DRIVE_CURRENT_CH3   ,
    
    FDC_MANUFACTURER_ID    = 0x7e,/*Manufacturer ID = 0x5449*/
    FDC_DEVICE_ID          = 0x7f,/*Device ID. FDC2214's ID is 0x3055*/
}_REGISTER_ADDR_TypeDef;
/*************************END ADDR REGISTER END***********************************/

/**************************** MASK FOR REGISTER***********************************/
/**********DATA_MSB_CHn***********/ 
#define   FDC_CHn_ERR_WD           0x2000  /*Channel n Conversion Watchdog Timeout Error Flag. Cleared by reading the bit.*/
#define   FDC_CHn_ERR_AW           0x1000  /*Channel n Amplitude Warning. Cleared by reading the bit.*/
#define   FDC_MSB_DATAn            0x0fff  /*Channel n Conversion Result DATAn[27:16]*/

/*******CLOCK_DIVIDERS_CHn********/
#define   FDC_CHn_FIN_SEL          0x3000
#define   FDC_CHn_FREF_DIVIDER     0x03ff

/************STATUS***************/
#define   FDC_ERR_CHAN             0xc000
#define   FDC_ERR_WD               0x0800
#define   FDC_ERR_AHW              0x0400
#define   FDC_ERR_ALW              0x0200
#define   FDC_DRDY                 0x0040
#define   FDC_CH0_UNREADCONV       0x0008
#define   FDC_CH1_UNREADCONV       0x0004
#define   FDC_CH2_UNREADCONV       0x0002
#define   FDC_CH3_UNREADCONV       0x0001

/**********STATUS_CONFIG***********/
#define   FDC_WD_ERR2OUT           0x2000
#define   FDC_AH_WARN2OUT          0x1000
#define   FDC_AL_WARN2OUT          0x0800
#define   FDC_WD_ERR2INT           0x0020
#define   FDC_DRDY_2INT            0x0001

/************CONFIG***************/
#define   FDC_ACTIVE_CHAN          0xc000
#define   FDC_SLEEP_MODE_EN        0x2000
#define   FDC_SENSOR_ACTIVATE_SEL  0x0800
#define   FDC_REF_CLK_SRC          0x0200
#define   FDC_INTB_DIS             0x0080
#define   FDC_HIGN_CURRENT_DRV     0x0040

/***********MUX_CONFIG************/
#define   FDC_AUTOSCAN_EN          0x8000
#define   FDC_RR_SEQUENCE          0x6000
#define   FDC_DEGLITCH             0x0007

/********DRIVE_CURRENT_CHn**********/
#define   FDC_CHn_IDRIVE           0xf800

/***********************END MASK FOR REGISTER END**********************************/
typedef struct _st_fdc_init_typedef
{
    /***config channel  ***/
    fdc_u16 Channel_Mode           ;//Channel_Mode_typedef
    fdc_u16 Channel_Single_Option  ;//Channel_Option_Single_typedef
    fdc_u16 Channel_Muti_Option    ;//Channel_Option_Muti_typedef
    
    /***config CLK   *****/
    fdc_u16 Clk_Src                ;//Clk_Src_typedef
    
    fdc_u16 Clk_Fref_Divider_Ch0   ;//between 0 and 511
    fdc_u16 Clk_Fref_Divider_Ch1   ;
    fdc_u16 Clk_Fref_Divider_Ch2   ;
    fdc_u16 Clk_Fref_Divider_Ch3   ;
    
    fdc_u16 Clk_Fin_Sel_Ch0        ;//Clk_Fin_Sel_typedef
    fdc_u16 Clk_Fin_Sel_Ch1        ;
    fdc_u16 Clk_Fin_Sel_Ch2        ;
    fdc_u16 Clk_Fin_Sel_Ch3        ;
    
    /*** config time ****/
    fdc_u16 Time_SettleCount_Ch0   ;//between 0 and 65535
    fdc_u16 Time_SettleCount_Ch1   ;
    fdc_u16 Time_SettleCount_Ch2   ;
    fdc_u16 Time_SettleCount_Ch3   ;
    
    fdc_u16 Time_RCount_Ch0        ;//between 0 and 65535
    fdc_u16 Time_RCount_Ch1        ;
    fdc_u16 Time_RCount_Ch2        ;
    fdc_u16 Time_RCount_Ch3        ;    
    
    /*** config current ***********/
    fdc_u16 Current_Activation_Mode;//Current_Activation_Mode_typedef
    
    fdc_u16 Current_High_Mode      ;//Current_High_Mode_typedef
    
    fdc_u16 Current_Driver_Ch0     ;//Current_Driver_typedef
    fdc_u16 Current_Driver_Ch1     ;
    fdc_u16 Current_Driver_Ch2     ;
    fdc_u16 Current_Driver_Ch3     ;
    
    /*** config Filter**************/
    fdc_u16 Filter_Input_Deglitch  ;//Filter_Input_Deglitch_typedef 
    
    /**** config ERROR_CONFIG****************/
    fdc_u16 Wd_Err2out             ;//Error_Config_typedef
    fdc_u16 Ah_Warn2out            ;
    fdc_u16 Al_Warn2out            ;
    fdc_u16 Wd_Err2int             ;
    fdc_u16 Drdy_2int              ;
    
}st_fdc_init_typedef;


typedef struct _st_register_typedef
{
    fdc_u16 DATA_MSB_CH0          ;
    fdc_u16 DATA_LSB_CH0          ;
    fdc_u16 DATA_MSB_CH1          ;
    fdc_u16 DATA_LSB_CH1          ;
    fdc_u16 DATA_MSB_CH2          ;
    fdc_u16 DATA_LSB_CH2          ;
    fdc_u16 DATA_MSB_CH3          ;
    fdc_u16 DATA_LSB_CH3          ;
    
    fdc_u16 RCOUNT_CH0            ;
    fdc_u16 RCOUNT_CH1            ;
    fdc_u16 RCOUNT_CH2            ;
    fdc_u16 RCOUNT_CH3            ;
    
    
    fdc_u16 SETTLECOUNT_CH0       ;
    fdc_u16 SETTLECOUNT_CH1       ;
    fdc_u16 SETTLECOUNT_CH2       ;
    fdc_u16 SETTLECOUNT_CH3       ;
    
    fdc_u16 CLOCK_DIVIDERS_CH0    ;
    fdc_u16 CLOCK_DIVIDERS_CH1    ;
    fdc_u16 CLOCK_DIVIDERS_CH2    ;
    fdc_u16 CLOCK_DIVIDERS_CH3    ;
    
    fdc_u16 STATUS                ;
    fdc_u16 ERROR_CONFIG          ;
    fdc_u16 CONFIG                ;
    fdc_u16 MUX_CONFIG            ;
    fdc_u16 RESET_DEV             ;
    
    fdc_u16 DRIVE_CURRENT_CH0     ;
    fdc_u16 DRIVE_CURRENT_CH1     ;
    fdc_u16 DRIVE_CURRENT_CH2     ;
    fdc_u16 DRIVE_CURRENT_CH3     ;
    
    fdc_u16 MANUFACTURER_ID       ;
    fdc_u16 DEVICE_ID             ;
}st_register_typedef;

typedef struct _st_status_typedef
{
    fdc_u8   STATUS_ERR_CHAN            ;
    fdc_u8   STATUS_ERR_WD              ;
    fdc_u8   STATUS_ERR_AHW             ;
    fdc_u8   STATUS_ERR_ALW             ;
    fdc_u8   STATUS_DRDY                ;
    fdc_u8   STATUS_CH0_UNREADCONV      ;
    fdc_u8   STATUS_CH1_UNREADCONV      ;
    fdc_u8   STATUS_CH2_UNREADCONV      ;
    fdc_u8   STATUS_CH3_UNREADCONV      ;
    
}st_status_typedef;

typedef struct _st_data_typedef
{
    fdc_u8    Flag_WD0                 ;
    fdc_u8    Flag_WD1                 ;
    fdc_u8    Flag_WD2                 ;
    fdc_u8    Flag_WD3                 ;
    
    fdc_u8    Flag_AW0                 ;
    fdc_u8    Flag_AW1                 ;
    fdc_u8    Flag_AW2                 ;
    fdc_u8    Flag_AW3                 ;
    
    fdc_u32   Data_Ch0                 ;
    fdc_u32   Data_Ch1                 ;
    fdc_u32   Data_Ch2                 ;
    fdc_u32   Data_Ch3                 ;

}st_data_typedef;

typedef struct _st_freq_cap_typedef
{
    float     freq_ch0                 ;
    float     freq_ch1                 ;
    float     freq_ch2                 ;
    float     freq_ch3                 ;    
    
    float    Cap_total_Ch0            ;
    float    Cap_total_Ch1            ;
    float    Cap_total_Ch2            ;
    float    Cap_total_Ch3            ;
    
    float    Cap_Sensor_Ch0           ;
    float    Cap_Sensor_Ch1           ;
    float    Cap_Sensor_Ch2           ;
    float    Cap_Sensor_Ch3           ;

}st_freq_cap_typedef;

/***********************   Define Parameter   **********************************/
typedef enum 
{
    Channel_Ch0 = 0x01,
    Channel_Ch1 = 0x02,
    Channel_Ch2 = 0x04,
    Channel_Ch3 = 0x08,   
    Channel_All = 0x0f,
}Channeln_typedef;

typedef enum 
{
    Channel_Mode_Single  = 0x0000,
    Channel_Mode_Mutio   = 0x8000,
}Channel_Mode_typedef;
#define IS_Channel_Mode_typedef(args)  ((args == Channel_Mode_Single ) || (args == Channel_Mode_Mutio ) )

typedef enum 
{
    /**CONFIG(0X1A)  ACTIVE_CHAN[15:14] **/ 
    Channel_Option_Ch0 = 0x0000,
    Channel_Option_Ch1 = 0x4000,
    Channel_Option_Ch2 = 0x8000,
    Channel_Option_Ch3 = 0xc000,
}Channel_Option_Single_typedef;
#define IS_Channel_Option_Single_typedef(args)  ( (args == Channel_Option_Ch0) || (args == Channel_Option_Ch1) || \
                                                  (args == Channel_Option_Ch2) || (args == Channel_Option_Ch3))

typedef enum 
{
    /**MUX_CONFIG(0X13) RR_SEQUENCE[14:13] **/
    Channel_Option_Ch0_1      = 0x0000,
    Channel_Option_Ch0_1_2    = 0x2000,
    Channel_Option_Ch0_1_2_3  = 0x4000,
}Channel_Option_Muti_typedef;
#define IS_Channel_Option_Muti_typedef(args)   ( (args == Channel_Option_Ch0_1) || (args == Channel_Option_Ch0_1_2) || \
                                                 (args == Channel_Option_Ch0_1_2_3))

typedef enum
{
    /**CONFIG(0X1A) CONFIG_REF_CLK_SRC[9]****/
    Clk_Src_Internal  = 0x0000,
    Clk_Src_Externa   = 0x0200,
}Clk_Src_typedef;
#define IS_Clk_Src_typedef(args)  ((args == Clk_Src_Internal) || (args == Clk_Src_Externa))


typedef enum
{
    /**CLOCK_DIVIDER(0X14) CLOCK_DIVIDERS_CHn[13:12]****/
    //for differential sensor configuration:Choose for sensor frequencies between 0.01MHz and 8.75MHz
    Clk_FirstMode    = 0x1000,
    //for differential sensor configuration:Choose for sensor frequencies between 5MHz and 10MHz
    //for single-ended sensor configuration:Choose for sensor frequencies between 0.01MHz and 10MHz
    Clk_SecondMode   = 0x2000,
}Clk_Fin_Sel_typedef;
#define IS_Clk_Fin_Sel_typedef(args)  ((args == Clk_FirstMode) || (args == Clk_SecondMode))

typedef enum
{
    /**CONFIG(0X1A) SENSOR_ACTIVATE_SEL[11]****/
    Current_Activation_Mode_Full      = 0x0000,
    Current_Activation_Mode_Low_Power = 0x0800,
}Current_Activation_Mode_typedef;
#define IS_Current_Activation_Mode_typedef(args)  ( (args == Current_Activation_Mode_Full) || (args == Current_Activation_Mode_Low_Power))

typedef enum
{
    /**CONFIG(0X1A)  HIGH_CURRENT_DRV[6]****/
    Current_High_Mode_Normal          = 0x0000,
    Current_High_Mode_Inereased       = 0x0040,
}Current_High_Mode_typedef;
#define IS_Current_High_Mode_typedef(args)  ( (args == Current_High_Mode_Normal) || (args == Current_High_Mode_Inereased) )

typedef enum
{
    /**MUX_CONFIG(0X1B) DEGLITCH[2:0]***********/
    Filter_Input_Deglitch_1MHz        = 0x0001,
    Filter_Input_Deglitch_3_3MHz      = 0x0004,
    Filter_Input_Deglitch_10MHz       = 0x0005,
    Filter_Input_Deglitch_33MHz       = 0x0003,
}Filter_Input_Deglitch_typedef;    
#define IS_Filter_Input_Deglitch_typedef(args)  ( (args == Filter_Input_Deglitch_1MHz) || (args == Filter_Input_Deglitch_3_3MHz) || \
                                                  (args == Filter_Input_Deglitch_10MHz) || (args == Filter_Input_Deglitch_33MHz) )

typedef enum
{
    //[13]
    Error_Config_Disable_Watchdog     = 0x0000,
    Error_Config_Enable_Watchdog      = 0x2000,
    
    //[12]
    Error_Config_Disable_Amp_High     = 0x0000,
    Error_Config_Enable_Amp_High      = 0x1000,
    
    //[11]
    Error_Config_Disable_Amp_Low      = 0x0000,
    Error_Config_Enable_Amp_Low       = 0x0800,

    //[5]
    Error_Config_Disable_Int_Wachdog  = 0x0000,
    Error_Config_Enable_Int_Wachdog   = 0x0020,

    //[0]
    Error_Config_Disable_Int_DataReady= 0x0000,
    Error_Config_Enable_Int_DataRead  = 0x0001,
    
}Error_Config_typedef;
#define IS_Error_Config_typedef(args)  ((args == Error_Config_Disable_Watchdog) || (args == Error_Config_Enable_Watchdog) || \
                                        (args == Error_Config_Disable_Amp_High) || (args == Error_Config_Enable_Amp_High) || \
                                        (args == Error_Config_Disable_Amp_Low)  || (args == Error_Config_Enable_Amp_Low)  || \
                                        (args == Error_Config_Disable_Int_Wachdog)   || (args == Error_Config_Enable_Int_Wachdog) || \
                                        (args == Error_Config_Disable_Int_DataReady) || (args == Error_Config_Enable_Int_DataRead))


typedef enum
{
    Current_0_016       =  0x0000,   
    Current_0_018       =  0x0800, 
    Current_0_021       =  0x1000, 
    Current_0_025       =  0x1800,
    Current_0_028       =  0x2000,
    Current_0_033       =  0x2800,
    Current_0_038       =  0x3000,
    Current_0_044       =  0x3800,
    Current_0_052       =  0x4000,
    Current_0_060       =  0x4800,
    Current_0_069       =  0x5000,
    Current_0_081       =  0x5800,
    Current_0_093       =  0x6000,
    Current_0_108       =  0x6800,
    Current_0_126       =  0x7000,
    Current_0_146       =  0x7800,
    Current_0_169       =  0x8000,
    Current_0_196       =  0x8800,
    Current_0_228       =  0x9000,
    Current_0_264       =  0x9800,
    Current_0_307       =  0xa000,
    Current_0_356       =  0xa800,
    Current_0_413       =  0xb000,
    Current_0_479       =  0xb800,
    Current_0_555       =  0xc000,
    Current_0_644       =  0xc800,   
    Current_0_747       =  0xd000,
    Current_0_867       =  0xd800,
    Current_1_006       =  0xe000,
    Current_1_167       =  0xe800,
    Current_1_354       =  0xf000,
    Current_1_571       =  0xf800,                                          
}Current_Driver_typedef;
#define  IS_Current_Driver_typedef(args)   ((args == Current_0_016) ||  (args == Current_0_018) || \
                                            (args == Current_0_021) ||  (args == Current_0_025) || \
                                            (args == Current_0_028) ||  (args == Current_0_033) || \
                                            (args == Current_0_038) ||  (args == Current_0_044) || \
                                            (args == Current_0_052) ||  (args == Current_0_060) || \
                                            (args == Current_0_069) ||  (args == Current_0_081) || \
                                            (args == Current_0_093) ||  (args == Current_0_108) || \
                                            (args == Current_0_126) ||  (args == Current_0_146) || \
                                            (args == Current_0_169) ||  (args == Current_0_196) || \
                                            (args == Current_0_228) ||  (args == Current_0_264) || \
                                            (args == Current_0_307) ||  (args == Current_0_356) || \
                                            (args == Current_0_413) ||  (args == Current_0_479) || \
                                            (args == Current_0_555) ||  (args == Current_0_644) || \
                                            (args == Current_0_747) ||  (args == Current_0_867) || \
                                            (args == Current_1_006) ||  (args == Current_1_167) || \
                                            (args == Current_1_354) ||  (args == Current_1_571) )
/***********************  END  Parameter  END **********************************/

extern st_fdc_init_typedef  st_fdc_init_structure;
extern st_register_typedef  st_register_structure;
extern st_data_typedef      st_data_structure;
extern st_status_typedef    st_status_structure;
extern st_freq_cap_typedef  st_freq_cap_structure;

extern int FDC_Cmd_SleepMode(fdc_u8 status);
extern int FDC_Cmd_Int(fdc_u8 status);
extern int FDC_Check_Exist(void);
extern int FDC_Reset(void);
extern int FDC_Init(st_fdc_init_typedef * fdc_init_structure,st_register_typedef *st_register_structure);
extern int FDC_Get_Data(st_data_typedef * st_data_structure,Channeln_typedef fdc_chn);
extern int FDC_Get_Status(st_status_typedef * st_status_structure);
extern int FDC_Reflash_RegisterPara(st_register_typedef * st_register_structure);
extern int FDC_Calc_Cap(st_data_typedef * st_data_structure,st_register_typedef * st_register_structure, \
                        st_freq_cap_typedef * st_freq_cap_structure);
void FDC2214_Init(void);
float FDC_Result(u8 ch);
#endif


