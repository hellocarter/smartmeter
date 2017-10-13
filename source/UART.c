#include "UART.h"

void RCC_Config()
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA , ENABLE);  

	//PA9-TX
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;                         
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                   
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);                          
  
	//PA10-RX
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;                      
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;          
  GPIO_Init(GPIOA, &GPIO_InitStructure);                         

	//PA8-R/X
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;                      
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;          
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
}

void NVIC_Configuration()
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable the USARTy Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}

void Uart_set_tx()
{
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
}

void Uart_set_rx()
{
	GPIO_ResetBits(GPIOA,GPIO_Pin_8);
}

void Uart_Init(const uint32_t baud_rate)
{
  USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef USART_ClockInitStructure;
  
  RCC_Config();
  NVIC_Configuration();
  
  USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;			
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;				
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;				
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;		
	/* Configure the USART1 synchronous paramters */
	USART_ClockInit(USART1, &USART_ClockInitStructure);					
				
  USART_InitStructure.USART_BaudRate = baud_rate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_Init(USART1, &USART_InitStructure); 
  USART_ClearFlag(USART1, USART_IT_RXNE);
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  USART_Cmd(USART1, ENABLE);
  
	Uart_set_rx();

}

void Uart_Write(uint8_t *buf,uint8_t size)
{
	int i=0;
	Uart_set_tx();
	for(i=0;i<size;i++)
	{
		while(USART_GetFlagStatus(USART1, USART_FLAG_TC) !=SET);
		USART_SendData(USART1,buf[i]);
	}
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) !=SET);
	Uart_set_rx();
}


