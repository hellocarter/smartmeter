#include "my_math.h"

#define PI 3.14159265358979323846

//复数相加
void complex_add(struct Complex* cp1, struct Complex* cp2, struct Complex* result)
{
	result->a = cp1->a + cp2->a;
	result->b = cp1->b + cp2->b;
}

//相量转复数
void phasor_to_complex(struct Phasor* ph, struct Complex* cp)
{
	cp->a = ph->gain*cos(ph->phase);
	cp->b = ph->gain*sin(ph->phase);
}

//复数转相量
void complex_to_phasor(struct Complex* cp, struct Phasor* ph)
{
	ph->gain = sqrt((cp->a)*(cp->a) + (cp->b)*(cp->b));
	ph->phase = atan(cp->b/cp->a);
	if (cp->a < 0)
	{
		if (cp->b > 0)
		{
			ph->phase = ph->phase + PI;
		}
		else
		{
			ph->phase = ph->phase - PI;
		}
	}
}

//相量相加
void phasor_add(struct Phasor* ph1, struct Phasor* ph2, struct Phasor* result)
{
	struct Complex tmp1,tmp2,sum;
	phasor_to_complex(ph1, &tmp1);
	phasor_to_complex(ph2, &tmp2);
	complex_add(&tmp1,&tmp2,&sum);
	complex_to_phasor(&sum,result);
}

//相量乘标量
void phasor_mult_scalar(struct Phasor* ph, float scalar, struct Phasor* result)
{
	result->gain = ph->gain*scalar;
	result->phase = ph->phase;
}