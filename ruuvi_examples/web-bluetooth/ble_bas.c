/* This file was generated by plugin 'Nordic Semiconductor nRF5x v.1.2.2' (BDS version 1.1.3135.0) */

#include "ble_bas.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "app_util_bds.h"

#define OPCODE_LENGTH 1 /**< Length of opcode inside Battery Service packet. */
#define HANDLE_LENGTH 2 /**< Length of handle inside Battery Service packet. */

/* TODO Consider changing the max values if encoded data for characteristic/descriptor is fixed length */ 
#define MAX_BATTERY_LEVEL_LEN (BLE_L2CAP_MTU_DEF - OPCODE_LENGTH - HANDLE_LENGTH) /**< Maximum size of a transmitted Battery Level. */ 
#define MAX_CHARACTERISTIC_PRESENTATION_FORMAT_LEN (BLE_L2CAP_MTU_DEF - OPCODE_LENGTH - HANDLE_LENGTH) /**< Maximum size of a transmitted Characteristic Presentation Format. */ 

/**@brief Function for encoding Battery Level.
 *
 * @param[in]   p_battery_level              Battery Level characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t battery_level_encode(ble_bas_battery_level_t * p_battery_level, uint8_t * encoded_buffer)
{
    uint8_t len = 0; 
    encoded_buffer[len++] = p_battery_level->level;
    return len;
}

/**@brief Function for encoding Format.
 *
 * @param[in]   p_format              Format structure to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t characteristic_presentation_format_format_encode(bas_characteristic_presentation_format_format_t * p_format, uint8_t * encoded_buffer)
{
    uint8_t format;
    format = p_format->format;
    encoded_buffer[0] = format;
    return 1; 
}

    
/**@brief Function for encoding Namespace.
 *
 * @param[in]   p_namespace              Namespace structure to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t characteristic_presentation_format_namespace_encode(bas_characteristic_presentation_format_namespace_t * p_namespace, uint8_t * encoded_buffer)
{
    uint8_t namespace;
    namespace = p_namespace->namespace;
    encoded_buffer[0] = namespace;
    return 1; 
}

    
/**@brief Function for encoding Characteristic Presentation Format.
 *
 * @param[in]   p_characteristic_presentation_format              Characteristic Presentation Format descriptor structure to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t characteristic_presentation_format_encode(ble_bas_characteristic_presentation_format_t * p_characteristic_presentation_format, uint8_t * encoded_buffer)
{
    uint8_t len = 0; 
    len += characteristic_presentation_format_format_encode(&p_characteristic_presentation_format->format, &encoded_buffer[len]); 
    encoded_buffer[len++] = p_characteristic_presentation_format->exponent;
    len += bds_uint16_encode(&p_characteristic_presentation_format->unit, &encoded_buffer[len]); 
    len += characteristic_presentation_format_namespace_encode(&p_characteristic_presentation_format->namespace, &encoded_buffer[len]); 
    len += bds_uint16_encode(&p_characteristic_presentation_format->description, &encoded_buffer[len]); 
    return len;
}
    

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_bas       Battery Service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_bas_t * p_bas, ble_evt_t * p_ble_evt)
{
    p_bas->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_bas       Battery Service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_bas_t * p_bas, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_bas->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_bas       Battery Service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_bas_t * p_bas, ble_gatts_evt_write_t * p_ble_evt)
{
    
    if(p_ble_evt->handle == p_bas->battery_level_handles.cccd_handle)
    {
        if(p_bas->evt_handler != NULL)
        {
            ble_bas_evt_t evt;
            evt.evt_type = BLE_BAS_BATTERY_LEVEL_EVT_CCCD_WRITE;
            bds_uint16_decode(p_ble_evt->len, p_ble_evt->data, &evt.params.cccd_value);
            p_bas->evt_handler(p_bas, &evt);
        }
    } 
}

/**@brief Authorize WRITE request event handler.
 *
 * @details Handles WRITE events from the BLE stack.
 *
 * @param[in]   p_sc_ctrlpt  SC Ctrlpt structure.
 * @param[in]   p_gatts_evt  GATTS Event received from the BLE stack.
 *
 */
static void on_rw_authorize_request(ble_bas_t * p_bas, ble_gatts_evt_t * p_gatts_evt)
{
    ble_gatts_evt_rw_authorize_request_t * p_auth_req = &p_gatts_evt->params.authorize_request;
    if (p_auth_req->type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
    {
        if (   (p_gatts_evt->params.authorize_request.request.write.op
                != BLE_GATTS_OP_PREP_WRITE_REQ)
            && (p_gatts_evt->params.authorize_request.request.write.op
                != BLE_GATTS_OP_EXEC_WRITE_REQ_NOW)
            && (p_gatts_evt->params.authorize_request.request.write.op
                != BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL)
           )
        {
        
        }
    }
}

/**@brief Function for handling BLE events.
 *
 * @param[in]   p_bas       Battery Service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
void ble_bas_on_ble_evt(ble_bas_t * p_bas, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_bas, p_ble_evt);
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_bas, p_ble_evt);
            break;
        case BLE_GATTS_EVT_WRITE:
            on_write(p_bas, &p_ble_evt->evt.gatts_evt.params.write);
            break;
         case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            on_rw_authorize_request(p_bas, &p_ble_evt->evt.gatts_evt);
            break;
        default:
            //No implementation needed.
            break;
    }
}

/**@brief Function for initializing the Battery Service. */
uint32_t ble_bas_init(ble_bas_t * p_bas, const ble_bas_init_t * p_bas_init)
{
    uint32_t err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_bas->evt_handler = p_bas_init->evt_handler;
    p_bas->conn_handle = BLE_CONN_HANDLE_INVALID;
    
    BLE_UUID_BLE_ASSIGN(ble_uuid, 0x180F);
        
    // Add service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_bas->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    } 

    // Add Battery Level characteristic
    ble_bas_battery_level_t battery_level_initial_value = p_bas_init->ble_bas_battery_level_initial_value; 
    p_bas->is_battery_level_notify_supported = p_bas_init->is_battery_level_notify_supported;

    uint8_t battery_level_encoded_value[MAX_BATTERY_LEVEL_LEN];
    ble_add_char_params_t add_battery_level_params;
    memset(&add_battery_level_params, 0, sizeof(add_battery_level_params));
    
    add_battery_level_params.uuid                = 0x2A19; 
    add_battery_level_params.max_len             = MAX_BATTERY_LEVEL_LEN;
    add_battery_level_params.init_len            = battery_level_encode(&battery_level_initial_value, battery_level_encoded_value);
    add_battery_level_params.p_init_value        = battery_level_encoded_value; 
    add_battery_level_params.char_props.notify   = p_bas->is_battery_level_notify_supported; 
    add_battery_level_params.char_props.read     = 1; 
    add_battery_level_params.read_access         = SEC_OPEN; 
    add_battery_level_params.cccd_write_access   = SEC_OPEN;
    // 1 for variable length and 0 for fixed length.
    add_battery_level_params.is_var_len          = 1; 

    // Add Characteristic Presentation Format descriptor 
    ble_gatts_char_pf_t characteristic_presentation_format;

    memset(&characteristic_presentation_format, 0, sizeof(characteristic_presentation_format));

    characteristic_presentation_format.format     = BLE_GATT_CPF_FORMAT_UINT64;   //See BLE_GATT_CPF_FORMATS in ble_gatt.h file for the list of possible values
    characteristic_presentation_format.desc       = BLE_GATT_CPF_NAMESPACE_BTSIG; //See BLE_GATT_CPF_NAMESPACES in ble_gatt.h file for the list of possible values
    characteristic_presentation_format.exponent   = -2;                           //Exponent for integer data types.
    characteristic_presentation_format.name_space = BLE_GATT_CPF_NAMESPACE_BTSIG; //See BLE_GATT_CPF_NAMESPACES in ble_gatt.h file for the list of possible values
    characteristic_presentation_format.unit       = 0x2702;                       //corresponds to mass (kilogram), see https://developer.bluetooth.org/gatt/units/Pages/default.aspx for the list of units.
    add_battery_level_params.p_presentation_format = &characteristic_presentation_format; 

    err_code = characteristic_add(p_bas->service_handle, &add_battery_level_params, &(p_bas->battery_level_handles));
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    } 

    return NRF_SUCCESS;
}

/**@brief Function for setting the Battery Level. */
uint32_t ble_bas_battery_level_set(ble_bas_t * p_bas, ble_bas_battery_level_t * p_battery_level)
{
    ble_gatts_value_t gatts_value;
    uint8_t encoded_value[MAX_BATTERY_LEVEL_LEN];

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = battery_level_encode(p_battery_level, encoded_value);
    gatts_value.offset  = 0;
    gatts_value.p_value = encoded_value;

    return sd_ble_gatts_value_set(p_bas->conn_handle, p_bas->battery_level_handles.value_handle, &gatts_value);
}

/**@brief Function for setting the Characteristic Presentation Format. */
uint32_t ble_bas_characteristic_presentation_format_set(ble_bas_t * p_bas, ble_bas_characteristic_presentation_format_t * p_characteristic_presentation_format)
{
    ble_gatts_value_t gatts_value;
    uint8_t encoded_value[MAX_CHARACTERISTIC_PRESENTATION_FORMAT_LEN];

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = characteristic_presentation_format_encode(p_characteristic_presentation_format, encoded_value);
    gatts_value.offset  = 0;
    gatts_value.p_value = encoded_value;

    return sd_ble_gatts_value_set(p_bas->conn_handle, p_bas->characteristic_presentation_format_handles.value_handle, &gatts_value);
} 
/**@brief Function for sending the Battery Level. */
uint32_t ble_bas_battery_level_send(ble_bas_t * p_bas, ble_bas_battery_level_t * p_battery_level)
{
    uint32_t err_code = NRF_SUCCESS;

    if (p_bas->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        ble_gatts_hvx_params_t hvx_params;
        uint8_t encoded_value[MAX_BATTERY_LEVEL_LEN];
        uint16_t hvx_len;

        // Initialize value struct.
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_len           = battery_level_encode(p_battery_level, encoded_value);
        hvx_params.handle = p_bas->battery_level_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.p_len  = &hvx_len;
        hvx_params.offset = 0;
        hvx_params.p_data = encoded_value;

        err_code = sd_ble_gatts_hvx(p_bas->conn_handle, &hvx_params);
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }

    return err_code;
}

