#include "measure.h"
#include "configs.h"
#include "ADC.h"
#include "string.h"
#include "math.h"
//#define WITH_REF

//电流采样电阻
#define CURRENT_SAMPLE_RES 330

//电流传感器变比
#define CT_RATIO 2000

//实际电流<-->采样值:5A<-->2.5*0.33/3.3*4096
#define CURRENT_RATIO 5/1024

//adc 通道数
const uint16_t ADC_NUM = 7;

//检测更新周期(eg:100ms/1ms=100)
const uint16_t MEASURE_CYCLES = 5000;
const uint16_t UPDATE_CYCLES = 5000; // UPDATE_CYCLES mod MEASURE_CYCLES == 0


static const float alpha = 1.0;

static uint16_t last_adc[ADC_NUM];
static uint64_t acc_currents[3];
static uint64_t acc_volts[3];

static const uint8_t FILTER_LEN = 8;
static float measure_queue[FILTER_LEN];
static uint8_t queue_index = 0;

void measure_init()
{
	ADC_Config();
}

//fun(x,y)=(x-y)^2+xy
static uint64_t fun(uint16_t curr, uint16_t last)
{
	uint64_t res;
	curr = (uint32_t)curr*3096/ADC_Values[6];
	res = (curr - last)*(curr - last) + 3*curr*last;
	return res;
}

/**
	* 单次采样积：1/3*(S1-S0)^2*dt+S1*S0*dt
	* AD采样0-6：current*3 voltage*3 ref*1
	*/
//测量更新返回1，否则为0
char measure_update()
{
	uint8_t i;
	static uint16_t measure_cnt = 0;
	static uint16_t update_cnt = 0;
	char update_flag=0;
	
	for(i=0;i<3;i++)
	{
		acc_currents[i]+=fun(ADC_Values[i],last_adc[i]);
		acc_volts[i]+=fun(ADC_Values[i+3],last_adc[i+3]);
	}

	measure_cnt++;
	update_cnt++;
	
	if (measure_cnt == MEASURE_CYCLES)
	{
		
		for(i=0;i<3;i++)
		{
			measured_volts[i] = (1-alpha)*measured_volts[i] + alpha*1.41421*sqrt((float)acc_volts[i+3]/3/MEASURE_CYCLES)*CURRENT_RATIO;
			measured_currents[i] = (1-alpha)*measured_currents[i] + alpha*1.41421*sqrt((float)(acc_currents[i])/3/MEASURE_CYCLES)*CURRENT_RATIO;
		}
		
		memset(acc_volts, 0, 3*sizeof(uint64_t));
		memset(acc_currents, 0, 3*sizeof(uint64_t));
		
		measure_cnt = 0;		
	}
	
	if (update_cnt == UPDATE_CYCLES)
	{
		update_flag = 1;
		update_cnt = 0;
	}
	
	//save last values
	memcpy(last_adc,ADC_Values,ADC_NUM);	
	
	return update_flag;
}
