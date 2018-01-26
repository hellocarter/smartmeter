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

//page index
uint8_t disp_index=0;
uint8_t disp_index_last=0;

//total pages
const uint8_t DISP_NUM = 12;

void update_param(void);
uint8_t res = 0;
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
    menu_call();
		openio_input();
    openio_output();
	}
	return 0;
}

//update params by key events
void update_param()
{
  
}

void SysTick_Handler(void)
{
	char is_update;		
	is_update = measure_update();
	time++;	
	count_key++;
	modbus_timer++;
}