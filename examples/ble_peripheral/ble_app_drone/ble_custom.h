#ifndef __BLE_H_
#define __BLE_H_

#include "main.h"

#define APP_ADV_INTERVAL                64                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 40 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      180                                         /**< The advertising timeout (in units of seconds). */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(20, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (20 ms), Connection interval uses 1.25 ms units. */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(75, UNIT_1_25_MS)             /**< Maximum acceptable connection interval (75 ms), Connection interval uses 1.25 ms units. */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds), Supervision Timeout uses 10 ms units. */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000, APP_TIMER_PRESCALER)  /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000, APP_TIMER_PRESCALER) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

extern  ble_nus_t                        m_nus;                                      /**< Structure to identify the Nordic UART Service. */

void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name);
void gap_params_init(void);
void nus_data_handler(ble_nus_t * p_nus, uint8_t * p_data, uint16_t length);
void services_init(void);
void on_conn_params_evt(ble_conn_params_evt_t * p_evt);
void conn_params_error_handler(uint32_t nrf_error);
void conn_params_init(void);
void sleep_mode_enter(void);
void on_adv_evt(ble_adv_evt_t ble_adv_evt);
void on_ble_evt(ble_evt_t * p_ble_evt);
void ble_evt_dispatch(ble_evt_t * p_ble_evt);
void ble_stack_init(void);
void bsp_event_handler(bsp_event_t event);
void advertising_init(void);
void buttons_leds_init(bool * p_erase_bonds);
void power_manage(void);

#endif
