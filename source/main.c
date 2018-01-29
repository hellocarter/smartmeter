#include "stm32f10x.h"
#include "display.h"
#include "measure.h"
#include "configs.h"                
#include "modbus.h"
#include "openio.h"
#include "stdio.h"

uint32_t time=0;

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

void SysTick_Handler(void)
{
	char is_update;		
	is_update = measure_update();
	time++;	
	modbus_timer++;
}