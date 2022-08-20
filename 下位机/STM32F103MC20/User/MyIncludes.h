
#include "stm32f10x.h"
#include "stm32_eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x_flash.h"
#include "stm32f10x_tim.h"
#include "USART/usart.h"
#include "CHIPSET/chipset.h"
#include "TIMERS/times.h"
#include "USART/usart.h"
#include "GPRS/gprs.h"
#include "LED/led.h"
#include "ds18b20/delay.h"
#include "EEPROM/eeprom.h"
#include "BUZZER/buzzer.h"
#include "Analysis/analysis.h"


//#include "times.h" 
//#include "stm32f10x_lib.h"
//#include "../USART/usart.h"
//#include "../GPRS/gprs.h"
//#include "../EEPROM/eeprom.h"
//#include <stdlib.h>
//#include <string.h>
//#include <stdio.h>


typedef unsigned char     uint8_t;
typedef unsigned short    uint16_t;
typedef unsigned long     uint32t;
#define FlagStatus     bool
#define u8  uint8_t
#define u16 uint16_t
#define u32 uint32t


#define uchar 	unsigned char 
#define uint  	unsigned int 
#define uint08 	unsigned char 
#define uint16 	unsigned int 
#define int08 	char 
#define int16  	int 
#define int32  	long
#define uint32 	unsigned long
	

#define Set_Mode 0
#define Work_Mode 1

extern unsigned char   Tx_Buf[37],Rx_Buf[37];
extern unsigned char   Rx_Index,Rx_LENGTH,RecCmdOK;      //接收个数，接收到得数字域长度，接收成功标志
extern unsigned int    Min_Num;





union FLAG
{
	uint8_t all;
	struct
	{
		uint8_t bit0:1;
		uint8_t bit1:1;
		uint8_t bit2:1;
		uint8_t bit3:1;
		uint8_t bit4:1;
		uint8_t bit5:1;
		uint8_t bit6:1;
		uint8_t bit7:1;
	}Bits;
};
typedef union FLAG Flags;


typedef struct SERIALPORT
{
	uint08 RxBuf[100];
	uint16 Index;
	uint08 RxCMD;
	uint08 RxSUM;
	uint08 RecFlag;
	uint08 Time;
	uint16 Lenght;
  uint32 Coll_Count;                                                         //采样的次数；
  uint08 Count;
  uint16 Point;                                                           //指令":*****"里面“:”的位置
  uint08 LoginPackage[50];                                                     //注册包
  uint16 Code_Num;                                                        //读取的数组转换后的长度
  uint08 Storage[200];                                                    //存储数组;
}SerialPort;
extern SerialPort COMUP;


extern volatile Flags g_SysRunFlag;					//----系统各种允许状态标志定义
#define	Com1RxFinshFlag			g_SysRunFlag.Bits.bit0	//ModBus接收一包完整数据完成标志
#define	CollectFlag			        g_SysRunFlag.Bits.bit1	//采集标志
#define	UpSendFlag				g_SysRunFlag.Bits.bit2	//上传标志
#define	DebugInitFlag				g_SysRunFlag.Bits.bit3	//系统调试模式标志
#define	ReSendFlag				g_SysRunFlag.Bits.bit4	//模组重发标志
#define	LowPowerFlag				g_SysRunFlag.Bits.bit5	//低功耗标志位
#define	TestFlag				g_SysRunFlag.Bits.bit6	//调试标志
#define	WorkModelFlag				g_SysRunFlag.Bits.bit7	//工作模式标志


extern volatile Flags g_TickRunFlag;					//----系统各种定时标志定义
#define	Tick100msFlag				g_TickRunFlag.Bits.bit0	//100ms计时标志  					1：计时完成
#define	Tick1minFlag				g_TickRunFlag.Bits.bit1	//一分钟计时标志					1：计时完成
#define	TickRealyFlag				g_TickRunFlag.Bits.bit2	//继电器打开延时时间标志			        1：计时完成
#define	TickReadMeterFlag			g_TickRunFlag.Bits.bit3	//读取水表等待回应时间计时标志		                1：计时完成
#define	TickBlockDelayFlag			g_TickRunFlag.Bits.bit4	//上传数据包帧与帧之间的延时计时标志	                1：计时完成
#define	Tick1sFlag				g_TickRunFlag.Bits.bit5	//1秒钟计时标志					        1：计时完成
#define	Tick5sFlag				g_TickRunFlag.Bits.bit6	//5秒钟计时标志					        1：计时完成
#define	Tick10sFlag			  	g_TickRunFlag.Bits.bit7	//10秒钟计时标志					1：计时完成



/************************************************************************************************

************************************************************************************************/
typedef struct _TICK_TIMES
{
	uint08	_100ms;
	uint16	_1min;
	uint16	_RealyTime;
	uint16	_ReadMeter;
	uint16	_BlockDelay;
  uint16	_CollectTime;
  uint16	_UpTime;
	uint16	_1s;
	uint16	_5s;
	uint16	_10s;
}TICK_TIMES;
extern volatile TICK_TIMES Tick;

/***********************************************************************************************

***********************************************************************************************/
typedef struct DTU_Info
{
  uint08 Mode;
	uint08 PROTOCOL_TYPE;
  uint08 ST[15];			//心跳包号码
  uint08 IP[4];				//IP
  uint08 BJIP[4];			//本地IP
  uint16 PO;				//端口号
  uint08 TestMode;			//调试等级
  uint16 HT;				//心跳包间隔时间
  uint16 DATA;				//数据位
  uint16 STOP;				//停止位
  uint16 CHECK;				//校验位
  uint16 BAUD;				//波特率
  uint16 ReLinkTime;		//重连时间
  uint16 PackLength;		//数据包长度
  uint16 ERROR_COUNT;		//错误次数
  uint16 HEART_COUNT;		//心跳次数

}DTU_Info;
extern DTU_Info DTU;



//#define LED_POWER_ON 		P7OUT  &=0xF3;   				//P7.3管脚;	
//#define LED_POWER_OFF   	P7OUT  |=0x08;

#define LED1_ON			P2OUT  |=0x10;
#define LED1_OFF		P2OUT  &=0xEF;   				//P2.4管脚;	

#define LED2_ON			P2OUT  |=0x20;
#define LED2_OFF		P2OUT  &=0xdf;   				//P2.5管脚;

#define LED3_ON			P2OUT  |=0x40;
#define LED3_OFF		P2OUT  &=0xbf;   				//P2.6管脚;

#define LED4_ON			P3OUT  |=0x01;
#define LED4_OFF		P2OUT  &=0xFE;   				//P3.0管脚;

#define Main_POWER_ON 		P2OUT  |=0x08;
#define Main_POWER_OFF   	P2OUT  &=0xF7;   			//总电源控制脚P2.3管脚;	

#define AV_DIV_ON 		P2OUT  |=0x04;
#define AV_DIV_OFF   	        P2OUT  &=0xFB;   			//24电源控制脚P2.2管脚;	

#define Coll_Save_StartAdrr     0x1000;
#define  FLL_FACTOR     549                                             // FLL_FACTOR: DCO倍频系数 

#define R485_DIV_ON             P10OUT  &=0x7F;                           //10.7管脚
#define R485_DIV_OFF            P10OUT  |=0x80;                    

#define DF_DIV_ON               P9OUT  &= 0xFB;                         //9.2管脚                           
#define DF_DIV_OFF              P9OUT  |= 0x04;

#define YF_DIV_OFF              P6OUT |= 0x08;                           //6.3管脚A1采集
#define YF_DIV_ON               P6OUT &= 0xF7;                           //6.3管脚



