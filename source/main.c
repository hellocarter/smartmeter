#include "stm32f10x.h"
#include "display.h"
#include "measure.h"
#include "configs.h"                
#include "modbus.h"
#include "openio.h"
#include "stdio.h"

uint8_t keys[4]={0};
uint32_t time=0;
uint16_t count_key=0;
uint8_t fast_key=0;

const uint16_t FAST_KEY_DELAY = 250;

//页面序号
uint8_t disp_index=0;
uint8_t disp_index_last=0;

//总页面数量
const uint8_t DISP_NUM = 7;

//function array for display
void (*disp_fun[DISP_NUM])(void) = {display_show_measure, display_volt_conn,
display_current_conn, display_voltage_ratio, display_current_ratio, display_baudrate, display_com_addr};

void update_param(void);

int main(){
	
	SysTick_Config(SystemCoreClock  / 5000);	
	
	//delay for device init
	while(time<500);

	display_init();
	measure_init();
	load_configs();
	map_register();
	openio_init();
	
	while(1){
		
		display_getkeys();
		if(KEY_SET)
		{
			while(KEY_SET)
			{
				display_getkeys();
			}
			disp_index++;
			display_clear();
			if(disp_index == DISP_NUM)
			{
				disp_index=0;
			}			
		}
		
		//根据显示页面序号调用显示函数
		disp_fun[disp_index]();
		
		//菜单循环后自动保存设置
		if(disp_index_last != 0 && disp_index == 0)
		{
			configs_save();
		}
		
		
		update_param();
		if(KEY_ENTER)
		{
			while(KEY_ENTER)
			{
				display_getkeys();
			}
			configs_save();
			disp_index=0;
		}

		if(!(KEY_SET||KEY_ENTER||KEY_LEFT||KEY_RIGHT))
		{
			fast_key=0;
		}
		disp_index_last = disp_index;
		openio_input();
    openio_output();
	}
	return 0;
}

void update_param()
{
	//set volt or current with left and right
	if (disp_fun[disp_index] == display_show_measure)
	{
		if(KEY_LEFT||KEY_RIGHT){
			if(KEY_LEFT || KEY_RIGHT){
				if(show_flag){
					show_flag=0;
				}
				else{
					show_flag=1;
				}
			}
			count_key=0;
			while(KEY_LEFT||KEY_RIGHT){
				display_getkeys();
				if(fast_key&&count_key>FAST_KEY_DELAY){
					break;
				}
				if(count_key>FAST_KEY_DELAY*10){
					fast_key=1;
					break;
				}
			}
		}
	}
	
	//设置电压接法
	if(disp_fun[disp_index] == display_volt_conn){
		if(KEY_LEFT||KEY_RIGHT){
			if(KEY_LEFT || KEY_RIGHT){
				if(volt_conn_type){
					volt_conn_type=0;
				}
				else{
					volt_conn_type=1;
				}
			}
			count_key=0;
			while(KEY_LEFT||KEY_RIGHT){
				display_getkeys();
				if(fast_key&&count_key>FAST_KEY_DELAY){
					break;
				}
				if(count_key>FAST_KEY_DELAY*10){
					fast_key=1;
					break;
				}
			}
		}
	}
	
	//set current connect type
	if(disp_fun[disp_index] == display_current_conn){
		if(KEY_LEFT||KEY_RIGHT){
			if(KEY_LEFT || KEY_RIGHT){
				if(current_conn_type){
					current_conn_type=0;
				}
				else{
					current_conn_type=1;
				}
			}
			count_key=0;
			while(KEY_LEFT||KEY_RIGHT){
				display_getkeys();
				if(fast_key&&count_key>FAST_KEY_DELAY){
					break;
				}
				if(count_key>FAST_KEY_DELAY*10){
					fast_key=1;
					break;
				}
			}
		}
	}
	
	//set voltage ratio
	if(disp_fun[disp_index] == display_voltage_ratio){
		if(KEY_LEFT||KEY_RIGHT){
			if(KEY_LEFT){
				volt_ratio++;
				if(volt_ratio>9999){
					volt_ratio=0;
				}
			}
			if(KEY_RIGHT){
				volt_ratio--;
				if(volt_ratio<0){
					volt_ratio=9999;
				}
			}
			count_key=0;
			while(KEY_LEFT||KEY_RIGHT){
				display_getkeys();
				if(fast_key&&count_key>FAST_KEY_DELAY){
					break;
				}
				if(count_key>FAST_KEY_DELAY*10){
					fast_key=1;
					break;
				}
			}
		}
	}
	
	//set current ratio
	if(disp_fun[disp_index] == display_current_ratio){
		if(KEY_LEFT||KEY_RIGHT){
			if(KEY_LEFT){
				current_ratio++;
				if(current_ratio>9999){
					current_ratio=0;
				}
			}
			if(KEY_RIGHT){
				current_ratio--;
				if(current_ratio<0){
					current_ratio=9999;
				}
			}
			count_key=0;
			while(KEY_LEFT||KEY_RIGHT){
				display_getkeys();
				if(fast_key&&count_key>FAST_KEY_DELAY){
					break;
				}
				if(count_key>FAST_KEY_DELAY*10){
					fast_key=1;
					break;
				}
			}
		}
	}

	//set baudrate
	if(disp_fun[disp_index] == display_baudrate){
		if(KEY_LEFT||KEY_RIGHT){
			if(KEY_LEFT){
				baud_index++;
				if(baud_index>=BAUD_NUM){
					baud_index=0;
				}
			}
			if(KEY_RIGHT){
				baud_index--;
				if(baud_index<0){
					baud_index=BAUD_NUM-1;
				}
			}
			while(KEY_LEFT||KEY_RIGHT){
				display_getkeys();
			}
		}
	}
	
	//set communicate address
	if(disp_fun[disp_index] == display_com_addr){
		if(KEY_LEFT||KEY_RIGHT){
			if(KEY_LEFT){
				com_addr++;
				if(com_addr>ADDR_MAX){
					com_addr=0;
				}
			}
			if(KEY_RIGHT){
				com_addr--;
				if(com_addr<0){
					com_addr=ADDR_MAX;
				}
			}
			count_key=0;
			while(KEY_LEFT||KEY_RIGHT){
				display_getkeys();
				if(fast_key&&count_key>FAST_KEY_DELAY){
					break;
				}
				if(count_key>FAST_KEY_DELAY*10){
					fast_key=1;
					break;
				}
			}
		}
	}
}

void SysTick_Handler(void)
{
	char is_update;		
	is_update = measure_update();
	time++;	
	count_key++;
	modbus_timer++;
}