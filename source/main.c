#include "stm32f10x.h"
#include "display.h"
#include "measure.h"
#include "configs.h"                
#include "modbus.h"
#include "stdio.h"

uint8_t keys[4]={0};
uint32_t time=0;
uint16_t count_key=0;
uint8_t fast_key=0;

//һ���˵�flag
uint8_t menu_flag=0;
uint8_t menu_flag_last=0;
const uint8_t DISP_MAX=5;
void update_param(void);

int main(){
	
	SysTick_Config(SystemCoreClock  / 5000);	
	
	//delay for device init
	while(time<500);

	display_init();
	measure_init();
	load_configs();
	map_register();
	
	while(1){
		
		display_getkeys();
		if(KEY_SET)
		{
			while(KEY_SET)
			{
				display_getkeys();
			}
			menu_flag++;
			if(menu_flag>DISP_MAX)
			{
				menu_flag=0;
			}			
		}
		switch (menu_flag){
			case 0:
				display_menu0(show_flag);
				if(menu_flag_last!=0)
				{
					configs_save();
				}
				break;
			case 1:
				display_menu1(show_flag);
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
		if(menu_flag>0)
		{
			update_param();
			if(KEY_ENTER)
			{
				while(KEY_ENTER)
				{
					display_getkeys();
				}
				configs_save();
				menu_flag=0;
			}
		}
		if(!(KEY_SET||KEY_ENTER||KEY_LEFT||KEY_RIGHT))
		{
			fast_key=0;
		}
		menu_flag_last = menu_flag;
	}
	return 0;
}

void update_param()
{
	//������ʾ��ѹ�����
	if(menu_flag==1)
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
	//���õ�ѹ���
	if(menu_flag==2){
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
	//���õ������
	if(menu_flag==3){
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
	//���ò�����
	if(menu_flag==4){
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
	//����ͨѶ��ַ
	if(menu_flag==5){
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
//���²���ֵ����ʾֵ
void measure_disp()
{
	if(show_flag)
	{
		//display_set_volts(convert_volts);
	}
	else
	{
		//display_set_currents(convert_currents);
	}
	
}

void SysTick_Handler(void)
{
	char is_update;		
	is_update = measure_update();
	//if(is_update)
	//{
		//measure_disp();
	//}
	time++;	
	count_key++;
	modbus_timer++;
}