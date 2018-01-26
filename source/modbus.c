#include "UART.h"
#include "display.h"
#include "string.h"
#include "stdlib.h"

static const uint8_t RX_BUF_SIZE = 32;
static const uint8_t RX_TIMEOUT = 100;

static const unsigned short crc16_table[256] = {
		0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,     
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,     
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,      
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,     
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,       
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,     
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,     
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,     
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,     
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,        
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,     
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,     
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,     
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,     
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,        
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,     
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,     
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,     
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,     
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,        
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,     
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,     
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,     
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,     
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,       
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,     
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,     
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,     
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,     
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,       
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,     
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};
uint8_t modbus_rx_buf[32];

//��ʱʹ��64�ֽڻ���
uint8_t modbus_tx_buf[64];

static uint8_t rx_cnt = 0;
uint16_t modbus_timer;

const uint8_t FUN_CODE_TAB[] = {0x03,0x06,0x07,0x0f};

union u_float
{
	float f_val;
	uint8_t arr[4];
};

void modbus_init(const uint16_t baud_rate)
{
	Uart_Init(baud_rate);
}

void modbus_send()
{
//	Uart_Write(buf, size);
}
static void clear_timer()
{
	modbus_timer = 0;
}
static void clear_rx_buf()
{
	memset(modbus_rx_buf,0,RX_BUF_SIZE);
	rx_cnt = 0;
}

static void clear_tx_buf()
{
	memset(modbus_rx_buf,0,RX_BUF_SIZE);
	rx_cnt = 0;
}

static uint16_t calc_crc(unsigned char *q, char len)
{
	u8 crc_high = 0xFF;     
	u8 crc_low = 0xFF;     
	uint16_t index;       
	while( len-- )     
	{          
		index = crc_low ^ *( q++ );         
		crc_low = ( u8 )( crc_high ^ crc16_table[index]);    
		crc_high = crc16_table[index]>>8;    
	}     
	return ( uint16_t )( crc_high << 8 | crc_low );  
}

static uint8_t is_supported_code(uint8_t fun_code)
{
	uint8_t i;
	for (i=0;i<sizeof(FUN_CODE_TAB) / sizeof(FUN_CODE_TAB[0]);i++)
	{
		if (FUN_CODE_TAB[i] == fun_code)
		{
			return 1;
		}
	}
	return 0;
}

static void rx_process()
{
	uint16_t crc;
	uint8_t len;
	uint16_t start_addr;
	uint8_t i;
	uint8_t fun_code = modbus_rx_buf[1];
	
	rx_cnt = 0;
	
	//ֻʶ��03,06,07,0f������,ע7Ϊ�Ǳ�׼�����룬����ͨ��modbus��ȡregƫ��֮�ڵ��������
	if (!is_supported_code(fun_code ))
	{
		return;
	}
	
	//��������֡8���ֽڣ�2���ֽ�CRC
	crc = calc_crc(modbus_rx_buf, 6);
	if ((crc % 256 == modbus_rx_buf[7])&&(crc / 256 == modbus_rx_buf[6]))
	{
		if (fun_code == 0x03)
		{
			start_addr = modbus_rx_buf[2]*256 + modbus_rx_buf[3];
			//length in "byte"
			len = 2 * (modbus_rx_buf[4]*256 + modbus_rx_buf[5]);
			if (start_addr + len > REG_NUM - REG_OFFSET)
			{
				return;
			}
			
			modbus_tx_buf[0] = com_addr;
			modbus_tx_buf[1] = 0x03;
			modbus_tx_buf[2] = len/256;
			modbus_tx_buf[3] = len%256;
			for(i=0;i<len;i++)
			{
				modbus_tx_buf[4+i] = *(regs[start_addr+i + REG_OFFSET]);
			}
			crc = calc_crc(modbus_tx_buf, 4 + len);
			modbus_tx_buf[4+len] = crc/256;
			modbus_tx_buf[4+len+1] = crc%256;
			Uart_Write(modbus_tx_buf, 4+len+1+1);
		}
		//��ȡƫ��֮�ڲ���
		if (fun_code == 0x07)
		{
			start_addr = modbus_rx_buf[2]*256 + modbus_rx_buf[3];
			//length in "byte"
			len = 2 * (modbus_rx_buf[4]*256 + modbus_rx_buf[5]);
			if (start_addr + len > REG_OFFSET)
			{
				return;
			}
			
			modbus_tx_buf[0] = com_addr;
			modbus_tx_buf[1] = 0x07;
			modbus_tx_buf[2] = len/256;
			modbus_tx_buf[3] = len%256;
			for(i=0;i<len;i++)
			{
				modbus_tx_buf[4+i] = *(regs[start_addr+i]);
			}
			crc = calc_crc(modbus_tx_buf, 4 + len);
			modbus_tx_buf[4+len] = crc/256;
			modbus_tx_buf[4+len+1] = crc%256;
			Uart_Write(modbus_tx_buf, 4+len+1+1);
		}
		//remote DO
		if (fun_code == 0x0f)
		{
			uint16_t addr = modbus_rx_buf[2] * 256 + modbus_rx_buf[3];
			uint16_t value = modbus_rx_buf[4] * 256 + modbus_rx_buf[5];
			if (addr == 0x00)
			{
				io_out1 = value==0xff00?1:0;
			}
			if (addr == 0x01)
			{
				io_out2 = value==0xff00?1:0;
			}
			Uart_Write(modbus_rx_buf, 8);
		}
		//sample calibration
		if (fun_code == 0x06)
		{
			uint16_t addr = modbus_rx_buf[2] * 256 + modbus_rx_buf[3];
			uint16_t value = modbus_rx_buf[4] * 256 + modbus_rx_buf[5];
			if (addr < REG_OFFSET/2)
			{
				//У׼������Ҫ�޶���Χ
				if (addr < 6 && value < CALIBRATE_UPPER_LIMIT && value > CALIBRATE_LOWER_LIMIT)
				{
					if (addr < 3)
					{
						voltage_factor[addr] = value;
					}
					else
					{
						current_factor[addr-3] = value;
					}
				}
				configs_save();
			}
			Uart_Write(modbus_rx_buf, 8);
		}
	}
	
}
void USART1_IRQHandler(void)                             
{ 
  if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)    
  {
		uint8_t rx_data;
    USART_ClearITPendingBit(USART1,USART_IT_RXNE);  
		if (rx_cnt > 0 && modbus_timer > RX_TIMEOUT)
		{
			clear_rx_buf();
		}
		clear_timer();
		rx_data = USART_ReceiveData(USART1);  
    if (rx_cnt == 0 && rx_data != (com_addr&0xff))
		{
			return;
		}
    modbus_rx_buf[rx_cnt++] = rx_data;
		if (rx_cnt == 8)
		{
			rx_process();
		}
  }
} 