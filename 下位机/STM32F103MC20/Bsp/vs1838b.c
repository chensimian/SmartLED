#include "stm32f10x.h"
#include "vs1838b.h"
#include "delay.h"
#include "Bsp.h"
#include "includes.h"

/*************	���ر�������	**************/
volatile uint32_t VS1838_data=0;    // һ֡���ݻ���
volatile uint8_t  VS1838_flag=0;    // һ֡���ݽ�����ɱ�־
volatile uint8_t  VS1838_cnt=0;     // ��������

volatile uint8_t	  IR_User;			          // �û���
volatile uint8_t	  IR_UserShit;		  		  // �û�����
volatile uint8_t	  IR_Data;			          // �����ԭ��
volatile uint8_t	  IR_DataShit;		        // ���������
extern WLMeter meter;
 /**************************************************************************************
 * ��  �� : ��ʼ��VS1838B�ź�����Ϊ�ⲿ�ж�����
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void VS1838_PA1_Init(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | VS1838_CLK, ENABLE); 
														   
  GPIO_InitStructure.GPIO_Pin = VS1838_PIN;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;        //��������
  GPIO_Init(VS1838_PORT, &GPIO_InitStructure);		  

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);   //����PG1�ܽ�Ϊ�ⲿ�ж���·��
	  
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;                    //����Ϊ�ⲿ�ж���1
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;           //����Ϊ�ж�����
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;       //������·�½���Ϊ�ж�����
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;	                    //ʹ���ж�
  EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //NVIC_Group:��ռ���ȼ�2λ�������ȼ�2λ  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;              //����Ϊ�ⲿ�ж�1�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     //��ռ���ȼ�Ϊ1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;            //�����ȼ�Ϊ4
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //ʹ���ж�ͨ��
  NVIC_Init(&NVIC_InitStructure); 
}

/**************************************************************************************
 * ��  �� : ��ȡ�ߵ�ƽʱ��
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
uint8_t VS1838_Pulse_Time(void)
{
  uint8_t temp = 0;
  while( VS1838_DATA_IN() )
  {
    if(temp >= 250)
    {
       return temp;       // ��ʱ���
    }
    temp ++;
    delay_us(20);     // ��ʱ 20us    
  }
  return temp;
}

/**************************************************************************************
 * ��  �� : �ⲿ�жϷ������
 * ��  �� : ��
 * ����ֵ : ��
 **************************************************************************************/
void EXTI1_IRQHandler(void)
{
  uint8_t ir_pulse_time = 0;                    //������
  uint8_t ir_code_flag = 0;                     //�����������־λ�������������ʱ����ʾһ֡���ݿ�ʼ 
  uint8_t ir_temp = 0;                          //���������ݴ�λ 
  
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)    //ȷ���Ƿ������EXTI Line�ж�
	{  
    while(1)
    {
      if( VS1838_DATA_IN()== SET )             //ֻ�����ߵ�ƽ��ʱ�� 
      {      
        ir_pulse_time = VS1838_Pulse_Time();
        
        // >=5ms ���������ź� �����ָ��Ż���������ʱ��������while(1)ѭ��  
        if( ir_pulse_time >= 250 )                
        {
          break;                                            //����while(1)ѭ��
        }
        
        if(ir_pulse_time>=200 && ir_pulse_time<250)         //���ǰ��λ 4ms~4.5ms
        {
          ir_code_flag = 1;
        }
        else if(ir_pulse_time>=10 && ir_pulse_time<50)      //0.56ms: 0.2ms~1ms 
        {
          ir_temp = 0;
        }
        else if(ir_pulse_time>=50 && ir_pulse_time<100)     //1.68ms��1ms~2ms
        {
          ir_temp =1 ; 
        }        
        else if( ir_pulse_time>=100 && ir_pulse_time<=200 ) //2.1ms��2ms~4ms 
        { 
          VS1838_flag = 1;               //һ֡���ݽ������ 
          VS1838_cnt++;                  //����������1 
          break;                         //����while(1)ѭ�� 
        }
        
        if( ir_code_flag == 1 )          // �ڵ�һ���ж������
        {
          VS1838_data <<= 1;
          VS1838_data += ir_temp;
        }
      }      
    }
	}
  
	EXTI_ClearITPendingBit(EXTI_Line1);  //���EXTI�ж���·����λ
}

 /**************************************************************************************
 * ��  �� : ��ȡ�������
 * ��  �� : �� 
 * ����ֵ : uint8_t
 * ��  ע : ����֡��ʽ��  �û��� ���� �û����� ���� ������� ���� ���������
 **************************************************************************************/
uint8_t IR_RX_VS1838(void)
{
  IR_User =  VS1838_data >> 24;			        // �����û�����ֽ�
  IR_UserShit =  (VS1838_data >>16) & 0xFF;			// �����û�����ֽ�
  IR_Data =  VS1838_data >> 8;			          // ��������ԭ��
  IR_DataShit =  VS1838_data;			            // ������������
		      
	if((IR_User==0x00) && (IR_UserShit==0xFF) )			// �ж��û�����ȷ��
	{
		if(IR_Data== (uint8_t)~IR_DataShit)		        // �жϺ��⽨���뷴���Ƿ������ȷ
	   return 1;                                 // ��ȷ����
	 }
   return 0;                                  // ���󷵻� 
 }

 
void Get_IR_Data(void)
{
		if(VS1838_flag && IR_RX_VS1838())	 	                  //��ȷ���յ�һ֡�����ĺ�������
		{ 
				switch( IR_Data )
				{
						case 24:
								VS1838_flag=0;                                    //����һ֡���ݽ�����ɱ�־
								if(meter.led_pwm<900)
								{
										meter.led_pwm = meter.led_pwm+100;
								}
								else
								{
										meter.led_pwm = 999;
								}
								break;

						case 16:
								VS1838_flag=0;                                    //����һ֡���ݽ�����ɱ�־
								if(meter.led_pwm>=100)
								{
										meter.led_pwm=meter.led_pwm-100;
								}
								else
								{
										meter.led_pwm = 0;
								}
								break;
								
						case 56:
								VS1838_flag=0;                                    //����һ֡���ݽ�����ɱ�־
								if(meter.auto_flag==0)
								{
										meter.auto_flag = 1;
								}
								else
								{
										meter.auto_flag = 0;
								}
								break;
								
						case 90: 
								VS1838_flag=0;                                    //����һ֡���ݽ�����ɱ�־
								if(meter.led_pwm<900)
								{
										meter.led_pwm = meter.led_pwm+100;
								}
								else
								{
										meter.led_pwm = 999;
								}
								break;
								
						case 74: 
								VS1838_flag=0;                                    //����һ֡���ݽ�����ɱ�־
								if(meter.led_pwm>=100)
								{
										meter.led_pwm=meter.led_pwm-100;
								}
								else
								{
										meter.led_pwm = 0;
								}	
								break;
								
						default:   
								printf("\r\n ����ʮ���ƣ�%d \r\n",IR_Data);        //�ϴ��������ʮ����				
								break;
				}
		}
}
