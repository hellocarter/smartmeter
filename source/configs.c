#include "configs.h"
#include "modbus.h"

#define PAGE_ADDR 0x0801f000

const uint16_t FLASH_NUM = 19;

//内部寄存器偏置量
const uint16_t REG_OFFSET = 16;
const uint16_t REG_NUM = 48;

//波特率
const uint8_t BAUD_NUM = 4;
const uint16_t BAUD_TAB[BAUD_NUM]={1200,2400,4800,9600};
const int16_t ADDR_MAX=247;


uint8_t* regs[REG_NUM];

//实际密码
int16_t password;

//显示密码
int16_t password_disp;

//电压或者电流,0电流,1电压
uint8_t show_flag;

//电压接法,0-三相四线,1-三相三线
int8_t volt_conn_type = 0;

//电流接法,0-三相3CT,1-三相2CT
int8_t current_conn_type = 0;

int16_t volt_ratio;
int16_t current_ratio;
int8_t baud_index;
int16_t com_addr;

int8_t io_in1;
int8_t io_in2;
int8_t io_out1;
int8_t io_out2;

int8_t alarm_type1;
int8_t alarm_type2;

//显示AL1,AL2
int8_t alarm_AL1;
int8_t alarm_AL2;

int16_t alarm_value1;
int16_t alarm_value2;

//电压校准值
int16_t voltage_factor[3];

//电流校准值
int16_t current_factor[3];

float measured_volts[3];
float measured_currents[3];

static uint8_t MemReadByte(int16_t *data,uint16_t num);
static uint8_t MemWriteByte(int16_t *data,uint16_t num);
static void configs_read(int16_t* buf);

//寄存器内存映射关系,高位在前
/*
00,01--volt_ratio 16b
02,03--current_ratio 16b
04,05--baud_rate 16b
06,07--com_addr 8b

08-11--voltageA 32b
12-15--voltageB 32b
16-19--voltageC 32b

20-23--currentA 32b
24-27--currentB 32b
28-31--currentC 32b
*/
void map_register()
{
	uint16_t i;
	
	regs[0] = (uint8_t*)voltage_factor + 1; 
	regs[1] = (uint8_t*)voltage_factor + 0; 	
	regs[2] = (uint8_t*)voltage_factor + 3; 
	regs[3] = (uint8_t*)voltage_factor + 2; 	
	regs[4] = (uint8_t*)voltage_factor + 5; 
	regs[5] = (uint8_t*)voltage_factor + 4; 
	
	regs[6] = (uint8_t*)voltage_factor + 7; 
	regs[7] = (uint8_t*)voltage_factor + 6; 	
	regs[8] = (uint8_t*)voltage_factor + 9; 
	regs[9] = (uint8_t*)voltage_factor + 8; 	
	regs[10] = (uint8_t*)voltage_factor + 11; 
	regs[11] = (uint8_t*)voltage_factor + 10; 
	
	regs[REG_OFFSET+0] = (uint8_t*)&volt_ratio+1;
	regs[REG_OFFSET+1] = ((uint8_t*)&volt_ratio);
	
	regs[REG_OFFSET+2] = (uint8_t*)&current_ratio+1;
	regs[REG_OFFSET+3] = ((uint8_t*)&current_ratio);
	
	//volts: reg4~15
	for(i=0;i<12;i++)
	{
		regs[REG_OFFSET+4+i] = ((uint8_t*)measured_volts)+(12-i-1);
	}
	
	//currents: reg16~27
	for(i=0;i<12;i++)
	{
		regs[REG_OFFSET+16+i] = ((uint8_t*)measured_currents)+(12-i-1);
	}
	//DI: reg28~29
	regs[REG_OFFSET+28] =  (uint8_t*)&io_in1;
	regs[REG_OFFSET+29] =  (uint8_t*)&io_in2;
	
	//DO: reg30~31
	regs[REG_OFFSET+30] =  (uint8_t*)&io_out1;
	regs[REG_OFFSET+31] =  (uint8_t*)&io_out2;
}

void configs_save()
{
	int16_t buf[FLASH_NUM];
	buf[0] = 1;
	buf[1] = show_flag;
	buf[2] = volt_ratio;
	buf[3] = current_ratio;
	buf[4] = baud_index;
	buf[5] = com_addr;
	
	buf[6] = alarm_type1;
	buf[7] = alarm_type2;
	buf[8] = alarm_value1;
	buf[9] = alarm_value2;
	
	buf[10] = voltage_factor[0];
	buf[11] = voltage_factor[1];
	buf[12] = voltage_factor[2];
	
	buf[13] = current_factor[0];
	buf[14] = current_factor[1];
	buf[15] = current_factor[2];
	
	buf[16] = password;
	
  buf[17] = volt_conn_type;
  buf[18] = current_conn_type;
  
	MemWriteByte(buf,FLASH_NUM);
	modbus_init(BAUD_TAB[baud_index]);
  
  password_disp = 1;
}

void load_configs()
{
	int16_t config_paras[FLASH_NUM];
	int16_t init_flag;
	
	configs_read(config_paras);
	init_flag = config_paras[0];
	
	//init_flag为1表示已初始化
	if (init_flag == 1)
	{
		show_flag = config_paras[1];
		volt_ratio = config_paras[2];
		current_ratio = config_paras[3];
		baud_index = config_paras[4];
		com_addr = config_paras[5];
		alarm_type1 = config_paras[6];
		alarm_type2 = config_paras[7];
		alarm_value1 = config_paras[8];
		alarm_value2 = config_paras[9];
		
		voltage_factor[0] = CALIBRATE_LOWER_LIMIT<config_paras[10]&&config_paras[10]<CALIBRATE_UPPER_LIMIT?config_paras[10]:1000;
	  voltage_factor[1] = CALIBRATE_LOWER_LIMIT<config_paras[11]&&config_paras[11]<CALIBRATE_UPPER_LIMIT?config_paras[11]:1000;
		voltage_factor[2] = CALIBRATE_LOWER_LIMIT<config_paras[12]&&config_paras[12]<CALIBRATE_UPPER_LIMIT?config_paras[12]:1000;

		current_factor[0] = CALIBRATE_LOWER_LIMIT<config_paras[13]&&config_paras[13]<CALIBRATE_UPPER_LIMIT?config_paras[13]:1000;
		current_factor[1] = CALIBRATE_LOWER_LIMIT<config_paras[14]&&config_paras[14]<CALIBRATE_UPPER_LIMIT?config_paras[14]:1000;
		current_factor[2] = CALIBRATE_LOWER_LIMIT<config_paras[15]&&config_paras[15]<CALIBRATE_UPPER_LIMIT?config_paras[15]:1000;

		password = config_paras[16] > 0 && config_paras[16] < 100 ? config_paras[16]:1;
    volt_conn_type = config_paras[17];
    current_conn_type = config_paras[18];
    
		modbus_init(BAUD_TAB[baud_index]);
	}
	else
	{
		show_flag = 0;
		volt_ratio = 1;
		current_ratio = 1;
		baud_index = 1;
		com_addr = 1;
		
		voltage_factor[0] = 1000;
	  voltage_factor[1] = 1000;
		voltage_factor[2] = 1000;
	
		current_factor[0] = 1000;
		current_factor[1] = 1000;
		current_factor[2] = 1000;
		
		alarm_type1 = 0;
		alarm_type2 = 0;
		alarm_value1 = 0;
		alarm_value2 = 0;
		
    volt_conn_type = 0;
    current_conn_type = 0;
    
		password = 1;
		configs_save();
	}
  password_disp = 1;
}

static uint8_t MemReadByte(int16_t *data,uint16_t num)                                
{
	uint16_t *temp_addr = (uint16_t *)PAGE_ADDR;

	while(num --)
	{
		*data ++ = *(__IO int16_t*)temp_addr ++;
	}
																																																	
	return 1;                                                                                                        
}

static uint8_t MemWriteByte(int16_t *data,uint16_t num)                                
{
	FLASH_Status temp_stat;
	uint32_t temp_addr = PAGE_ADDR;

	FLASH_Unlock();
	temp_stat = FLASH_ErasePage(PAGE_ADDR);

	if(temp_stat != FLASH_COMPLETE)
	{
		FLASH_Lock();
		return 0;
	}

	while(num --)
	{
		temp_stat = FLASH_ProgramHalfWord(temp_addr,*data);
		if(temp_stat != FLASH_COMPLETE)
		{
			FLASH_Lock();
			return 0;
		}

		temp_addr += 2;
		data++;
	}

	FLASH_Lock();        
	return 1;
}

static void configs_read(int16_t* buf)
{
	MemReadByte(buf,FLASH_NUM);
}

void voltage_calibration(float* voltages)
{
	uint8_t i;
	for (i=0;i<3;i++)
	{
		float x = voltages[i];
		voltages[i] = x*voltage_factor[i]/1000;
	}
}

void current_calibration(float* currents)
{
	uint8_t i;
	for (i=0;i<3;i++)
	{
		float x = currents[i];
		if (x>1.0)
		{
			currents[i] = (0.047*x*x + 0.8487*x + 0.12)*current_factor[i]/1000;
		}
	}
}

