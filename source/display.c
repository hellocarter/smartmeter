#include "display.h"
#include "configs.h"
#include "stdio.h"
#include "string.h"
/**
	* 0:NA
	* 1:上b-下c-sigma-中b-上a-var-中c-中a
	* 2:中A-下V-AL2-中H-中V-cos-下A-Hz
	* 3:DO-上V-k-M-DI-WH-上A-AL1
	* 4-15:right->left,down->up
	*/
//显存
unsigned char disp_buf[16];

const unsigned char NUM_TAB[16]={0x5f,0x05,0xd9,0x9d,0x87,0x9e,0xde,0x15,0xdf,0x9f,0xd7,0xce,0x5a,0xcd,0xda,0xd2};
const unsigned char ADD_DOT=0x20;
const unsigned char CHAR_P=0xD3,CHAR_U=0x4f,CHAR_u=0x4c,CHAR_T=0xca,CHAR_R=0xc0,CHAR_N=0x57,CHAR_O=0xcc,CHAR_L=0x4a,CHAR_H=0xc7,CHAR_BAR=0x80;

unsigned char KEY_LEFT=0;
unsigned char KEY_RIGHT=0;
unsigned char KEY_SET=0;
unsigned char KEY_ENTER=0;

//-,aL,bL,cL,aH,bH,cH
const uint16_t TYPE_TAB[ALARM_TYPE_NUM] = {0x0080,0xd74a,0xce4a,0x5a4a,0xd7c7,0xcec7,0x5ac7};

typedef struct{
	uint16_t number;
	uint8_t dot;
}s_dotnum;

//三相电压
static s_dotnum st_volt[3];
//三相电流
static s_dotnum st_current[3];

//初始化显示设备
void display_init(void)
{
	TM1629C_Init();
	TM1629C_Clear();
}

//refresh diplay buffer
static void display_refresh(uint8_t *disp_buf)
{
	unsigned char buf[16];
	int i;
	for(i=0;i<8;i++)
	{
		int j;
		short tmp=0;
		for(j=0;j<16;j++)
		{
			if((disp_buf[j]>>i)&0x01)
			{
				tmp=(tmp<<1)|0x01;
			}
			else
			{
				tmp=(tmp<<1);
			}
		}
		buf[i*2]=tmp;
		buf[i*2+1]=tmp>>8;
	}
	TM1629C_Refresh(buf);
}

//清空本地缓存及显示驱动缓存
void display_clear()
{
  memset(disp_buf,0,16);
	display_refresh(disp_buf);
}

//DO,DI,AL1,AL2等附加显示项
static void display_additional()
{
	//DI
	disp_buf[3] = (io_in1 || io_in2)? disp_buf[3]|0x10 : disp_buf[3];
	
	//DO
	disp_buf[3] = (io_out1 || io_out2) ? disp_buf[3]|0x01 : disp_buf[3];
	
	//AL1
	disp_buf[3] = alarm_AL1 ? disp_buf[3]|0x80 : disp_buf[3];
	
	//AL2
	disp_buf[2] = alarm_AL2 ? disp_buf[2]|0x04 : disp_buf[2];
}

//显示电压
void display_show_voltage()
{
	int i;
	display_set_volts(measured_volts);
	for(i=0;i<3;i++)
	{
		disp_buf[i*4+4] = NUM_TAB[st_volt[i].number%10];
		disp_buf[i*4+1+4]= NUM_TAB[(st_volt[i].number/10)%10];
		disp_buf[i*4+2+4]= NUM_TAB[(st_volt[i].number/100)%10];
		disp_buf[i*4+3+4]= NUM_TAB[(st_volt[i].number/1000)%10];
	}
	for(i=0;i<3;i++)
	{
		if(st_volt[i].dot>3)
		{
			st_volt[i].dot=3;
		}
		if(st_volt[i].dot>0){
			disp_buf[i*4+st_volt[i].dot+4]|=ADD_DOT;
		}
	}
	disp_buf[0]=0x00;
	if (volt_conn_type)
	{
		//show line ab bc ca
		disp_buf[1]=0x1a|0xc3;
	}
	else
	{
		//show phase a b c
		disp_buf[1]=0x1a;
	}
	disp_buf[2]=0x12;
	disp_buf[3]=0x02;
	
	display_additional();
	
	display_refresh(disp_buf);
}

//显示电流
void display_show_current()
{
	int i;
	display_set_currents(measured_currents);
	for(i=0;i<3;i++)
	{
		disp_buf[i*4+4] = NUM_TAB[st_current[i].number%10];
		disp_buf[i*4+1+4]= NUM_TAB[(st_current[i].number/10)%10];
		disp_buf[i*4+2+4]= NUM_TAB[(st_current[i].number/100)%10];
		disp_buf[i*4+3+4]= NUM_TAB[(st_current[i].number/1000)%10];
	}
	for(i=0;i<3;i++)
	{
		if(st_current[i].dot>3)
		{
			st_current[i].dot=3;
		}
		if(st_current[i].dot>0){
			disp_buf[i*4+st_current[i].dot+4]|=ADD_DOT;
		}
	}
		
	disp_buf[0]=0x00;
	disp_buf[1]=0x1a;
	disp_buf[2]=0x41;
	disp_buf[3]=0x40;
	
	display_additional();
	
	display_refresh(disp_buf);
}

//display page for volt or current
void display_show_measure()
{
	if (show_flag == 0)
	{
		display_show_voltage();
	}
	else
	{
		display_show_current();
	}
}

//设置电压变比
void display_voltage_ratio()
{
	disp_buf[15]=CHAR_P;
	disp_buf[14]=CHAR_T;
	disp_buf[11]=NUM_TAB[volt_ratio/1000];
	disp_buf[10]=NUM_TAB[volt_ratio/100%10];
	disp_buf[9]=NUM_TAB[volt_ratio/10%10];
	disp_buf[8]=NUM_TAB[volt_ratio%10];
	display_refresh(disp_buf);
}

//设置电流变比
void display_current_ratio()
{
	disp_buf[15]=NUM_TAB[0x0c];
	disp_buf[14]=CHAR_T;
	disp_buf[11]=NUM_TAB[current_ratio/1000];
	disp_buf[10]=NUM_TAB[current_ratio/100%10];
	disp_buf[9]=NUM_TAB[current_ratio/10%10];
	disp_buf[8]=NUM_TAB[current_ratio%10];
	display_refresh(disp_buf);
}

//电压连接方式
void display_conn_type()
{
	disp_buf[15]=NUM_TAB[0x0c];
	disp_buf[14]=NUM_TAB[0x0];
	disp_buf[13]=CHAR_N;
	disp_buf[12]=CHAR_N;
	
	//set for volt
	disp_buf[11]=CHAR_U;
	if (volt_conn_type)
	{
		disp_buf[8]=NUM_TAB[0x03];
	}
	else
	{
		disp_buf[8]=NUM_TAB[0x04];
	}
	
	//set for current
	disp_buf[7]=NUM_TAB[0x0a];
	if (current_conn_type)
	{
		disp_buf[4]=NUM_TAB[0x03];
	}
	else
	{
		disp_buf[4]=NUM_TAB[0x04];
	}
	display_refresh(disp_buf);
}

//报警输出类型
void display_alarm_type()
{
	disp_buf[15]=CHAR_O;
	disp_buf[14]=CHAR_u;
	disp_buf[13]=CHAR_T;
	disp_buf[12]=0;
	
	//set for o1
	disp_buf[11]=NUM_TAB[0x01];
	disp_buf[9] = TYPE_TAB[alarm_type1]>>8;
	disp_buf[8] = TYPE_TAB[alarm_type1];
	
	//set for o2
	disp_buf[7]=NUM_TAB[0x02];
	disp_buf[5]=TYPE_TAB[alarm_type2]>>8;
	disp_buf[4]=TYPE_TAB[alarm_type2];
	
	display_refresh(disp_buf);
}

//报警输出1
void display_alarm_value1()
{
	disp_buf[15]=CHAR_O;
	disp_buf[14]=NUM_TAB[0x01];
	disp_buf[13]=0;
	disp_buf[12]=0;
	
	disp_buf[11]=NUM_TAB[0x00];
	disp_buf[10]=NUM_TAB[alarm_value1/100%10];
	disp_buf[9]=NUM_TAB[alarm_value1/10%10];
	disp_buf[8]=NUM_TAB[alarm_value1%10];
	
	display_refresh(disp_buf);
}

//报警输出2
void display_alarm_value2()
{
	disp_buf[15]=CHAR_O;
	disp_buf[14]=NUM_TAB[0x02];
	disp_buf[13]=0;
	disp_buf[12]=0;
	
	disp_buf[11]=NUM_TAB[0x00];
	disp_buf[10]=NUM_TAB[alarm_value2/100%10];
	disp_buf[9]=NUM_TAB[alarm_value2/10%10];
	disp_buf[8]=NUM_TAB[alarm_value2%10];
	
	display_refresh(disp_buf);
}

//设置波特率
void display_baudrate()
{
	disp_buf[15]=NUM_TAB[0x0b];
	disp_buf[14]=NUM_TAB[0x0a];
	disp_buf[13]=CHAR_U;
	disp_buf[12]=NUM_TAB[0x0d];
	disp_buf[11]=NUM_TAB[BAUD_TAB[baud_index]/1000];
	disp_buf[10]=NUM_TAB[BAUD_TAB[baud_index]/100%10];
	disp_buf[9]=NUM_TAB[BAUD_TAB[baud_index]/10%10];
	disp_buf[8]=NUM_TAB[BAUD_TAB[baud_index]%10];
	display_refresh(disp_buf);
}
//设置通讯地址
void display_com_addr()
{
	disp_buf[15]=NUM_TAB[0x0a];
	disp_buf[14]=NUM_TAB[0x0d];
	disp_buf[13]=NUM_TAB[0x0d];
	disp_buf[12]=CHAR_R;
	
	disp_buf[10]=NUM_TAB[com_addr/100%10];
	disp_buf[9]=NUM_TAB[com_addr/10%10];
	disp_buf[8]=NUM_TAB[com_addr%10];
	display_refresh(disp_buf);
}

void display_getkeys()
{
	unsigned char buf[4];
	TM1629C_ReadKey(buf);
	KEY_RIGHT=buf[0]&0x0f;
	KEY_LEFT=buf[0]&0xf0;
	KEY_SET=buf[1]&0xf0;
	KEY_ENTER=buf[1]&0x0f;
}

//浮点数转显示结构体
static void convert(float v,s_dotnum *st)
{
	uint16_t big_part;
	uint8_t dot;
	uint16_t mult=0;
	if(v<0)
	{
		v=0.0;
	}
	if(v>9999)
	{
		v=9999.0;
	}
	big_part = v;
	if (big_part > 999)
	{
		mult = 1;
		dot = 0;
	}
	else if (big_part > 99)
	{
		mult = 10;
		dot = 1;
	}
	else if (big_part > 9)
	{
		mult = 100;
		dot = 2;
	}
	else
	{
		mult = 1000;
		dot = 3;
	}
	
	st->number = v*mult;
	st->dot = dot;
	
}
//设置电压显示值
void display_set_volts(float *volts)
{
	char i;
	for(i=0;i<3;i++)
	{
		convert(volts[2-i]*volt_ratio,st_volt+i);
	}	
}
//设置电流显示值
void display_set_currents(float *currents)
{
	char i;
	for(i=0;i<3;i++)
	{
		convert(currents[2-i]*current_ratio,st_current+i);
	}	
}
