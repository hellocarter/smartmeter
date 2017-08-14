#include "stm32f10x.h"
#include "display.h"
#include "TM1629C.h"
uint8_t keys[4]={0};
int time=0;
uint16_t count_key=0;
uint8_t fast_key=0;
//一级菜单flag
uint8_t disp_flag=0;
const uint8_t DISP_MAX=5;
void update_param();
int main(){

	int i,n;
	uint16_t volts[3]={2210,2208,2209};
	uint8_t dots[3]={1,1,1};
	
	SysTick_Config(SystemCoreClock  / 1000);	
	display_init();
	
	while(1){
		display_getkeys();
		if(KEY_SET)
		{
			while(KEY_SET)
			{
				display_getkeys();
			}
			disp_flag++;
			if(disp_flag>DISP_MAX)
			{
				disp_flag=0;
			}			
		}
		switch (disp_flag){
			case 0:
				display_menu0();
				break;
			case 1:
				display_menu1();
				break;
			case 2:
				display_menu2();
				break;
			case 3:
				display_menu3();
				break;
			case 4:
				display_menu4();
				break;
			case 5:
				display_menu5();
				break;
		}
		if(disp_flag>0){
			update_param();
			if(KEY_ENTER){
				while(KEY_ENTER)
				{
					display_getkeys();
				}
				disp_flag=0;
			}
		}
		if(!(KEY_SET||KEY_ENTER||KEY_LEFT||KEY_RIGHT)){
			fast_key=0;
		}
		
	}
	return 0;
}
void update_param()
{
	//设置显示电压或电流
	if(disp_flag==1)
	{
		if(KEY_LEFT||KEY_RIGHT)
		{
			while(KEY_LEFT||KEY_RIGHT)
			{
				display_getkeys();
			}
			if(show_flag){
				show_flag=0;
			}else{
				show_flag=1;
			}
		}
	}
	//设置电压变比
	if(disp_flag==2){
		if(KEY_LEFT||KEY_RIGHT){
			
			if(KEY_LEFT){
				volt_ratio++;
				if(volt_ratio==10000){
					volt_ratio=0;
				}
			}
			if(KEY_RIGHT){
				volt_ratio--;
				if(volt_ratio==-1){
					volt_ratio=9999;
				}
			}
			count_key=0;
			while(KEY_LEFT||KEY_RIGHT){
				display_getkeys();
				if(fast_key&&count_key>50){
					break;
				}
				if(count_key>500){
					fast_key=1;
					break;
				}
			}
		}
	}
	//设置电流变比
	if(disp_flag==3){
		if(KEY_LEFT||KEY_RIGHT){
			
			if(KEY_LEFT){
				current_ratio++;
				if(current_ratio==10000){
					current_ratio=0;
				}
			}
			if(KEY_RIGHT){
				current_ratio--;
				if(current_ratio==-1){
					current_ratio=9999;
				}
			}
			count_key=0;
			while(KEY_LEFT||KEY_RIGHT){
				display_getkeys();
				if(fast_key&&count_key>50){
					break;
				}
				if(count_key>500){
					fast_key=1;
					break;
				}
			}
		}
	}
	//设置波特率
	if(disp_flag==4){
		if(KEY_LEFT||KEY_RIGHT){
			if(KEY_LEFT){
				baud_index++;
				if(baud_index==BAUD_NUM){
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
	//设置通讯地址
	if(disp_flag==5){
		if(KEY_LEFT||KEY_RIGHT){
			if(KEY_LEFT){
				com_addr++;
				if(com_addr==ADDR_MAX+1){
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
				if(fast_key&&count_key>50){
					break;
				}
				if(count_key>500){
					fast_key=1;
					break;
				}
			}
		}
	}
}
void SysTick_Handler(void){
	time++;	
	count_key++;
}