#include "stm32f10x.h"
#include "includes.h"
#include "bmp.h"
TICK_TIMES Tick;
RbufInfo buf_uart1;
RbufInfo buf_uart2;
extern WLMeter meter;
extern uint8_t  VS1838_flag;              // һ֡���ݽ�����ɱ�־

extern uint8_t	  IR_UserH;			          // �û���(��ַ)���ֽ�
extern uint8_t	  IR_UserL;		  		      // �û���(��ַ)���ֽ�
extern uint8_t	  IR_Data;			  	      // �����ԭ��	 
extern uint8_t		IR_DataShit;		        // ��������� 

char wifi[20] = {"CMCC-GHPN"};            //wifi�˺�
char password[20] = {"y7n2qrya"};			  	//wifi����

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
    TIM2_init();																	//��ʱ��2��ʼ��
		Usart1_Init(9600);														//����1��ʼ��
		Usart2_Init(9600);														//����2��ʼ��
		VS1838_PA1_Init();														//������չܳ�ʼ��
		Sensor_hc_Init();															//���崫������ʼ��
		Adc_Init();																		//���������ʼ��
		TIM1_PWM_Init(999,0);													//PWM��ʼ�� PWMƵ��=72000/(999+1)=72Khz 
		TIM_SetCompare1(TIM1,999);										//
		OLED_GPIO_Init();															//OLED���ų�ʼ��
    OLED_Init();																	//OLED��ʼ��
		OLED_CLS();																		//����

		OLED_P8x16Str(0,0,"   Smart LED ");						//��һ�� -- 8x16����ʾ��Ԫ��ʾASCII��
		OLED_P8x16Str(0,2,"   Waiting... ");
		OLED_P8x16Str(0,4,"");
		OLED_P8x16Str(0,6,"");
		
		ESP8266_SERVER();															//ESP8266��ʼ��
		
		OLED_CLS();

		OLED_P8x16Str(0,0,"   Smart LED ");						//��һ�� -- 8x16����ʾ��Ԫ��ʾASCII��
		OLED_P8x16Str(0,2,"");
		OLED_P8x16Str(0,4,"");
		OLED_P8x16Str(0,6,"");

		
		meter.adc_value = 5;													//�������������ֵ
		meter.hc_flag = 0;
		meter.led_flag = 0;
		meter.auto_flag = 1;
		meter.led_pwm = 500;
		meter.led_on_off_flag = 0;
		Tick._30s = 0;
		
		
		
		
		while(1)
    {		
				delay_ms(10);
				if(PAin(4)==1)                      		//������
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
				Get_IR_Data();												//�ɼ�������ֵ
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
				else if(adc>(meter.adc_value+20))       //���ճ���25 ����
				{
						
						adc_num2++;
						adc_num1 = 0;
						if(adc_num2>30)
						{
								meter.led_flag = 0;
						}
				}
				
				if(meter.auto_flag)                   //�Զ�ģʽ
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
				
				
				if(buf_uart1.flay)                            //��ȡWiFi����
				{
						buf_uart1.flay = RESET;
						//Get_Clinet();
						ClearRebuf();
				}
				
				
				
				
				if(Read_KEY0_Status()==0)                    //���ٰ���
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
				
				if(Read_KEY1_Status()==0)                   //���Ӱ���
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
				

				if(meter.auto_flag)																		//����OLED��ʾ
				{
						sprintf(text0, "AUTO: ON  ");
				}
				else
				{
						sprintf(text0, "AUTO: OFF  ");
				}
				sprintf(text1, "ADC: %d ",adc);

				sprintf(text2, "PWM: %d  ",meter.led_pwm);
				

				OLED_P8x16Str(0,0,"   Smart LED ");//��һ�� -- 8x16����ʾ��Ԫ��ʾASCII��
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
