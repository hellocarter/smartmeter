#include "configs.h"
#include "modbus.h"

#define PAGE_ADDR 0x08010000

//波特率
const uint8_t BAUD_NUM = 4;
const uint16_t BAUD_TAB[BAUD_NUM]={1200,2400,4800,9600};
const int16_t ADDR_MAX=247;

const uint16_t REG_MAX = 33;
uint8_t* regs[REG_MAX];

//电压或者电流,1电压，0电流
uint8_t show_flag;
int16_t volt_ratio;
int16_t current_ratio;
int8_t baud_index;
int16_t com_addr;

float measured_volts[3];
float measured_currents[3];

//寄存器内存映射关系
/*
00--show_flag
01,02--volt_ratio 16b
03,04--current_ratio 16b
05,06--baud_rate 16b
07-08--com_addr 8b

09-12--voltageA 32b
13-16--voltageB 32b
17-20--voltageC 32b

21-24--currentA 32b
25-28--currentB 32b
29-32--currentC 32b
*/
void map_register()
{
	uint16_t i;
	
	regs[0] = &show_flag;
	
	regs[1] = (uint8_t*)&volt_ratio;
	regs[2] = ((uint8_t*)&volt_ratio)+1;
	
	regs[3] = (uint8_t*)&current_ratio;
	regs[4] = ((uint8_t*)&current_ratio)+1;
	
	regs[5] = (uint8_t*)&(BAUD_TAB[baud_index]);
	regs[6] = (uint8_t*)&(BAUD_TAB[baud_index])+1;
	
	regs[7] = (uint8_t*)&com_addr;
	regs[8] = ((uint8_t*)&com_addr)+1;
	
	//volts: reg9~20
	for(i=0;i<12;i++)
	{
		regs[9+i] = ((uint8_t*)measured_volts)+i;
	}
	
	//currents: reg21~32 
	for(i=0;i<12;i++)
	{
		regs[21+i] = ((uint8_t*)measured_currents)+i;
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


void configs_save()
{
	int16_t buf[5];
	buf[0] = show_flag;
	buf[1] = volt_ratio;
	buf[2] = current_ratio;
	buf[3] = baud_index;
	buf[4] = com_addr;
	
	MemWriteByte(buf,5);
	modbus_init(BAUD_TAB[baud_index]);
}

static void configs_read(int16_t* buf)
{
	MemReadByte(buf,5);
}

void load_configs()
{
	int16_t config_paras[5];
	configs_read(config_paras);
	
	show_flag = config_paras[0];
	volt_ratio = config_paras[1];
	current_ratio = config_paras[2];
	baud_index = config_paras[3];
	com_addr = config_paras[4];
	
	modbus_init(BAUD_TAB[baud_index]);
}
