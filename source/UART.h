#include "stm32f10x.h"

void Uart_Init(const uint32_t baud_rate);
void Uart_Write(uint8_t *buf,uint8_t size);