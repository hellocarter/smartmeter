#include "display.h"
/**
	* 0:NA
	* 1:上b-下c-sigma-中b-上a-var-中c-中a
	* 2:中A-下V-AL2-中H-中V-cos-下A-Hz
	* 3:DO-上V-k-M-DI-WH-上A-AL1
	* 4-15:right->left,down->up
	*/

unsigned char menu0_buf[16];
unsigned char menu1_buf[16];
unsigned char menu2_buf[16];
unsigned char menu3_buf[16];
unsigned char menu4_buf[16];
unsigned char menu5_buf[16];
const unsigned char NUM_TAB[16]={0x5f,0x05,0xd9,0x9d,0x87,0x9e,0xde,0x15,0xdf,0x9f,0xd7,0xce,0x5a,0xcd,0xda,0xd2};
const unsigned char ADD_DOT=0x20;
const unsigned char CHAR_P=0xD3,CHAR_U=0x4f,CHAR_T=0xca,CHAR_R=0xc0;

unsigned char KEY_LEFT=0;
unsigned char KEY_RIGHT=0;
unsigned char KEY_SET=0;
unsigned char KEY_ENTER=0;

//sys parameters

//三相电压
s_dotnum st_volt[3];
//三相电流
s_dotnum st_current[3];

//电压或者电流
uint8_t show_flag;

//电压电流变比
int16_t volt_ratio;
int16_t current_ratio;

//波特率
const uint8_t BAUD_NUM=4;
const uint16_t BAUD_TAB[BAUD_NUM]={1200,2400,4800,9600};
int8_t baud_index;

//通讯地址
int16_t com_addr;
int16_t ADDR_MAX=247;
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
void display_clear(void)
{

}
//显示电流
void show_current()
{
	int i;
	for(i=0;i<3;i++)
	{
		menu0_buf[i*4+4] = NUM_TAB[st_current[i].number%10];
		menu0_buf[i*4+1+4]= NUM_TAB[(st_current[i].number/10)%10];
		menu0_buf[i*4+2+4]= NUM_TAB[(st_current[i].number/100)%10];
		menu0_buf[i*4+3+4]= NUM_TAB[(st_current[i].number/1000)%10];
	}
	for(i=0;i<3;i++)
	{
		if(st_current[i].dot>3)
		{
			st_current[i].dot=3;
		}
		if(st_current[i].dot>0){
			menu0_buf[i*4+st_current[i].dot+4]|=ADD_DOT;
		}
		
	}
	//show phase a b c
	menu0_buf[0]=0x00;
	menu0_buf[1]=0x1a;
	menu0_buf[2]=0x41;
	menu0_buf[3]=0x40;
	
	display_refresh(menu0_buf);
}
//显示电压
void show_voltage()
{
	int i;
	for(i=0;i<3;i++)
	{
		menu0_buf[i*4+4] = NUM_TAB[st_volt[i].number%10];
		menu0_buf[i*4+1+4]= NUM_TAB[(st_volt[i].number/10)%10];
		menu0_buf[i*4+2+4]= NUM_TAB[(st_volt[i].number/100)%10];
		menu0_buf[i*4+3+4]= NUM_TAB[(st_volt[i].number/1000)%10];
	}
	for(i=0;i<3;i++)
	{
		if(st_volt[i].dot>3)
		{
			st_volt[i].dot=3;
		}
		if(st_volt[i].dot>0){
			menu0_buf[i*4+st_volt[i].dot+4]|=ADD_DOT;
		}
		
	}
	//show phase a b c
	menu0_buf[0]=0x00;
	menu0_buf[1]=0x1a;
	menu0_buf[2]=0x12;
	menu0_buf[3]=0x02;
	
	display_refresh(menu0_buf);
}
void display_menu0()
{
	if(show_flag)
	{
		show_voltage();
	}
	else
	{
		show_current();
	}
}
//menu1:设置电流电压
//disp,A,U
void display_menu1()
{
	menu1_buf[15]=NUM_TAB[0x0d];
	menu1_buf[14]=NUM_TAB[1];
	menu1_buf[13]=NUM_TAB[5];
	menu1_buf[12]=CHAR_P;
	if(show_flag)
	{
		menu1_buf[8]=CHAR_U;
	}
	else
	{
		menu1_buf[8]=NUM_TAB[0x0a];
	}
	display_refresh(menu1_buf);
}
//menu2:设置电压变比
void display_menu2()
{
	menu2_buf[15]=CHAR_P;
	menu2_buf[14]=CHAR_T;
	menu2_buf[11]=NUM_TAB[volt_ratio/1000];
	menu2_buf[10]=NUM_TAB[volt_ratio/100%10];
	menu2_buf[9]=NUM_TAB[volt_ratio/10%10];
	menu2_buf[8]=NUM_TAB[volt_ratio%10];
	display_refresh(menu2_buf);
}
//menu3:设置电流变比
void display_menu3()
{
	menu3_buf[15]=NUM_TAB[0x0c];
	menu3_buf[14]=CHAR_T;
	menu3_buf[11]=NUM_TAB[current_ratio/1000];
	menu3_buf[10]=NUM_TAB[current_ratio/100%10];
	menu3_buf[9]=NUM_TAB[current_ratio/10%10];
	menu3_buf[8]=NUM_TAB[current_ratio%10];
	display_refresh(menu3_buf);
}
//menu4:设置波特率
void display_menu4()
{
	menu4_buf[15]=NUM_TAB[0x0b];
	menu4_buf[14]=NUM_TAB[0x0a];
	menu4_buf[13]=CHAR_U;
	menu4_buf[12]=NUM_TAB[0x0d];
	menu4_buf[11]=NUM_TAB[BAUD_TAB[baud_index]/1000];
	menu4_buf[10]=NUM_TAB[BAUD_TAB[baud_index]/100%10];
	menu4_buf[9]=NUM_TAB[BAUD_TAB[baud_index]/10%10];
	menu4_buf[8]=NUM_TAB[BAUD_TAB[baud_index]%10];
	display_refresh(menu4_buf);
}
//menu5:设置通讯地址
void display_menu5()
{
	menu5_buf[15]=NUM_TAB[0x0a];
	menu5_buf[14]=NUM_TAB[0x0d];
	menu5_buf[13]=NUM_TAB[0x0d];
	menu5_buf[12]=CHAR_R;
	
	menu5_buf[10]=NUM_TAB[com_addr/100%10];
	menu5_buf[9]=NUM_TAB[com_addr/10%10];
	menu5_buf[8]=NUM_TAB[com_addr%10];
	display_refresh(menu5_buf);
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

