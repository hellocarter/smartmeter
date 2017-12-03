#include "configs.h"
#include "modbus.h"

#define PAGE_ADDR 0x08010000

const uint16_t FLASH_NUM = 7;

//波特率
const uint8_t BAUD_NUM = 4;
const uint16_t BAUD_TAB[BAUD_NUM]={1200,2400,4800,9600};
const int16_t ADDR_MAX=247;

const uint16_t REG_NUM = 32;
uint8_t* regs[REG_NUM];

const float VOLT_THRESHOLD = 0.0;
const float CURRENT_THRESHOLD = 0.0;

//电压或者电流,0电流,1电压
uint8_t show_flag;

//电压接法,0-三相四线,1-三相三线
int8_t volt_conn_type = 1;

//电流接法,0-三相四线,1-三相三线
int8_t current_conn_type = 1;

int16_t volt_ratio;
int16_t current_ratio;
int8_t baud_index;
int16_t com_addr;

int8_t io_in1;
int8_t io_in2;
int8_t io_out1;
int8_t io_out2;

//校准值
uint16_t calibration;

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
	
	//regs[0] = &show_flag;
	
	regs[0] = (uint8_t*)&volt_ratio+1;
	regs[1] = ((uint8_t*)&volt_ratio);
	
	regs[2] = (uint8_t*)&current_ratio+1;
	regs[3] = ((uint8_t*)&current_ratio);
	
	regs[4] = (uint8_t*)&(BAUD_TAB[baud_index])+1;
	regs[5] = (uint8_t*)&(BAUD_TAB[baud_index]);
	
	regs[6] = (uint8_t*)&com_addr+1;
	regs[7] = ((uint8_t*)&com_addr);
	
	//volts: reg8~19
	for(i=0;i<12;i++)
	{
		regs[8+i] = ((uint8_t*)measured_volts)+(12-i-1);
	}
	
	//currents: reg20~31 
	for(i=0;i<12;i++)
	{
		regs[20+i] = ((uint8_t*)measured_currents)+(12-i-1);
	}
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
	buf[6] = calibration;
	
	MemWriteByte(buf,FLASH_NUM);
	modbus_init(BAUD_TAB[baud_index]);
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
		calibration = (uint16_t)config_paras[6];
		
		modbus_init(BAUD_TAB[baud_index]);
	}
	else
	{
		show_flag = 0;
		volt_ratio = 1;
		current_ratio = 1;
		baud_index = 1;
		com_addr = 1;
		
		//2.495v对应ad值
		calibration = 3096;
		configs_save();
	}
	
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

//y = 0.0507x^2 + 0.8194x + 0.1611
float current_calibration(float x)
{
	return 0.0507*x*x + 0.8194*x + 0.1611;
}

