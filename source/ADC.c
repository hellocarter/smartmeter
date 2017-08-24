#include "ADC.h"

uint16_t ADC_Values[7]; 

void GPIO_Configuration()   //PA0,PA1
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void RCC_Configuration()
{  
  RCC_ADCCLKConfig(RCC_PCLK2_Div6); 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 |RCC_APB2Periph_GPIOA, ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}


void ADC1_Configuration(void)
{
  ADC_InitTypeDef  ADC_InitStructure;
    
  ADC_DeInit(ADC1);  
  
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	
  ADC_InitStructure.ADC_ScanConvMode =ENABLE;	
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
  ADC_InitStructure.ADC_NbrOfChannel = 7;	
  ADC_Init(ADC1, &ADC_InitStructure);	
  
  /* ADC1 regular channel11 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_239Cycles5 );
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 2, ADC_SampleTime_239Cycles5 );
  ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 3, ADC_SampleTime_239Cycles5 );
  ADC_RegularChannelConfig(ADC1, ADC_Channel_3, 4, ADC_SampleTime_239Cycles5 );	
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 5, ADC_SampleTime_239Cycles5 );
  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 6, ADC_SampleTime_239Cycles5 );	
  ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 7, ADC_SampleTime_239Cycles5 );
  
  ADC_DMACmd(ADC1, ENABLE);	
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);	  
  /* Enable ADC1 reset calibaration register */   
  ADC_ResetCalibration(ADC1);	  
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));	//
  
  /* Start ADC1 calibaration */
  ADC_StartCalibration(ADC1);		//
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));		
  
}

void DMA_Configuration(void)
{
  /* ADC1  DMA1 Channel Config */
  DMA_InitTypeDef DMA_InitStructure;
  DMA_DeInit(DMA1_Channel1);   //?DMA???1?????????
  DMA_InitStructure.DMA_PeripheralBaseAddr =  (u32)&ADC1->DR;  //DMA??ADC???
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&ADC_Values;  //DMA?????
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //????????????
  DMA_InitStructure.DMA_BufferSize = 7;  //DMA???DMA?????
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //?????????
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //?????????
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //?????16?
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //?????16?
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //?????????
  DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA?? x?????? 
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA??x????????????
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //??DMA_InitStruct?????????DMA???
  
}

void ADC_Config()
{
  GPIO_Configuration();
  RCC_Configuration();
  ADC1_Configuration();
  DMA_Configuration();
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);
  DMA_Cmd(DMA1_Channel1, ENABLE);	
}
