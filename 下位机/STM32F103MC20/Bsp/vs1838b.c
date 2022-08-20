#include "stm32f10x.h"
#include "vs1838b.h"
#include "delay.h"
#include "Bsp.h"
#include "includes.h"

/*************	本地变量声明	**************/
volatile uint32_t VS1838_data=0;    // 一帧数据缓存
volatile uint8_t  VS1838_flag=0;    // 一帧数据接收完成标志
volatile uint8_t  VS1838_cnt=0;     // 按键次数

volatile uint8_t	  IR_User;			          // 用户码
volatile uint8_t	  IR_UserShit;		  		  // 用户反码
volatile uint8_t	  IR_Data;			          // 红外键原码
volatile uint8_t	  IR_DataShit;		        // 红外键反码
extern WLMeter meter;
 /**************************************************************************************
 * 描  述 : 初始化VS1838B信号引脚为外部中断输入
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void VS1838_PA1_Init(void)
{		
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | VS1838_CLK, ENABLE); 
														   
  GPIO_InitStructure.GPIO_Pin = VS1838_PIN;	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;        //浮空输入
  GPIO_Init(VS1838_PORT, &GPIO_InitStructure);		  

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);   //配置PG1管脚为外部中断线路用
	  
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;                    //配置为外部中断线1
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;           //配置为中断请求
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;       //输入线路下降沿为中断请求
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;	                    //使能中断
  EXTI_Init(&EXTI_InitStructure);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);               //NVIC_Group:先占优先级2位，从优先级2位  
  NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;              //配置为外部中断1中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     //先占优先级为1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;            //从优先级为4
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               //使能中断通道
  NVIC_Init(&NVIC_InitStructure); 
}

/**************************************************************************************
 * 描  述 : 获取高电平时间
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
uint8_t VS1838_Pulse_Time(void)
{
  uint8_t temp = 0;
  while( VS1838_DATA_IN() )
  {
    if(temp >= 250)
    {
       return temp;       // 超时溢出
    }
    temp ++;
    delay_us(20);     // 延时 20us    
  }
  return temp;
}

/**************************************************************************************
 * 描  述 : 外部中断服务程序
 * 入  参 : 无
 * 返回值 : 无
 **************************************************************************************/
void EXTI1_IRQHandler(void)
{
  uint8_t ir_pulse_time = 0;                    //脉冲宽度
  uint8_t ir_code_flag = 0;                     //红外引导码标志位，当引导码出现时，表示一帧数据开始 
  uint8_t ir_temp = 0;                          //红外数据暂存位 
  
  if(EXTI_GetITStatus(EXTI_Line1) != RESET)    //确保是否产生了EXTI Line中断
	{  
    while(1)
    {
      if( VS1838_DATA_IN()== SET )             //只测量高电平的时间 
      {      
        ir_pulse_time = VS1838_Pulse_Time();
        
        // >=5ms 不是有用信号 当出现干扰或者连发码时，会跳出while(1)循环  
        if( ir_pulse_time >= 250 )                
        {
          break;                                            //跳出while(1)循环
        }
        
        if(ir_pulse_time>=200 && ir_pulse_time<250)         //获得前导位 4ms~4.5ms
        {
          ir_code_flag = 1;
        }
        else if(ir_pulse_time>=10 && ir_pulse_time<50)      //0.56ms: 0.2ms~1ms 
        {
          ir_temp = 0;
        }
        else if(ir_pulse_time>=50 && ir_pulse_time<100)     //1.68ms：1ms~2ms
        {
          ir_temp =1 ; 
        }        
        else if( ir_pulse_time>=100 && ir_pulse_time<=200 ) //2.1ms：2ms~4ms 
        { 
          VS1838_flag = 1;               //一帧数据接收完成 
          VS1838_cnt++;                  //按键次数加1 
          break;                         //跳出while(1)循环 
        }
        
        if( ir_code_flag == 1 )          // 在第一次中断中完成
        {
          VS1838_data <<= 1;
          VS1838_data += ir_temp;
        }
      }      
    }
	}
  
	EXTI_ClearITPendingBit(EXTI_Line1);  //清除EXTI中断线路挂起位
}

 /**************************************************************************************
 * 描  述 : 读取红外键码
 * 入  参 : 无 
 * 返回值 : uint8_t
 * 备  注 : 数据帧格式：  用户码 —— 用户反码 —— 红外键码 —— 红外键反码
 **************************************************************************************/
uint8_t IR_RX_VS1838(void)
{
  IR_User =  VS1838_data >> 24;			        // 保存用户码低字节
  IR_UserShit =  (VS1838_data >>16) & 0xFF;			// 保存用户码高字节
  IR_Data =  VS1838_data >> 8;			          // 保存红外键原码
  IR_DataShit =  VS1838_data;			            // 保存红外键反码
		      
	if((IR_User==0x00) && (IR_UserShit==0xFF) )			// 判断用户码正确否
	{
		if(IR_Data== (uint8_t)~IR_DataShit)		        // 判断红外建码与反码是否接收正确
	   return 1;                                 // 正确返回
	 }
   return 0;                                  // 错误返回 
 }

 
void Get_IR_Data(void)
{
		if(VS1838_flag && IR_RX_VS1838())	 	                  //正确接收到一帧完整的红外数据
		{ 
				switch( IR_Data )
				{
						case 24:
								VS1838_flag=0;                                    //清零一帧数据接收完成标志
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
								VS1838_flag=0;                                    //清零一帧数据接收完成标志
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
								VS1838_flag=0;                                    //清零一帧数据接收完成标志
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
								VS1838_flag=0;                                    //清零一帧数据接收完成标志
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
								VS1838_flag=0;                                    //清零一帧数据接收完成标志
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
								printf("\r\n 键码十进制：%d \r\n",IR_Data);        //上传红外键码十进制				
								break;
				}
		}
}
