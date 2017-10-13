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

//pin low, relay on, 1-on
void openio_output()
{
	if (io_out1)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_8);
	}
	else
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_8);
	}
	
	if (io_out2)
	{
		GPIO_ResetBits(GPIOB,GPIO_Pin_9);
	}
	else
	{
		GPIO_SetBits(GPIOB,GPIO_Pin_9);
	}
}
