
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
extern unsigned char   Rx_Index,Rx_LENGTH,RecCmdOK;      //���ո��������յ��������򳤶ȣ����ճɹ���־
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
  uint32 Coll_Count;                                                         //�����Ĵ�����
  uint08 Count;
  uint16 Point;                                                           //ָ��":*****"���桰:����λ��
  uint08 LoginPackage[50];                                                     //ע���
  uint16 Code_Num;                                                        //��ȡ������ת����ĳ���
  uint08 Storage[200];                                                    //�洢����;
}SerialPort;
extern SerialPort COMUP;


extern volatile Flags g_SysRunFlag;					//----ϵͳ��������״̬��־����
#define	Com1RxFinshFlag			g_SysRunFlag.Bits.bit0	//ModBus����һ������������ɱ�־
#define	CollectFlag			        g_SysRunFlag.Bits.bit1	//�ɼ���־
#define	UpSendFlag				g_SysRunFlag.Bits.bit2	//�ϴ���־
#define	DebugInitFlag				g_SysRunFlag.Bits.bit3	//ϵͳ����ģʽ��־
#define	ReSendFlag				g_SysRunFlag.Bits.bit4	//ģ���ط���־
#define	LowPowerFlag				g_SysRunFlag.Bits.bit5	//�͹��ı�־λ
#define	TestFlag				g_SysRunFlag.Bits.bit6	//���Ա�־
#define	WorkModelFlag				g_SysRunFlag.Bits.bit7	//����ģʽ��־


extern volatile Flags g_TickRunFlag;					//----ϵͳ���ֶ�ʱ��־����
#define	Tick100msFlag				g_TickRunFlag.Bits.bit0	//100ms��ʱ��־  					1����ʱ���
#define	Tick1minFlag				g_TickRunFlag.Bits.bit1	//һ���Ӽ�ʱ��־					1����ʱ���
#define	TickRealyFlag				g_TickRunFlag.Bits.bit2	//�̵�������ʱʱ���־			        1����ʱ���
#define	TickReadMeterFlag			g_TickRunFlag.Bits.bit3	//��ȡˮ��ȴ���Ӧʱ���ʱ��־		                1����ʱ���
#define	TickBlockDelayFlag			g_TickRunFlag.Bits.bit4	//�ϴ����ݰ�֡��֮֡�����ʱ��ʱ��־	                1����ʱ���
#define	Tick1sFlag				g_TickRunFlag.Bits.bit5	//1���Ӽ�ʱ��־					        1����ʱ���
#define	Tick5sFlag				g_TickRunFlag.Bits.bit6	//5���Ӽ�ʱ��־					        1����ʱ���
#define	Tick10sFlag			  	g_TickRunFlag.Bits.bit7	//10���Ӽ�ʱ��־					1����ʱ���



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
  uint08 ST[15];			//����������
  uint08 IP[4];				//IP
  uint08 BJIP[4];			//����IP
  uint16 PO;				//�˿ں�
  uint08 TestMode;			//���Եȼ�
  uint16 HT;				//���������ʱ��
  uint16 DATA;				//����λ
  uint16 STOP;				//ֹͣλ
  uint16 CHECK;				//У��λ
  uint16 BAUD;				//������
  uint16 ReLinkTime;		//����ʱ��
  uint16 PackLength;		//���ݰ�����
  uint16 ERROR_COUNT;		//�������
  uint16 HEART_COUNT;		//��������

}DTU_Info;
extern DTU_Info DTU;



//#define LED_POWER_ON 		P7OUT  &=0xF3;   				//P7.3�ܽ�;	
//#define LED_POWER_OFF   	P7OUT  |=0x08;

#define LED1_ON			P2OUT  |=0x10;
#define LED1_OFF		P2OUT  &=0xEF;   				//P2.4�ܽ�;	

#define LED2_ON			P2OUT  |=0x20;
#define LED2_OFF		P2OUT  &=0xdf;   				//P2.5�ܽ�;

#define LED3_ON			P2OUT  |=0x40;
#define LED3_OFF		P2OUT  &=0xbf;   				//P2.6�ܽ�;

#define LED4_ON			P3OUT  |=0x01;
#define LED4_OFF		P2OUT  &=0xFE;   				//P3.0�ܽ�;

#define Main_POWER_ON 		P2OUT  |=0x08;
#define Main_POWER_OFF   	P2OUT  &=0xF7;   			//�ܵ�Դ���ƽ�P2.3�ܽ�;	

#define AV_DIV_ON 		P2OUT  |=0x04;
#define AV_DIV_OFF   	        P2OUT  &=0xFB;   			//24��Դ���ƽ�P2.2�ܽ�;	

#define Coll_Save_StartAdrr     0x1000;
#define  FLL_FACTOR     549                                             // FLL_FACTOR: DCO��Ƶϵ�� 

#define R485_DIV_ON             P10OUT  &=0x7F;                           //10.7�ܽ�
#define R485_DIV_OFF            P10OUT  |=0x80;                    

#define DF_DIV_ON               P9OUT  &= 0xFB;                         //9.2�ܽ�                           
#define DF_DIV_OFF              P9OUT  |= 0x04;

#define YF_DIV_OFF              P6OUT |= 0x08;                           //6.3�ܽ�A1�ɼ�
#define YF_DIV_ON               P6OUT &= 0xF7;                           //6.3�ܽ�



