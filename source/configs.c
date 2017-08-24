#include "display.h"
#include "configs.h"
#define PAGE_ADDR 0x08010000

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
}
