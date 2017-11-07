#include "TM1629C.h"
#include "configs.h"

extern unsigned char KEY_LEFT;
extern unsigned char KEY_RIGHT;
extern unsigned char KEY_SET;
extern unsigned char KEY_ENTER;

void display_init(void);
//void display_voltmeter(uint16_t *volts,uint8_t *dots);
void display_getkeys(void);

void display_menu0(uint8_t is_volt);
void display_menu1(uint8_t is_volt);
void display_menu2(void);
void display_menu3(void);
void display_menu4(void);
void display_menu5(void);

void display_set_volts(float *volts);
void display_set_currents(float *currents);
