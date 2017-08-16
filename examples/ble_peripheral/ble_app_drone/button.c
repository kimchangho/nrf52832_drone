#include "button.h"

uint8_t device_state;

static int turn_on_time;
APP_TIMER_DEF(long_button);
bool first_button_release = false;
static void timer_long_button_handler(void * p_context)
{
	
	if(turn_on_time++ > 19) // 3-Second
	{
		if(device_state == DEVICE_OFF)
		{
			nrf_gpio_pin_set(5);
			device_state = DEVICE_ON;
			turn_on_time = 0;
		}
		if(device_state == DEVICE_ON && first_button_release == true)
		{
			nrf_gpio_pin_clear(5);
			device_state = DEVICE_OFF;
			turn_on_time = 0;
		}
	}
}
static void button_handler(uint8_t pin_no, uint8_t button_action)
{
	if(button_action == APP_BUTTON_ACTIVE_HIGH)
	{
			app_timer_start(long_button,  APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
	}
	else
	{
		turn_on_time = 0;
		if(device_state == DEVICE_ON)
		{
			first_button_release = true;
			app_timer_stop(long_button);
		}
		if(device_state == DEVICE_OFF)
		{
			app_timer_stop(long_button);
			first_button_release = false;
		}
	}
}

void button_init()
{
	static app_button_cfg_t app_button_config;
	
	app_button_config.pin_no = 4;
	app_button_config.active_state = APP_BUTTON_ACTIVE_HIGH;
	app_button_config.pull_cfg = NRF_GPIO_PIN_NOPULL;
	app_button_config.button_handler = button_handler;
	app_button_init(&app_button_config, 1, BUTTON_DETECTION_DELAY);
	
	app_button_enable();
	
	app_timer_create(&long_button, APP_TIMER_MODE_REPEATED, timer_long_button_handler);
	
	if(nrf_gpio_pin_read(4) == 1)
	{
		app_timer_start(long_button,  APP_TIMER_TICKS(100, APP_TIMER_PRESCALER), NULL);
	}
}
