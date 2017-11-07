#include "math.h"

//相量
struct Phasor
{
	//幅度
	float gain;
	
	//相角，单位弧度
	float phase;
};

//复数
struct Complex
{
	//实部
	float a;
	
	//虚部
	float b;
};

//复数相加
void complex_add(struct Complex* cp1, struct Complex* cp2, struct Complex* result);

//相量转复数
void phasor_to_complex(struct Phasor* ph, struct Complex* cp);

//复数转相量
void complex_to_phasor(struct Complex* cp, struct Phasor* ph);

//相量相加
void phasor_add(struct Phasor* ph1, struct Phasor* ph2, struct Phasor* result);

//相量乘标量
void phasor_mult_scalar(struct Phasor* ph, float scalar, struct Phasor* result);

