/* This file was generated by plugin 'Nordic Semiconductor nRF5x v.1.2.2' (BDS version 1.1.3135.0) */

#include "ble_acceleration_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "app_util_bds.h"

#define OPCODE_LENGTH 1 /**< Length of opcode inside Acceleration service packet. */
#define HANDLE_LENGTH 2 /**< Length of handle inside Acceleration service packet. */

/* TODO Consider changing the max values if encoded data for characteristic/descriptor is fixed length */ 
#define MAX_IMU_DATA_LEN (BLE_L2CAP_MTU_DEF - OPCODE_LENGTH - HANDLE_LENGTH) /**< Maximum size of a transmitted IMU Data. */ 

/**@brief Function for encoding IMU Data.
 *
 * @param[in]   p_imu_data              IMU Data characteristic structure to be encoded.
 * @param[out]  p_encoded_buffer   Buffer where the encoded data will be written.
 *
 * @return      Size of encoded data.
 */
static uint8_t imu_data_encode(ble_acceleration_service_imu_data_t * p_imu_data, uint8_t * encoded_buffer)
{
    uint8_t len = 0; 
    len += bds_uint8_array_encode(&p_imu_data->imu_array, &encoded_buffer[len]); 
    return len;
}


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_acceleration_service       Acceleration service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_acceleration_service_t * p_acceleration_service, ble_evt_t * p_ble_evt)
{
    p_acceleration_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_acceleration_service       Acceleration service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_acceleration_service_t * p_acceleration_service, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_acceleration_service->conn_handle = BLE_CONN_HANDLE_INVALID;
}

/**@brief Function for handling the Write event.
 *
 * @param[in]   p_acceleration_service       Acceleration service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_write(ble_acceleration_service_t * p_acceleration_service, ble_gatts_evt_write_t * p_ble_evt)
{
    
    if(p_ble_evt->handle == p_acceleration_service->imu_data_handles.cccd_handle)
    {
        if(p_acceleration_service->evt_handler != NULL)
        {
            ble_acceleration_service_evt_t evt;
            evt.evt_type = BLE_ACCELERATION_SERVICE_IMU_DATA_EVT_CCCD_WRITE;
            bds_uint16_decode(p_ble_evt->len, p_ble_evt->data, &evt.params.cccd_value);
            p_acceleration_service->evt_handler(p_acceleration_service, &evt);
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
static void on_rw_authorize_request(ble_acceleration_service_t * p_acceleration_service, ble_gatts_evt_t * p_gatts_evt)
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
 * @param[in]   p_acceleration_service       Acceleration service Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
void ble_acceleration_service_on_ble_evt(ble_acceleration_service_t * p_acceleration_service, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_acceleration_service, p_ble_evt);
            break;
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_acceleration_service, p_ble_evt);
            break;
        case BLE_GATTS_EVT_WRITE:
            on_write(p_acceleration_service, &p_ble_evt->evt.gatts_evt.params.write);
            break;
         case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            on_rw_authorize_request(p_acceleration_service, &p_ble_evt->evt.gatts_evt);
            break;
        default:
            //No implementation needed.
            break;
    }
}

/**@brief Function for initializing the Acceleration service. */
uint32_t ble_acceleration_service_init(ble_acceleration_service_t * p_acceleration_service, const ble_acceleration_service_init_t * p_acceleration_service_init)
{
    uint32_t err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_acceleration_service->evt_handler = p_acceleration_service_init->evt_handler;
    p_acceleration_service->conn_handle = BLE_CONN_HANDLE_INVALID;
    
    BLE_UUID_BLE_ASSIGN(ble_uuid, 0xBBE4);
        
    // Add service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_acceleration_service->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    } 

    // Add IMU Data characteristic
    ble_acceleration_service_imu_data_t imu_data_initial_value = p_acceleration_service_init->ble_acceleration_service_imu_data_initial_value; 

    uint8_t imu_data_encoded_value[MAX_IMU_DATA_LEN];
    ble_add_char_params_t add_imu_data_params;
    memset(&add_imu_data_params, 0, sizeof(add_imu_data_params));
    
    add_imu_data_params.uuid                = 0xE153;
    add_imu_data_params.uuid_type           = ble_uuid.type; 
    add_imu_data_params.max_len             = MAX_IMU_DATA_LEN;
    add_imu_data_params.init_len            = imu_data_encode(&imu_data_initial_value, imu_data_encoded_value);
    add_imu_data_params.p_init_value        = imu_data_encoded_value; 
    add_imu_data_params.char_props.notify   = 1; 
    add_imu_data_params.char_props.read     = 1; 
    add_imu_data_params.read_access         = SEC_OPEN; 
    add_imu_data_params.cccd_write_access   = SEC_OPEN;
    // 1 for variable length and 0 for fixed length.
    add_imu_data_params.is_var_len          = 1; 

    err_code = characteristic_add(p_acceleration_service->service_handle, &add_imu_data_params, &(p_acceleration_service->imu_data_handles));
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    } 

    return NRF_SUCCESS;
}

/**@brief Function for setting the IMU Data. */
uint32_t ble_acceleration_service_imu_data_set(ble_acceleration_service_t * p_acceleration_service, ble_acceleration_service_imu_data_t * p_imu_data)
{
    ble_gatts_value_t gatts_value;
    uint8_t encoded_value[MAX_IMU_DATA_LEN];

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = imu_data_encode(p_imu_data, encoded_value);
    gatts_value.offset  = 0;
    gatts_value.p_value = encoded_value;

    return sd_ble_gatts_value_set(p_acceleration_service->conn_handle, p_acceleration_service->imu_data_handles.value_handle, &gatts_value);
}

/**@brief Function for sending the IMU Data. */
uint32_t ble_acceleration_service_imu_data_send(ble_acceleration_service_t * p_acceleration_service, ble_acceleration_service_imu_data_t * p_imu_data)
{
    uint32_t err_code = NRF_SUCCESS;

    if (p_acceleration_service->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        ble_gatts_hvx_params_t hvx_params;
        uint8_t encoded_value[MAX_IMU_DATA_LEN];
        uint16_t hvx_len;

        // Initialize value struct.
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_len           = imu_data_encode(p_imu_data, encoded_value);
        hvx_params.handle = p_acceleration_service->imu_data_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.p_len  = &hvx_len;
        hvx_params.offset = 0;
        hvx_params.p_data = encoded_value;

        err_code = sd_ble_gatts_hvx(p_acceleration_service->conn_handle, &hvx_params);
    }
    else
    {
        err_code = NRF_ERROR_INVALID_STATE;
    }

    return err_code;
}

