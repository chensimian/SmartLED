
#ifndef __INCLUDES_H
#define __INCLUDES_H

#include "stm32f10x.h"
#include "stm32f10x_it.h"
#include "bsp.h"
#include "math.h"
#include "delay.h"
#include <stdlib.h>
#include  <stdio.h>
#include  <string.h>
#include "led.h"
#include "key.h"
#include "sys.h"
#include "oled.h"
#include "rtc.h"
#include "esp8266.h"
#include "pwm.h"
#include "sensor.h"
#include "vs1838b.h"
#include "adc.h"

#define SIZE sizeof(TEXT_Buffer)	 	//数组长度
#define FLASH_SAVE_ADDR  0X0800FC00 	//设置FLASH 保存地址(第63页）0X0800FC00-0X0800FFFF




typedef struct
{
  FlagStatus flay;
	FlagStatus recFlag;
	u8    time;
  u16   cnt;
  u8    Rebuf[200];
}RbufInfo;
extern RbufInfo buf_uart1;
extern RbufInfo buf_uart2;

typedef struct
{
	u8 auto_flag;
  u8 led_flag;
	u8 led_on_off_flag;
	u8 hc_flag;
	u8 adc_value;
	u16 led_pwm;
	//char wifi[20];
	//char password[20];
}WLMeter;
extern WLMeter meter;


typedef struct _TICK_TIMES
{
    unsigned int    _1ms;
		unsigned int    _30s;
		unsigned int    _5stime;

    FlagStatus   flag;
		FlagStatus   _1s_flag;
		FlagStatus   _30s_flag;
		FlagStatus   refreshtime_flag;

}TICK_TIMES;
extern TICK_TIMES Tick;


/////////////////////////////////////////////////////////////////////
//函数原型
/////////////////////////////////////////////////////////////////////
 

#define TRUE 1
#define FALSE 0

#endif
