#include "math.h"

//����
struct Phasor
{
	//����
	float gain;
	
	//��ǣ���λ����
	float phase;
};

//����
struct Complex
{
	//ʵ��
	float a;
	
	//�鲿
	float b;
};

//�������
void complex_add(struct Complex* cp1, struct Complex* cp2, struct Complex* result);

//����ת����
void phasor_to_complex(struct Phasor* ph, struct Complex* cp);

//����ת����
void complex_to_phasor(struct Complex* cp, struct Phasor* ph);

//�������
void phasor_add(struct Phasor* ph1, struct Phasor* ph2, struct Phasor* result);

//�����˱���
void phasor_mult_scalar(struct Phasor* ph, float scalar, struct Phasor* result);

