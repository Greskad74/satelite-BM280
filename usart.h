#ifndef _usart
#define _usart
#include "stm32f10x.h"





void uart_dma_init(char *rx_buffer, uint16_t buffer_size);
void uart_send_char(char c);
void uart_send_string(const char *s);
void uart_send_double(double val);
#endif		