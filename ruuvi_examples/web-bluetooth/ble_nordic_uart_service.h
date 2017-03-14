/* This file was generated by plugin 'Nordic Semiconductor nRF5x v.1.2.2' (BDS version 1.1.3135.0) */

#ifndef BLE_NORDIC_UART_SERVICE_H__
#define BLE_NORDIC_UART_SERVICE_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "app_util_bds.h"



/**@brief Nordic UART Service event type. */
typedef enum
{ 
    BLE_NORDIC_UART_SERVICE_TX_EVT_NOTIFICATION_ENABLED,  /**< TX value notification enabled event. */
    BLE_NORDIC_UART_SERVICE_TX_EVT_NOTIFICATION_DISABLED, /**< TX value notification disabled event. */
    BLE_NORDIC_UART_SERVICE_TX_EVT_WRITE, /**< TX write event. */
    BLE_NORDIC_UART_SERVICE_RX_EVT_NOTIFICATION_ENABLED,  /**< RX value notification enabled event. */
    BLE_NORDIC_UART_SERVICE_RX_EVT_NOTIFICATION_DISABLED, /**< RX value notification disabled event. */
    BLE_NORDIC_UART_SERVICE_RX_EVT_CCCD_WRITE, /**< RX CCCD write event. */
} ble_nordic_uart_service_evt_type_t;

// Forward declaration of the ble_nordic_uart_service_t type.
typedef struct ble_nordic_uart_service_s ble_nordic_uart_service_t;








/**@brief TX structure. */
typedef struct
{
    uint8_array_t tx;
} ble_nordic_uart_service_tx_t;
/**@brief RX structure. */
typedef struct
{
    uint8_array_t rx;
} ble_nordic_uart_service_rx_t;

/**@brief Nordic UART Service Service event. */
typedef struct
{
    ble_nordic_uart_service_evt_type_t evt_type;    /**< Type of event. */
    union {
        uint16_t cccd_value; /**< Holds decoded data in Notify and Indicate event handler. */
        ble_nordic_uart_service_tx_t tx; /**< Holds decoded data in Write event handler. */
    } params;
} ble_nordic_uart_service_evt_t;

/**@brief Nordic UART Service Service event handler type. */
typedef void (*ble_nordic_uart_service_evt_handler_t) (ble_nordic_uart_service_t * p_nordic_uart_service, ble_nordic_uart_service_evt_t * p_evt);

/**@brief Nordic UART Service Service init structure. This contains all options and data needed for initialization of the service */
typedef struct
{
    ble_nordic_uart_service_evt_handler_t     evt_handler; /**< Event handler to be called for handling events in the Nordic UART Service Service. */
    ble_nordic_uart_service_tx_t ble_nordic_uart_service_tx_initial_value; /**< If not NULL, initial value of the TX characteristic. */ 
    ble_nordic_uart_service_rx_t ble_nordic_uart_service_rx_initial_value; /**< If not NULL, initial value of the RX characteristic. */ 
} ble_nordic_uart_service_init_t;

/**@brief Nordic UART Service Service structure. This contains various status information for the service.*/
struct ble_nordic_uart_service_s
{
    ble_nordic_uart_service_evt_handler_t evt_handler; /**< Event handler to be called for handling events in the Nordic UART Service Service. */
    uint16_t service_handle; /**< Handle of Nordic UART Service Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t tx_handles; /**< Handles related to the TX characteristic. */
    ble_gatts_char_handles_t rx_handles; /**< Handles related to the RX characteristic. */
    uint16_t conn_handle; /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
};

/**@brief Function for initializing the Nordic UART Service.
 *
 * @param[out]  p_nordic_uart_service       Nordic UART Service Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_nordic_uart_service_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_nordic_uart_service_init(ble_nordic_uart_service_t * p_nordic_uart_service, const ble_nordic_uart_service_init_t * p_nordic_uart_service_init);

/**@brief Function for handling the Application's BLE Stack events.*/
void ble_nordic_uart_service_on_ble_evt(ble_nordic_uart_service_t * p_nordic_uart_service, ble_evt_t * p_ble_evt);

/**@brief Function for setting the RX.
 *
 * @details Sets a new value of the RX characteristic. The new value will be sent
 *          to the client the next time the client reads the RX characteristic.
 *          This function is only generated if the characteristic's Read property is not 'Excluded'.
 *
 * @param[in]   p_nordic_uart_service                 Nordic UART Service Service structure.
 * @param[in]   p_rx  New RX.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_nordic_uart_service_rx_set(ble_nordic_uart_service_t * p_nordic_uart_service, ble_nordic_uart_service_rx_t * p_rx);

/**@brief Function for sending the RX.
 *
 * @details The application calls this function after having performed a rx.
 *          The rx data is encoded and sent to the client.
 *          This function is only generated if the characteristic's Notify or Indicate property is not 'Excluded'.
 *
 * @param[in]   p_nordic_uart_service                    Nordic UART Service Service structure.
 * @param[in]   p_rx               New rx.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_nordic_uart_service_rx_send(ble_nordic_uart_service_t * p_nordic_uart_service, ble_nordic_uart_service_rx_t * p_rx);

#endif //_BLE_NORDIC_UART_SERVICE_H__
