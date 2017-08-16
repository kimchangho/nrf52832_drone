#ifndef __BUTTON_H_
#define __BUTTON_H_

#include "main.h"

extern uint8_t device_state;

enum
{
	DEVICE_OFF = 0,
	DEVICE_ON
};

void button_init(void);

#endif
