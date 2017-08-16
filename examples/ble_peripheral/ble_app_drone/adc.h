#ifndef __ADC_H__
#define __ADC_H__

#include "nrf_drv_saadc.h"
#include "nrf_drv_timer.h"
#include "nrf_drv_ppi.h"
#include "main.h"

#define SAMPLES_IN_BUFFER 2

void saadc_init(void);
void saadc_sampling_event_init(void);
void saadc_sampling_event_enable(void);

uint16_t getValue(nrf_saadc_value_t adc_value); 
char maxNumber(uint8_t count);
void minNumber(uint8_t count);
uint8_t digitalGetRate(uint32_t millis);

extern nrf_saadc_value_t m_adc_value[2];
extern uint32_t millis;

#endif
