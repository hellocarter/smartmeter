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
//�����ѹ
extern s_dotnum st_volt[3];
//�������
extern s_dotnum st_current[3];

//��ѹ���ߵ���
extern uint8_t show_flag;

//��ѹ�������
extern int16_t volt_ratio;
extern int16_t current_ratio;

//������
extern const uint8_t BAUD_NUM;
extern const uint16_t BAUD_TAB[];
extern int8_t baud_index;

//ͨѶ��ַ
extern int16_t com_addr;
extern int16_t ADDR_MAX;
