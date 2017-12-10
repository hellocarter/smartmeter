#include "openio.h"
#include "configs.h"

//开出J1-PB8,J2-PB9
//开入CF1-PB10,CF2-PB11
void openio_init()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  

	//J1-PB8,J2-PB9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;                         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;                   
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);                          

	//CF1-PB10,CF2-PB11
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;                      
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;          
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	openio_output();
}

//normal high, in-1
void openio_input()
{
	uint16_t input;
	input = GPIO_ReadInputData(GPIOB);
	io_in1 = (input&GPIO_Pin_10)?0:1;
	io_in2 = (input&GPIO_Pin_11)?0:1;
}

uint8_t check_alarm1(int8_t type, int16_t value)
{

	if (type>=4)
	{
		if(measured_volts[type-4] > value*450/100)
		{
			return 1;
		}
	}
	else
	{
		if(measured_volts[type-1] < value*450/100)
		{
			return 1;
		}
	}
	return 0;
}

uint8_t check_alarm2(int8_t type, int16_t value)
{

	if (type>=4)
	{
		if(measured_currents[type-4] > value*5/100)
		{
			return 1;
		}
	}
	else
	{
		if(measured_currents[type-1] < value*5/100)
		{
			return 1;
		}
	}
	return 0;
}
//pin low, relay on, 1-on
void openio_output()
{
	if(alarm_type1)
	{
		io_out1 = check_alarm1(alarm_type1,alarm_value1);
	}
	
	if (alarm_type2)
	{
		io_out2 = check_alarm2(alarm_type2,alarm_value2);
	}
	
	if (io_out1)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_8);
	}
	else
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	}
	
	if (io_out2)
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_9);
	}
	else
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_9);
	}
}
