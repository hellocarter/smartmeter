#include "TM1629C.h"
#include "configs.h"

extern unsigned char KEY_LEFT;
extern unsigned char KEY_RIGHT;
extern unsigned char KEY_SET;
extern unsigned char KEY_ENTER;

void display_init(void);

void display_getkeys(void);

void display_clear(void);

void display_show_measure(void);
void display_show_voltage(void);
void display_show_current(void);

void display_volt_conn(void);
void display_current_conn(void);

void display_voltage_ratio(void);
void display_current_ratio(void);

void display_baudrate(void);
void display_com_addr(void);

void display_set_volts(float *volts);
void display_set_currents(float *currents);
