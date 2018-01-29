#include "display.h"
#include "configs.h"
#include "stdio.h"
#include "string.h"

/**********************�ײ���ʾ�����ֶζ���****************************/
/**
	* 0:NA
	* 1:��b-��c-sigma-��b-��a-var-��c-��a
	* 2:��A-��V-AL2-��H-��V-cos-��A-Hz
	* 3:DO-��V-k-M-DI-WH-��A-AL1
	* 4-15:right->left,down->up
	*/
//�Դ�
unsigned char disp_buf[16];

const unsigned char NUM_TAB[16]={0x5f,0x05,0xd9,0x9d,0x87,0x9e,0xde,0x15,
                                  0xdf,0x9f,0xd7,0xce,0x5a,0xcd,0xda,0xd2};
const unsigned char ADD_DOT=0x20;
const unsigned char CHAR_P=0xD3,CHAR_U=0x4f,CHAR_u=0x4c,CHAR_T=0xca,CHAR_R=0xc0,CHAR_N=0x57,CHAR_O=0xcc,
                    CHAR_L=0x4a,CHAR_H=0xc7,CHAR_BAR=0x80,CHAR_n=0xc4,CHAR_l=0x42,CHAR_Y=0x8f;

unsigned char KEY_LEFT=0;
unsigned char KEY_RIGHT=0;
unsigned char KEY_SET=0;
unsigned char KEY_ENTER=0;

//-,aL,bL,cL,aH,bH,cH
const uint16_t TYPE_TAB[ALARM_TYPE_NUM] = {0x0080,0xd74a,0xce4a,0x5a4a,0xd7c7,0xcec7,0x5ac7};

/*********************************��ʾ������������****************************************/
void display_show_measure(str_menu* self);
void display_code(str_menu* self);
void display_conn_type(str_menu* self);
void display_alarm_type(str_menu* self);
void display_alarm_value1(str_menu* self);
void display_alarm_value2(str_menu* self);
void display_voltage_ratio(str_menu* self);
void display_current_ratio(str_menu* self);
void display_baudrate(str_menu* self);
void display_com_addr(str_menu* self);
void display_set_password(str_menu* self);
void display_confirm(str_menu* self);

void key_show_measure(str_menu* self);
void key_code(str_menu* self);
void key_conn_type(str_menu* self);
void key_alarm_type(str_menu* self);
void key_alarm_value1(str_menu* self);
void key_alarm_value2(str_menu* self);
void key_voltage_ratio(str_menu* self);
void key_current_ratio(str_menu* self);
void key_baudrate(str_menu* self);
void key_com_addr(str_menu* self);
void key_set_password(str_menu* self);
void key_confirm(str_menu* self);

/********************************�˵���ʾ�ṹ����*********************************/
const uint8_t MENU_NUM = 12;
const uint8_t CONFIRM_INDEX = MENU_NUM -1 ;
const uint16_t TOGGLE_PERIOD = 500;
uint8_t menu_index = 0;
uint8_t menu_switch_flag = 0;
uint8_t is_save;

str_menu menu_show_measure = {0,0xff,0,display_show_measure,key_show_measure},
menu_code = {0,0xff,0,display_code,key_code}, 
menu_conn_type = {0,0xff,0,display_conn_type,key_conn_type},
menu_alarm_type = {0,0xff,0,display_alarm_type,key_alarm_type}, 
menu_alarm_value1 = {0,0xff,0,display_alarm_value1,key_alarm_value1},
menu_alarm_value2 = {0,0xff,0,display_alarm_value2,key_alarm_value2}, 
menu_voltage_ratio = {0,0xff,0,display_voltage_ratio,key_voltage_ratio}, 
menu_current_ratio = {0,0xff,0,display_current_ratio,key_current_ratio}, 
menu_baudrate = {0,0xff,0,display_baudrate,key_baudrate}, 
menu_com_addr = {0,0xff,0,display_com_addr,key_com_addr},
menu_set_password = {0,0xff,0,display_set_password,key_set_password}, 
menu_confirm = {0,0xff,0,display_confirm,key_confirm};

str_menu* p_menu[MENU_NUM] = {&menu_show_measure, &menu_code, &menu_conn_type, &menu_alarm_type,
&menu_alarm_value1, &menu_alarm_value2, &menu_voltage_ratio, &menu_current_ratio, &menu_baudrate,
&menu_com_addr, &menu_set_password, &menu_confirm};

/*************************************������������****************************************/
uint8_t get_digit(int16_t num, uint8_t index);
int16_t change_digit(int16_t num, uint8_t index, uint8_t digit);
int16_t add_digit(int16_t num, uint8_t index);
int16_t dec_digit(int16_t num, uint8_t index);

typedef struct{
	uint16_t number;
	uint8_t dot;
}s_dotnum;

//�����ѹ
static s_dotnum st_volt[3];
//�������
static s_dotnum st_current[3];

//��ʼ����ʾ�豸
void display_init(void)
{
	TM1629C_Init();
	TM1629C_Clear();
}

/********��ʾ������������ú���*****/
void menu_call()
{
  static uint8_t old_index =0 ;
  if (old_index != menu_index)
  {
    menu_switch_flag = 1;
    display_clear();
  }
  else
  {
    menu_switch_flag = 0;
  }
  old_index = menu_index;
  p_menu[menu_index]->disp(p_menu[menu_index]);
  p_menu[menu_index]->key(p_menu[menu_index]);
}

/************ˢ����ʾ**********/
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

/**********��ձ��ػ��漰��ʾ��������*******/
void display_clear()
{
  memset(disp_buf,0,16);
	display_refresh(disp_buf);
}

/********DO,DI,AL1,AL2�ȸ�����ʾ��*********/
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

/*********************�˵���ʾ������������*********************/

/***********************1.������ʾ*********/
//��ʾ��ѹ
void display_show_voltage(str_menu* self)
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

//��ʾ����
void display_show_current(str_menu* self)
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
void display_show_measure(str_menu* self)
{
	if (show_flag == 0)
	{
		display_show_voltage(self);
	}
	else
	{
		display_show_current(self);
	}
}
//������ʾ��������
void key_show_measure(str_menu* self)
{
  if(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
    if(KEY_LEFT || KEY_RIGHT){
      if(show_flag){
        show_flag=0;
      }
      else{
        show_flag=1;
      }
    }
    if (KEY_SET){
      menu_index++;
    }
    while(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
      display_getkeys();
    }
  }
}

/****************2.�����趨��������*****************/
void display_code(str_menu* self)
{
  if (menu_switch_flag)
  {
    self->index = 8;
    password_disp=1;
  }
	disp_buf[15] = NUM_TAB[0x0c];
	disp_buf[14] = CHAR_O;
	disp_buf[13] = NUM_TAB[0x0d];
	disp_buf[12] = NUM_TAB[0x0e];
	
	disp_buf[11]=NUM_TAB[password_disp/1000];
	disp_buf[10]=NUM_TAB[password_disp/100%10];
	disp_buf[9]=NUM_TAB[password_disp/10%10];
	disp_buf[8]=NUM_TAB[password_disp%10];
  
  //��˸λ
  disp_buf[self->index] =  disp_buf[self->index] & self->flag;
  
	display_refresh(disp_buf);
  
  self->tick++;
  if (self->tick % TOGGLE_PERIOD == 0)
  {
    self->flag = self->flag > 0?0:0xff;
  }
}
void key_code(str_menu* self)
{
  if(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
    if(KEY_LEFT){
      self->index = self->index + 1 > 11 ? 8:self->index + 1;
    }
    if(KEY_RIGHT){
      password_disp = add_digit(password_disp,self->index - 8);
    }
    if (KEY_SET){
      menu_index=0;
    }
    if (KEY_ENTER){
      if (password_disp == password){
        //�����������ǰ���뵱ǰ����
        load_configs();
        menu_index++;
      }
    }
    while(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
      display_getkeys();
    }
  }
}

/**************3.���ӷ�ʽ**************/
void display_conn_type(str_menu* self)
{
	disp_buf[15]=CHAR_l;
	disp_buf[14]=CHAR_n|ADD_DOT;
	disp_buf[13]=CHAR_P;
	disp_buf[12]=CHAR_T;
	
	//set for volt
	disp_buf[11]=CHAR_U;
	if (volt_conn_type)
	{
    disp_buf[9]=NUM_TAB[0x03]|ADD_DOT;
		disp_buf[8]=NUM_TAB[0x03];
	}
	else
	{
    disp_buf[9]=NUM_TAB[0x03]|ADD_DOT;
		disp_buf[8]=NUM_TAB[0x04];
	}
	
	//set for current
	disp_buf[7]=NUM_TAB[0x0a];
	if (current_conn_type)
	{
		disp_buf[5]=NUM_TAB[0x02];
    disp_buf[4]=NUM_TAB[0x0c];
	}
	else
	{
		disp_buf[5]=NUM_TAB[0x03];
    disp_buf[4]=NUM_TAB[0x0c];
	}
	display_refresh(disp_buf);
}
void key_conn_type(str_menu* self)
{
  if(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
    if(KEY_LEFT){
      if(volt_conn_type){
        volt_conn_type=0;
      }
      else{
        volt_conn_type=1;
      }
    }
    if (KEY_RIGHT)
    {
      if(current_conn_type){
        current_conn_type=0;
      }
      else{
        current_conn_type=1;
      }
    }
    if (KEY_SET)
    {
      menu_index++;
    }
    if (KEY_ENTER)
    {
      menu_index = CONFIRM_INDEX;
    }
    while(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
      display_getkeys();
    }
  }
}
/****************4.������������*****************/
void display_alarm_type(str_menu* self)
{
	disp_buf[15]=NUM_TAB[0x0d];
	disp_buf[14]=CHAR_O;
	disp_buf[13]=0;
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
void key_alarm_type(str_menu* self)
{
  if(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
    if(KEY_LEFT){
      alarm_type1++;
      if(alarm_type1>=ALARM_TYPE_NUM){
        alarm_type1=0;
      }
    }
    if (KEY_RIGHT)
    {
      alarm_type2++;
      if(alarm_type2>=ALARM_TYPE_NUM){
        alarm_type2=0;
      }
    }
    if (KEY_SET)
    {
      menu_index++;
    }
    if (KEY_ENTER)
    {
      menu_index = CONFIRM_INDEX;
    }
    while(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
      display_getkeys();
    }
  }
}
/*************5.����1��ֵ����**********/
void display_alarm_value1(str_menu* self)
{
  if (menu_switch_flag)
  {
    self->index = 8;
    password_disp=1;
  }
  
	disp_buf[15]=NUM_TAB[0x0d];
	disp_buf[14]=CHAR_O;
	disp_buf[13]=CHAR_BAR;
	disp_buf[12]=NUM_TAB[0x01];
	
	disp_buf[10]=NUM_TAB[alarm_value1/100%10];
	disp_buf[9]=NUM_TAB[alarm_value1/10%10];
	disp_buf[8]=NUM_TAB[alarm_value1%10];
	
  disp_buf[4]=CHAR_U;
	disp_buf[self->index] =  disp_buf[self->index] & self->flag;
  
	display_refresh(disp_buf);
  
  self->tick++;
  if (self->tick % TOGGLE_PERIOD == 0)
  {
    self->flag = self->flag > 0?0:0xff;
  }
}
void key_alarm_value1(str_menu* self)
{
  int16_t tmp;
  if(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
    if(KEY_LEFT){
      self->index = self->index + 1 > 10 ? 8:self->index + 1;
    }
    if(KEY_RIGHT){
      tmp = add_digit(alarm_value1,self->index - 8);
      alarm_value1 = tmp <= 120 ? tmp:change_digit(alarm_value1,self->index - 8,0);
    }
    if (KEY_SET)
    {
      menu_index++;
    }
    if (KEY_ENTER)
    {
      menu_index = CONFIRM_INDEX;
    }
    while(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
      display_getkeys();
    }
  }
}
/*************6.����2��ֵ����**********/
void display_alarm_value2(str_menu* self)
{
  if (menu_switch_flag)
  {
    self->index = 8;
    password_disp=1;
  }
  
	disp_buf[15]=NUM_TAB[0x0d];
	disp_buf[14]=CHAR_O;
	disp_buf[13]=CHAR_BAR;
	disp_buf[12]=NUM_TAB[0x02];
	
	disp_buf[10]=NUM_TAB[alarm_value2/100%10];
	disp_buf[9]=NUM_TAB[alarm_value2/10%10];
	disp_buf[8]=NUM_TAB[alarm_value2%10];
  
  disp_buf[4]=NUM_TAB[0x0a];
	
	disp_buf[self->index] =  disp_buf[self->index] & self->flag;
  
	display_refresh(disp_buf);
  
  self->tick++;
  if (self->tick % TOGGLE_PERIOD == 0)
  {
    self->flag = self->flag > 0?0:0xff;
  }
}
void key_alarm_value2(str_menu* self)
{
  int16_t tmp;
  if(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
			if(KEY_LEFT){
        self->index = self->index + 1 > 10 ? 8:self->index + 1;
      }
      if(KEY_RIGHT){
        tmp = add_digit(alarm_value2,self->index - 8);
        alarm_value2 = tmp <= 120 ? tmp:change_digit(alarm_value2,self->index - 8,0);
      }
      if (KEY_SET)
      {
        menu_index++;
      }
      if (KEY_ENTER)
      {
        menu_index = CONFIRM_INDEX;
      }
			while(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
				display_getkeys();
			}
		}
}
/*************7.���õ�ѹ���*************/
void display_voltage_ratio(str_menu* self)
{
  if (menu_switch_flag)
  {
    self->index = 8;
  }
	disp_buf[15]=CHAR_P;
	disp_buf[14]=CHAR_T;
  
	disp_buf[11]=NUM_TAB[volt_ratio/1000];
	disp_buf[10]=NUM_TAB[volt_ratio/100%10];
	disp_buf[9]=NUM_TAB[volt_ratio/10%10];
	disp_buf[8]=NUM_TAB[volt_ratio%10];
  
  //��˸λ
  disp_buf[self->index] =  disp_buf[self->index] & self->flag;
  
	display_refresh(disp_buf);
  
  self->tick++;
  if (self->tick % TOGGLE_PERIOD == 0)
  {
    self->flag = self->flag > 0?0:0xff;
  }
  
}
void key_voltage_ratio(str_menu* self)
{
  if(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
    if(KEY_LEFT){
      self->index = self->index + 1 > 11 ? 8:self->index + 1;
    }
    if(KEY_RIGHT){
      volt_ratio = add_digit(volt_ratio,self->index - 8);
    }
    if (KEY_SET)
    {
      menu_index++;
    }
    if (KEY_ENTER)
    {
      menu_index = CONFIRM_INDEX;
    }
    while(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
      display_getkeys();
    }
  }
}
/*************8.���õ������*************/
void display_current_ratio(str_menu* self)
{
  if (menu_switch_flag)
  {
    self->index = 8;
  }
  
	disp_buf[15]=NUM_TAB[0x0c];
	disp_buf[14]=CHAR_T;
  
	disp_buf[11]=NUM_TAB[current_ratio/1000];
	disp_buf[10]=NUM_TAB[current_ratio/100%10];
	disp_buf[9]=NUM_TAB[current_ratio/10%10];
	disp_buf[8]=NUM_TAB[current_ratio%10];
  
  //��˸λ
  disp_buf[self->index] =  disp_buf[self->index] & self->flag;
  
	display_refresh(disp_buf);
  self->tick++;
  if (self->tick % TOGGLE_PERIOD == 0)
  {
    self->flag = self->flag > 0?0:0xff;
  }
}
void key_current_ratio(str_menu* self)
{
  if(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
    if(KEY_LEFT){
      self->index = self->index + 1 > 11 ? 8:self->index + 1;
    }
    if(KEY_RIGHT){
      current_ratio = add_digit(current_ratio,self->index - 8);
    }
    if (KEY_SET)
    {
      menu_index++;
    }
    if (KEY_ENTER)
    {
      menu_index = CONFIRM_INDEX;
    }
    while(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
      display_getkeys();
    }
  }
}
/*************9.����������*********/
void display_baudrate(str_menu* self)
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
void key_baudrate(str_menu* self)
{
  if(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
    if(KEY_LEFT){
      baud_index--;
      if(baud_index<0){
        baud_index=BAUD_NUM-1;
      }
    }
    if(KEY_RIGHT){
      baud_index++;
      if(baud_index>=BAUD_NUM){
        baud_index=0;
      }
    }
    if (KEY_SET)
    {
      menu_index++;
    }
    if (KEY_ENTER)
    {
      menu_index = CONFIRM_INDEX;
    }
    while(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
      display_getkeys();
    }
  }
}
/************10.����ͨѶ��ַ***********/
void display_com_addr(str_menu* self)
{
  if (menu_switch_flag)
  {
    self->index = 8;
    password_disp=1;
  }
  
	disp_buf[15]=NUM_TAB[0x0a];
	disp_buf[14]=NUM_TAB[0x0d];
	disp_buf[13]=NUM_TAB[0x0d];
	disp_buf[12]=CHAR_R;
	
	disp_buf[10]=NUM_TAB[com_addr/100%10];
	disp_buf[9]=NUM_TAB[com_addr/10%10];
	disp_buf[8]=NUM_TAB[com_addr%10];
  
	disp_buf[self->index] =  disp_buf[self->index] & self->flag;
  
	display_refresh(disp_buf);
  
  self->tick++;
  if (self->tick % TOGGLE_PERIOD == 0)
  {
    self->flag = self->flag > 0?0:0xff;
  }
}
void key_com_addr(str_menu* self)
{
  int16_t tmp_addr;
  if(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
    if(KEY_LEFT){
      self->index = self->index + 1 > 10 ? 8:self->index + 1;
    }
    if(KEY_RIGHT){
      tmp_addr = add_digit(com_addr,self->index - 8);
      com_addr = tmp_addr < ADDR_MAX ? tmp_addr:change_digit(com_addr,self->index - 8,0);
    }
    if (KEY_SET)
    {
      menu_index++;
    }
    if (KEY_ENTER)
    {
      menu_index = CONFIRM_INDEX;
    }
    while(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
      display_getkeys();
    }
  }
}

/************11.����������*************/
void display_set_password(str_menu* self)
{
  if (menu_switch_flag)
  {
    self->index = 4;
    password_disp=1;
  }
  disp_buf[15] = NUM_TAB[0x05];
	disp_buf[14] = NUM_TAB[0x0e];
	disp_buf[13] = CHAR_T;
	disp_buf[12] = 0;
  
	disp_buf[11] = NUM_TAB[0x0c];
	disp_buf[10] = CHAR_O;
	disp_buf[9] = NUM_TAB[0x0d];
	disp_buf[8] = NUM_TAB[0x0e];
	
	disp_buf[7]=NUM_TAB[password_disp/1000];
	disp_buf[6]=NUM_TAB[password_disp/100%10];
	disp_buf[5]=NUM_TAB[password_disp/10%10];
	disp_buf[4]=NUM_TAB[password_disp%10];
	//��˸λ
  disp_buf[self->index] =  disp_buf[self->index] & self->flag;
  
	display_refresh(disp_buf);
  
  self->tick++;
  if (self->tick % TOGGLE_PERIOD == 0)
  {
    self->flag = self->flag > 0?0:0xff;
  }
}
void key_set_password(str_menu* self)
{
  if(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
    if(KEY_LEFT){
      self->index = self->index + 1 > 7 ? 4:self->index + 1;
    }
    if(KEY_RIGHT){
      password_disp = add_digit(password_disp,self->index - 4);
    }
    if (KEY_SET)
    {
      menu_index++;
    }
    if (KEY_ENTER)
    {
      menu_index = CONFIRM_INDEX;
    }
    while(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
      display_getkeys();
    }
  }
}
/***********12.�����޸�ȷ�ϱ���*************/
void display_confirm(str_menu* self)
{
  if (menu_switch_flag)
  {
    is_save = 1;
  }
  disp_buf[15] = NUM_TAB[0x05];
	disp_buf[14] = NUM_TAB[0x0a];
	disp_buf[13] = CHAR_U;
	disp_buf[12] = NUM_TAB[0x0e];
  
  if (is_save)
  {
    disp_buf[10] = CHAR_Y;
    disp_buf[9] = NUM_TAB[0x0e];
    disp_buf[8] = NUM_TAB[0x05];
  }
  else
  {
    disp_buf[10] = 0;
    disp_buf[9] = CHAR_n;
    disp_buf[8] = CHAR_O;
  }
	
	display_refresh(disp_buf);
}
void key_confirm(str_menu* self)
{
  if(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
    if (KEY_LEFT||KEY_RIGHT)
    {
      is_save = is_save>0?0:1;
    }
    if (KEY_ENTER)
    {
      if (is_save)
      {
        password = password_disp;
        configs_save();
      }
      menu_index=0;
    }
    while(KEY_LEFT||KEY_RIGHT||KEY_SET||KEY_ENTER){
      display_getkeys();
    }
  }
}
/*****************************************/
void display_getkeys()
{
	unsigned char buf[4];
	TM1629C_ReadKey(buf);
	KEY_RIGHT=buf[0]&0x0f;
	KEY_LEFT=buf[0]&0xf0;
	KEY_SET=buf[1]&0xf0;
	KEY_ENTER=buf[1]&0x0f;
}

//��ȡ��λ���ֵ�ĳһλ
uint8_t get_digit(int16_t num, uint8_t index)
{
  uint8_t digit;
  switch (index)
  {
    case 0:
      digit = num % 10;
      break;
    case 1:
      digit = num /10 % 10;
      break;
    case 2:
      digit = num / 100 % 10;
      break;
    default:
      digit = num / 1000 % 10;
      break;
  }
  return digit;
}
//��ȡ�޸���λ����ĳһλ�Ľ��
int16_t change_digit(int16_t num, uint8_t index, uint8_t digit)
{
  int16_t result;
  switch (index)
  {
    case 0:
      result = num / 10 * 10 + digit;
      break;
    case 1:
      result = num/100*100 + digit*10 + num%10;
      break;
    case 2:
      result = num/1000*1000 + digit*100 + num%100;
      break;
    default:
      result = num%1000 + digit*1000;
      break;
  }
  return result;
}

//������λ����ĳһλ
int16_t add_digit(int16_t num, uint8_t index)
{
  uint8_t digit;
  digit = get_digit(num, index);
  digit++;
  return change_digit(num,index,digit%10);
}
//��С��λ����ĳһλ
int16_t dec_digit(int16_t num, uint8_t index)
{
  uint8_t digit;
  digit = get_digit(num, index);
  digit--;
  return change_digit(num,index,digit%10);
}

//������ת��ʾ�ṹ��
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
//���õ�ѹ��ʾֵ
void display_set_volts(float *volts)
{
	char i;
	for(i=0;i<3;i++)
	{
		convert(volts[2-i]*volt_ratio,st_volt+i);
	}	
}
//���õ�����ʾֵ
void display_set_currents(float *currents)
{
	char i;
	for(i=0;i<3;i++)
	{
		convert(currents[2-i]*current_ratio,st_current+i);
	}	
}
