#ifndef __UART_H_
#define __UART_H_

#include "main.h"

void uart_event_handle(app_uart_evt_t * p_event);
void uart_init(void);

#endif
