#include "stm32f10x.h"

#define ALARM_TYPE_NUM 7

//extern const float VOLT_THRESHOLD;
//extern const float CURRENT_THRESHOLD;

//电压或者电流,1电压，0电流
extern uint8_t show_flag;

//电压接法,0-三相四线,1-三相三线
extern int8_t volt_conn_type;

//电流接法,0-三相四线,1-三相三线
extern int8_t current_conn_type;

//电压电流变比
extern int16_t volt_ratio;
extern int16_t current_ratio;
extern int8_t baud_index;
extern int16_t com_addr;

extern int8_t io_in1;
extern int8_t io_in2;
extern int8_t io_out1;
extern int8_t io_out2;

extern int8_t alarm_type1;
extern int8_t alarm_type2;

extern int16_t alarm_value1;
extern int16_t alarm_value2;

extern uint16_t calibration;

extern const uint8_t BAUD_NUM;
extern const uint16_t BAUD_TAB[];
extern const int16_t ADDR_MAX;

extern float measured_currents[3];
extern float measured_volts[3];

extern const uint16_t REG_NUM;
extern uint8_t* regs[];

void configs_save(void);
void load_configs(void);

