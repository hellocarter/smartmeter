#include "TM1629C.h"
#include "configs.h"

typedef struct menu
{
  //内部计数
  uint16_t tick;
  //闪烁flag
  uint8_t flag;
  //闪烁位
  uint8_t index;
  //显示函数
  void (*disp)(struct menu*);
  //按键处理
  void (*key)(struct menu*);
}str_menu;

extern unsigned char KEY_LEFT;
extern unsigned char KEY_RIGHT;
extern unsigned char KEY_SET;
extern unsigned char KEY_ENTER;

void display_init(void);

void display_getkeys(void);

void display_clear(void);

void display_set_volts(float *volts);
void display_set_currents(float *currents);

void menu_call(void);

