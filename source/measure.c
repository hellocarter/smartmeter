#include "measure.h"
#include "ADC.h"
#include "string.h"
#include "math.h"
//#define WITH_REF

//������������
#define CURRENT_SAMPLE_RES 330

//�������������
#define CT_RATIO 2000
//ʵ�ʵ���<-->����ֵ:5A<-->2.5*0.33/3.3*4096
#define CURRENT_RATIO 5/1024

//adc ͨ����
const uint16_t ADC_NUM = 7;

//����ʱ��
const float dt = 0.001;

//����������(eg:500ms/1ms=500)
const uint16_t MEASURE_CYCLES = 500;
static uint16_t cnt=0;

static uint16_t last_adc[ADC_NUM];
static uint64_t acc_currents[3];
static uint64_t acc_volts[3];

float convert_currents[3];
float convert_volts[3];

void measure_init()
{
	ADC_Config();
}

//fun(x,y)=(x-y)^2+xy
static uint64_t fun(uint16_t curr, uint16_t last)
{
	uint64_t res;
	res = (curr - last)*(curr - last) + 3*curr*last;
	return res;
}

/**
	* ���β�������1/3*(S1-S0)^2*dt+S1*S0*dt
	* AD����0-6��current*3 voltage*3 ref*1
	*/
//�������·���1������Ϊ0
char measure_update()
{
	uint8_t i;
	char update_flag=0;
	for(i=0;i<3;i++)
	{
		acc_currents[i]+=fun(ADC_Values[i],last_adc[i]);
	}
	for(i=0;i<3;i++)
	{
		acc_volts[i]+=fun(ADC_Values[i+3],last_adc[i+3]);
	}
	
	cnt++;
	if(cnt == MEASURE_CYCLES)
	{
		uint8_t i;
		for(i=0;i<3;i++)
		{
			#ifndef WITH_REF
			  convert_volts[i] = 1.41421*sqrt((float)acc_volts[i+3]/3/MEASURE_CYCLES)*CURRENT_RATIO;
			  convert_currents[i] = 1.41421*sqrt((float)acc_currents[i]/3/MEASURE_CYCLES)*CURRENT_RATIO;
			#elif
			  convert_volts[i] = sqrt((float)acc_volts[i+3]/3/MEASURE_CYCLES)*2.5/ADC_Values[6];
			  convert_currents[i] = sqrt((float)acc_currents[i]/3/MEASURE_CYCLES)*2.5/ADC_Values[6];
			#endif
		}
		for(i=0;i<3;i++)
		{
			acc_volts[i]=0;
			acc_currents[i]=0;
		}
		cnt = 0;
		update_flag = 1;
	}
	//save last values
	memcpy(last_adc,ADC_Values,ADC_NUM);	
	return update_flag;
}
