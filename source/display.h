#include "TM1629C.h"

extern unsigned char KEY_LEFT;
extern unsigned char KEY_RIGHT;
extern unsigned char KEY_SET;
extern unsigned char KEY_ENTER;

void display_init(void);
void display_voltmeter(uint16_t *volts,uint8_t *dots);
void display_getkeys(void);

void display_menu0(void);
void display_menu1(void);
void display_menu2(void);
void display_menu3(void);
void display_menu4(void);
void display_menu5(void);

typedef struct{
	uint16_t number;
	uint8_t dot;
}s_dotnum;
//三相电压
extern s_dotnum st_volt[3];
//三相电流
extern s_dotnum st_current[3];

//电压或者电流
extern uint8_t show_flag;

//电压电流变比
extern int16_t volt_ratio;
extern int16_t current_ratio;

//波特率
extern const uint8_t BAUD_NUM;
extern const uint16_t BAUD_TAB[];
extern int8_t baud_index;

//通讯地址
extern int16_t com_addr;
extern int16_t ADDR_MAX;
