#include "stm32f10x.h"

//电压或者电流,1电压，0电流
extern uint8_t show_flag;

//电压电流变比
extern int16_t volt_ratio;
extern int16_t current_ratio;
extern int8_t baud_index;
extern int16_t com_addr;
extern const uint8_t BAUD_NUM;
extern const uint16_t BAUD_TAB[];
extern const int16_t ADDR_MAX;

extern float measured_currents[3];
extern float measured_volts[3];

extern const uint16_t REG_MAX;
extern uint8_t* regs[];

void configs_save(void);
void load_configs(void);

