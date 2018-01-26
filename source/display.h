#include "TM1629C.h"
#include "configs.h"

typedef struct menu
{
  //�ڲ�����
  uint16_t tick;
  //��˸flag
  uint8_t flag;
  //��˸λ
  uint8_t index;
  //��ʾ����
  void (*disp)(struct menu*);
  //��������
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

