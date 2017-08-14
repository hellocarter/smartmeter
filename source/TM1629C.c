/**
 *  TM1629C驱动程序
 */
 
#include "TM1629C.h"

#define DIO GPIO_Pin_13
#define CLK GPIO_Pin_14
#define STB  GPIO_Pin_15

static const int WAITTIME = 1;

void delay_us(vu32 nCount)
{
	vu32 i;
	while(nCount--){
		i=8;
		while(i--);
	}
}
void Ports_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd  ( RCC_APB2Periph_GPIOC, ENABLE);   
  GPIO_InitStructure.GPIO_Pin = CLK|STB;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 	
	
}
void DIO_Input(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	
  GPIO_InitStructure.GPIO_Pin = DIO;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOC, &GPIO_InitStructure);   	
}
void DIO_Output(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
  GPIO_InitStructure.GPIO_Pin = DIO;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  	
}
void DIO_Set(void)
{
	GPIOC->ODR|=DIO;
}
void DIO_Reset(void)
{
	GPIOC->ODR&=~DIO;
}
uint8_t DIO_Read(void)
{
	return GPIO_ReadInputDataBit(GPIOC,DIO);
}
void CLK_Set(void)
{
	GPIOC->ODR|=CLK;
}
void CLK_Reset(void)
{
	GPIOC->ODR&=~CLK;
}
void STB_Set(void)
{
	GPIOC->ODR|=STB;
}
void STB_Reset(void)
{
	GPIOC->ODR&=~STB;
}
static void TM1629C_Write(uint8_t data)
{
	 uint8_t i;
	 DIO_Output();
	 CLK_Set();
	 delay_us(WAITTIME);
   for(i=0;i<8;i++)
   {
      CLK_Reset();
      if(data&0x01)
			{
				DIO_Set();
			}
			else
			{
				DIO_Reset();
			}
      CLK_Set();
      data>>=1;
   }
	 delay_us(WAITTIME);
}
static void Write_CMD(uint8_t cmd)      
{
   STB_Reset();
   TM1629C_Write(cmd);
   STB_Set();
}
void TM1629C_Clear(void)
{
   unsigned char i;
	 Write_CMD(0x40); 
   STB_Reset();
	 
   TM1629C_Write(0xc0);   
   for(i=0;i<16;i++)
   TM1629C_Write(0x00);     
   STB_Set();
}
static uint8_t TM1629C_ReadByte()
{
	int i;
	uint8_t ch=0;
	STB_Reset();
	DIO_Input();
	
	for(i=0;i<8;i++)
	{
		CLK_Reset();
		if(DIO_Read())
		{
			ch=(ch<<1)|0x01;
		}
		else
		{
			ch=ch<<1;
		}
		CLK_Set();
	}
	return ch;
}
void TM1629C_ReadKey(unsigned char *buf)
{
  int i;
	STB_Reset();
	TM1629C_Write(0x42);
	DIO_Input();
	CLK_Set();
	for(i=0;i<4;i++){
		buf[i]=TM1629C_ReadByte();
	}
	STB_Set();
}
void TM1629C_Refresh(uint8_t *buf)
{
	unsigned char i;
	Write_CMD(0x40); 
   STB_Reset();
	 
   TM1629C_Write(0xc0);   
   for(i=0;i<16;i++)
	 {
		TM1629C_Write(*buf);
		buf++;
	 }
        
   STB_Set();
}
void TM1629C_Init(void)
{
	Ports_Config();
  CLK_Set();          
  STB_Set();            
  DIO_Set();
	
	//打开显示，设置亮度
  Write_CMD(0x8f);  
  TM1629C_Clear();
}