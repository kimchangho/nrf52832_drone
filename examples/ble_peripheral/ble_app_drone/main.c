#include "main.h"

/**@brief Application main function.
 */
struct bno055_accel_t accel_xyz;
struct bno055_mag_t mag_xyz;
struct bno055_gyro_t gyro_xyz;
struct bno055_euler_t euler_hrp;
struct bno055_euler_double_t d_euler_hpr;
struct bno055_gravity_t gravity_xyz;

float temp_pressure_lbs = 0; 
float temp_temperature_lbs = 0; 
float temp_asl_lbs  = 0;


int main(void)
{
    uint32_t err_code;
    //bool erase_bonds;
		ble_stack_init();
    // Initialize.
    APP_TIMER_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, false);
		twi_init();
    uart_init();
		gpio_init();
	
		saadc_init();
		saadc_sampling_event_init();
    saadc_sampling_event_enable();
	
		nrf_bno055_init();
		lps25hInit();
		lps25hSetEnabled(true);
		//buttons_leds_init(&erase_bonds);
    gap_params_init();
    services_init();
    advertising_init();
    conn_params_init();
    printf("\r\nUART Start!!\r\n");
    err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
		button_init();
		//motor_init();
		
    APP_ERROR_CHECK(err_code);

    // Enter main loop.
    for (;;)
    {
      power_manage();
			
			//bno055_convert_double_euler_hpr_deg(&d_euler_hpr);
			//printf("%f\n", d_euler_hpr.h);
			//lps25hGetData(&temp_pressure_lbs, &temp_temperature_lbs, &temp_asl_lbs);
			//printf("%f\n", temp_asl_lbs);
			//nrf_delay_ms(10);
			if(device_state == DEVICE_ON)
			{
				nrf_gpio_pin_set(15);
			}
			else if(device_state == DEVICE_OFF)
			{
				nrf_gpio_pin_clear(15);
			}
			nrf_gpio_pin_toggle(31);
			nrf_delay_ms(500);
    }
}

void gpio_init()
{
	nrf_gpio_cfg_output(5);
	nrf_gpio_cfg_output(14);
	nrf_gpio_cfg_output(15);
	nrf_gpio_cfg_output(16);
	nrf_gpio_cfg_input(4, NRF_GPIO_PIN_NOPULL);
	//nrf_gpio_cfg_input(9, NRF_GPIO_PIN_PULLUP);
	nrf_gpio_cfg_output(9);
	//nrf_gpio_cfg(9, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
	nrf_gpio_cfg_output(31);
}

/**
 * @}
 */
