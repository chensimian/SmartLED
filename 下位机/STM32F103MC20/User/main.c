#include "stm32f10x.h"
#include "includes.h"
#include "bmp.h"
TICK_TIMES Tick;
RbufInfo buf_uart1;
RbufInfo buf_uart2;
extern WLMeter meter;
extern uint8_t  VS1838_flag;              // 一帧数据接收完成标志

extern uint8_t	  IR_UserH;			          // 用户码(地址)高字节
extern uint8_t	  IR_UserL;		  		      // 用户码(地址)低字节
extern uint8_t	  IR_Data;			  	      // 红外键原码	 
extern uint8_t		IR_DataShit;		        // 红外键反码 

char wifi[20] = {"CMCC-GHPN"};            //wifi账号
char password[20] = {"y7n2qrya"};			  	//wifi密码

int main(void)
{  
		char text[20],text0[20],text1[20],text2[20];

		u16 hc_num1,hc_num2;
		u16 adc_num1,adc_num2;
		u8 adc;
		u16 auto_pwm;
    delay_init(72);                         
   	NVIC_Configuration();
	
		KEY_Init();
    TIM2_init();																	//定时器2初始化
		Usart1_Init(9600);														//串口1初始化
		Usart2_Init(9600);														//串口2初始化
		VS1838_PA1_Init();														//红外接收管初始化
		Sensor_hc_Init();															//人体传感器初始化
		Adc_Init();																		//光敏电阻初始化
		TIM1_PWM_Init(999,0);													//PWM初始化 PWM频率=72000/(999+1)=72Khz 
		TIM_SetCompare1(TIM1,999);										//
		OLED_GPIO_Init();															//OLED引脚初始化
    OLED_Init();																	//OLED初始化
		OLED_CLS();																		//清屏

		OLED_P8x16Str(0,0,"   Smart LED ");						//第一行 -- 8x16的显示单元显示ASCII码
		OLED_P8x16Str(0,2,"   Waiting... ");
		OLED_P8x16Str(0,4,"");
		OLED_P8x16Str(0,6,"");
		
		ESP8266_SERVER();															//ESP8266初始化
		
		OLED_CLS();

		OLED_P8x16Str(0,0,"   Smart LED ");						//第一行 -- 8x16的显示单元显示ASCII码
		OLED_P8x16Str(0,2,"");
		OLED_P8x16Str(0,4,"");
		OLED_P8x16Str(0,6,"");

		
		meter.adc_value = 5;													//光敏电阻采样阈值
		meter.hc_flag = 0;
		meter.led_flag = 0;
		meter.auto_flag = 1;
		meter.led_pwm = 500;
		meter.led_on_off_flag = 0;
		Tick._30s = 0;
		
		
		
		
		while(1)
    {		
				delay_ms(10);
				if(PAin(4)==1)                      		//人体检测
				{
						hc_num1++;
						hc_num2 = 0;
						if(hc_num1>3) 
						{
								hc_num1 = 0;
								meter.hc_flag = 1;
						}
				}
				else
				{
						hc_num1 = 0;
					  hc_num2++;
						if(hc_num2>300)
						{
								hc_num2 = 0;
								meter.hc_flag = 0;
						}
				}
				Get_IR_Data();												//采集红外码值
				adc = Get_Adc_Average(0,5);
				if(adc<meter.adc_value)
				{
						adc_num1++;
						adc_num2 = 0;
						if(adc_num1>30)
						{
								meter.led_flag = 1;
						}
				}
				else if(adc>(meter.adc_value+20))       //光照超过25 白天
				{
						
						adc_num2++;
						adc_num1 = 0;
						if(adc_num2>30)
						{
								meter.led_flag = 0;
						}
				}
				
				if(meter.auto_flag)                   //自动模式
				{
						if(meter.led_flag)
						{
								if(meter.hc_flag)
								{
										Tick._30s = 0;
										auto_pwm = adc/3;
										if(auto_pwm>9)
										{
												auto_pwm = 9;
										}
										TIM_SetCompare1(TIM1,auto_pwm*100);
										meter.led_pwm = 999-auto_pwm*100;
										meter.led_on_off_flag = 1;
								}
								else
								{
										if(Tick._30s>6000)
										{
												Tick._30s = 0;
												TIM_SetCompare1(TIM1,999);
												meter.led_on_off_flag = 0;
										}
								}
						}
						else
						{
								TIM_SetCompare1(TIM1,999);
								meter.led_on_off_flag = 0;
						}
				}
				else
				{
						TIM_SetCompare1(TIM1,999-meter.led_pwm);
						meter.led_on_off_flag = 1;
				}
				
				
				if(buf_uart1.flay)                            //读取WiFi数据
				{
						buf_uart1.flay = RESET;
						//Get_Clinet();
						ClearRebuf();
				}
				
				
				
				
				if(Read_KEY0_Status()==0)                    //减少按键
				{
						delay_ms(20);
						if(Read_KEY0_Status()==0)
						{
								if(meter.led_pwm>=100)
								{
										meter.led_pwm=meter.led_pwm-100;
								}
								else
								{
										meter.led_pwm = 0;
								}
							
						}
				}
				
				if(Read_KEY1_Status()==0)                   //增加按键
				{
						delay_ms(20);
						if(Read_KEY1_Status()==0)
						{
								if(meter.led_pwm<900)
								{
										meter.led_pwm = meter.led_pwm+100;
								}
								else
								{
										meter.led_pwm = 999;
								}
						}
				}
				

				if(meter.auto_flag)																		//更新OLED显示
				{
						sprintf(text0, "AUTO: ON  ");
				}
				else
				{
						sprintf(text0, "AUTO: OFF  ");
				}
				sprintf(text1, "ADC: %d ",adc);

				sprintf(text2, "PWM: %d  ",meter.led_pwm);
				

				OLED_P8x16Str(0,0,"   Smart LED ");//第一行 -- 8x16的显示单元显示ASCII码
				OLED_P8x16Str(0,2,text0);
				OLED_P8x16Str(0,4,text1);
				OLED_P8x16Str(0,6,text2);
				
				if(Tick._5stime>10000)
				{
						Tick._5stime = 0;
						ESP8266_SendCmd("AT+SUB=sldev/123456,0\r\n","ok");  
						sprintf(text, "U%d%d%d%0.3d%0.3d",meter.auto_flag,meter.led_on_off_flag,meter.hc_flag,adc,meter.led_pwm);
						Send_DATA(text);	
				}
		}
}
