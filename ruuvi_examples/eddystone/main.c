/* Copyright (c) 2016 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

/**
 * Example application: Eddystone Beacon for RuuviTag.
 * Based on /examples/ble_peripheral/ble_app_eddystone in the nRF SDK .
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "bsp.h"
#include "boards.h"
#include "bsp_board_config.h"
#include "ble_gap.h"
#include "ble_conn_params.h"
#include "ble_advertising.h"
#include "softdevice_handler.h"
#include "app_uart.h"
#include "app_timer_appsh.h"
#include "es_app_config.h"
#include "app_scheduler.h"
#include "nrf_ble_escs.h"
#include "nrf_ble_es.h"
#include "fstorage.h"
#include "nrf_delay.h"
#include "nrf_ble_es.h"

#define NRF_LOG_MODULE_NAME "MAIN"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

#include "init.h"
#include "pin_interrupt.h"

#define DEAD_BEEF                   0xDEADBEEF       //!< Value used as error code on stack dump, can be used to identify stack location on stack unwind.

/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in]   line_num   Line number of the failing ASSERT call.
 * @param[in]   file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


static bool connectable = false;

/**@brief Function for handling Eddystone events.
 *
 * @param[in] evt Eddystone event to handle.
 */
static void on_es_evt(nrf_ble_es_evt_t evt)
{
    switch(evt)
    {
        case NRF_BLE_ES_EVT_ADVERTISEMENT_SENT:
            // non-connectable advertisement
            connectable = false;
            break;
        
        case NRF_BLE_ES_EVT_CONNECTABLE_ADV_STARTED:
            // connectable advertisement
            connectable = true;
            break;

        default:
            break;
    }
}

bool isConnectable(void)
{
    return connectable;
}

/**@brief Function for doing power management.
 * Turns green led on when device is connecteable.
 * Blink red led when not in sleep
 */
static void power_manage(void)
{
  if(!isConnectable()) { nrf_gpio_pin_set(LED_GREEN); }
  nrf_gpio_pin_set(LED_RED);
  uint32_t err_code = sd_app_evt_wait();
  APP_ERROR_CHECK(err_code);
  nrf_gpio_pin_clear(LED_RED);
  if(isConnectable()) nrf_gpio_pin_clear(LED_GREEN);
}





/**@brief Function for handling button events from app_button IRQ
 *
 * @param[in] pin_no        Pin of the button for which an event has occured
 * @param[in] button_action Press or Release
 */
ret_code_t button_press_handler(const ruuvi_standard_message_t message)
{ 
  nrf_ble_es_on_start_connectable_advertising();
  return NRF_SUCCESS;
}

/**
 *  @brief pull CS of sensors up to keep them powered off
 */
static void gpio_init()
{
    nrf_gpio_cfg_output	(SPIM0_SS_HUMI_PIN);
    nrf_gpio_pin_set(SPIM0_SS_HUMI_PIN);
    nrf_gpio_cfg_output	(SPIM0_SS_ACC_PIN);
    nrf_gpio_pin_set(SPIM0_SS_ACC_PIN);
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t err_code;

    // Initialize.
    init_log(); 
    
    err_code = init_ble(); 
    NRF_LOG_INFO("BLE init status: %d\r\n", err_code);
    nrf_delay_ms(10);

    err_code |= init_nfc();

    // Start interrupts.
    err_code |= pin_interrupt_init();
 
    // Initialize button.
    err_code |= pin_interrupt_enable(BSP_BUTTON_0, NRF_GPIOTE_POLARITY_HITOLO, button_press_handler);

    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
    APP_TIMER_APPSH_INIT(APP_TIMER_PRESCALER, APP_TIMER_OP_QUEUE_SIZE, true);
    APP_ERROR_CHECK(err_code);

    gpio_init();
    nrf_ble_es_init(on_es_evt);
    
    NRF_LOG_INFO("Start!\r\n");
    // Enter main loop.
    for (;; )
    {
        app_sched_execute();
        if (NRF_LOG_PROCESS() == false)
        {
            power_manage();
        }
    }
}
