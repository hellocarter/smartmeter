#include "measure.h"
#include "configs.h"
#include "ADC.h"
#include "string.h"
#include "math.h"
#include "my_math.h"

#define PI 3.14159265358979323846

//������������330
#define CURRENT_SAMPLE_RES 330

//��ѹ��������510
#define VOLTAGE_SAMPLE_RES 510

//��ѹ������ѹ����150k
#define VOLTAGE_LIMIT_RES 150000

//�������������
#define CT_RATIO 2000

//��������--����ֵ,��������510
#define VOLTAGE_RATIO 0.006470588/4095

//ʵ�ʵ���<-->����ֵ:5A<-->2.5*0.33/3.3*4096����������330
#define CURRENT_RATIO 5/1024

//adc ͨ����
#define ADC_NUM 7

//����������(eg:100ms/1ms=100)
const uint16_t MEASURE_CYCLES = 5000;
const uint16_t UPDATE_CYCLES = 5000; // UPDATE_CYCLES mod MEASURE_CYCLES == 0

uint16_t current_adc[ADC_NUM];
uint16_t last_adc[ADC_NUM];

uint64_t acc_volts[3];
uint64_t acc_currents[3];

void measure_init()
{
	ADC_Config();
}

//fun(x,y)=(x-y)^2+xy
static uint64_t fun(uint16_t curr, uint16_t last)
{
	uint64_t res;
	curr = (uint32_t)curr*3096/current_adc[6];
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
	static uint16_t measure_cnt = 0;
	static uint16_t update_cnt = 0;
	char update_flag=0;
	
	//��adc���濽�������أ����ⵥ�����ڲ����任����ɼ�����
	memcpy(current_adc, ADC_Values, ADC_NUM*sizeof(uint16_t));
	for(i=0;i<3;i++)
	{
		acc_currents[i]+=fun(current_adc[i],last_adc[i]);
		acc_volts[i]+=fun(current_adc[i+3],last_adc[i+3]);
	}

	measure_cnt++;
	update_cnt++;
	
	if (measure_cnt == MEASURE_CYCLES)
	{
		
		for(i=0;i<3;i++)
		{
			//ֱ�Ӽ�������������µ���ֵ
			measured_volts[i] = 1.41421*sqrt((float)acc_volts[i]/3/MEASURE_CYCLES)*VOLTAGE_RATIO;
			
			//ֱ�Ӽ��������ֵ
			measured_currents[i] = 1.41421*sqrt((float)(acc_currents[i])/3/MEASURE_CYCLES)*CURRENT_RATIO;
			
			//С����С����ѹ��Ϊ0V
			if (measured_volts[i] < VOLT_THRESHOLD)
			{
				measured_volts[i] = 0.0;
			}
			//С����С��������Ϊ0A
			if (measured_currents[i] < CURRENT_THRESHOLD)
			{
				measured_currents[i] = 0.0;
			}
		}
		//���ˣ���ѹ�����������������ĵ���ֵ��������������������������ĵ���ֵ
		
		//��ѹ��������
		if (volt_conn_type==0)
		{
			//Ua = Ia*(Rs+Rn) + Rn*(Ia + Ib + Ic)
			struct Phasor Ua, Ub, Uc;
			struct Phasor Ia , Ib, Ic;
			struct Phasor sum, tmp;
			
			Ia.gain = measured_volts[0];
			Ia.phase = 0.0;
			
			Ib.gain = measured_volts[1];
			Ib.phase = 2*PI/3;
			
			Ic.gain = measured_volts[2];
			Ic.phase = -2*PI/3;
			
			phasor_add(&Ia, &Ib, &sum);
			phasor_add(&Ic, &sum, &sum);
			
			sum.gain = sum.gain * VOLTAGE_LIMIT_RES; //Rn*(Ia + Ib + Ic)
			
			phasor_mult_scalar(&Ia, VOLTAGE_SAMPLE_RES + VOLTAGE_LIMIT_RES, &tmp);//Ia*(Rs+Rn)
			phasor_add(&tmp, &sum, &Ua);
			
			phasor_mult_scalar(&Ib, VOLTAGE_SAMPLE_RES + VOLTAGE_LIMIT_RES, &tmp);//Ib*(Rs+Rn)
			phasor_add(&tmp, &sum, &Ub);
			
			phasor_mult_scalar(&Ic, VOLTAGE_SAMPLE_RES + VOLTAGE_LIMIT_RES, &tmp);//Ic*(Rs+Rn)
			phasor_add(&tmp, &sum, &Uc);
			
			measured_volts[0] = Ua.gain;
			measured_volts[1] = Ub.gain;
			measured_volts[2] = Uc.gain;
		}
		else
		{
			//Uab = Ia*(Rn+Rs)+(Ia+Ic)*Rn
			struct Phasor Uab, Ubc, Uca;
			struct Phasor Iab , Ibc, Ica;
			struct Phasor sum, tmp;
			
			Iab.gain = measured_volts[0];
			Iab.phase = 0.0;
			
			Ibc.gain = measured_volts[2];
			Ibc.phase = 2*PI/3;
			
			phasor_add(&Iab, &Ibc, &sum);
			sum.gain = sum.gain * VOLTAGE_LIMIT_RES;
			
			phasor_mult_scalar(&Iab, VOLTAGE_SAMPLE_RES + VOLTAGE_LIMIT_RES, &tmp);
			phasor_add(&tmp, &sum, &Uab);
			
			phasor_mult_scalar(&Ibc, VOLTAGE_SAMPLE_RES + VOLTAGE_LIMIT_RES, &tmp);
			phasor_add(&tmp, &sum, &Ubc);
			
			phasor_add(&Uab, &Ubc, &Uca);//�˴���������λ��180�ȣ�ʵ����Uac��ֻȡģֵ��Ӱ����
			
			measured_volts[0] = Uab.gain;
			measured_volts[1] = Ubc.gain;
			measured_volts[2] = Uca.gain;
			
		}
		
		//������������
		if (current_conn_type==0)
		{
			//���봦��
		}
		else
		{
			//Ib = -(Ia + Ic)
			struct Phasor Ia , Ib, Ic;
			Ia.gain = measured_currents[0];
			Ia.phase = 0.0;
			
			Ic.gain = measured_currents[2];
			Ic.phase = -2*PI/3;
			
			phasor_add(&Ia, &Ic, &Ib);
			measured_currents[1] = Ib.gain;
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
	memcpy(last_adc,current_adc,ADC_NUM*sizeof(uint16_t));	
	
	return update_flag;
}
