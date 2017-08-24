#include "measure.h"
#include "ADC.h"

void measure_init()
{
	ADC_Config();
}

const uint16_t* measure_currents()
{
	return ADC_Values;
}

const uint16_t* measure_voltages()
{
	return ADC_Values+3;
}

const uint16_t* measure_ref()
{
	return ADC_Values+6;
}
