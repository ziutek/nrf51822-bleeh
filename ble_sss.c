/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the license.txt file.
 */

#include "ble_sss.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "segger_debugger\segger_RTT.h"



/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_sss       Speed Sensor Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_sss_t * p_sss, ble_evt_t * p_ble_evt)
{
    p_sss->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_sss       Speed Sensor Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_sss_t * p_sss, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_sss->conn_handle = BLE_CONN_HANDLE_INVALID;
}


void ble_sss_on_ble_evt(ble_sss_t * p_sss, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_sss, p_ble_evt);
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_sss, p_ble_evt);
            break;
				
        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for adding the Button characteristic.
 *
 */
static uint32_t speed_sensor_char_add(ble_sss_t * p_sss, const ble_sss_init_t * p_sss_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    
    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_sss->uuid_type;
    ble_uuid.uuid = sss_UUID_BUTTON_CHAR;
    
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(uint8_t);
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(p_sss->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_sss->button_char_handles);
}

uint32_t ble_sss_init(ble_sss_t * p_sss, const ble_sss_init_t * p_sss_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_sss->conn_handle       = BLE_CONN_HANDLE_INVALID;
    p_sss->led_write_handler = p_sss_init->led_write_handler;
    
    // Add service
    ble_uuid128_t base_uuid = sss_UUID_BASE;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_sss->uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    ble_uuid.type = p_sss->uuid_type;
    ble_uuid.uuid = sss_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_sss->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    
    err_code = speed_sensor_char_add(p_sss, p_sss_init);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
        return NRF_SUCCESS;
}

uint32_t ble_sss_on_button_change(ble_sss_t * p_sss, uint8_t button_state)			//This function notify on BLE the new value!
{
    ble_gatts_hvx_params_t params;
    //uint16_t len = sizeof(button_state);
		static uint8_t counter_to_send=0;					//new variable to test custom data sending
		++counter_to_send;												//increment the counter
		static uint16_t counter_to_send_len=sizeof(counter_to_send);	//lenght of the counter
    
    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;	//state here if you want notification or indication
    params.handle = p_sss->button_char_handles.value_handle;
    params.p_data = &counter_to_send;					//counter_to_send copied to hvx structure
    params.p_len = &counter_to_send_len;			//counter_to_send_len copied to hvx structure
    
    return sd_ble_gatts_hvx(p_sss->conn_handle, &params);	//issue the PDU
}

uint32_t ble_sss_on_speed_change(ble_sss_t * p_sss, uint8_t actual_speed)
{
	SEGGER_RTT_WriteString(0,"I'm on speed_change event. Now I'll tx speed data through BLE!!!\n\r");
	
	ble_gatts_hvx_params_t params;
	uint16_t actual_speed_len=sizeof(actual_speed);
	
	memset(&params, 0, sizeof(params));
	
	params.type = BLE_GATT_HVX_NOTIFICATION;	//state here if you want notification or indication
	params.handle = p_sss->button_char_handles.value_handle;
	params.p_data = &actual_speed;						//actual_speed copied to hvx structure
	params.p_len = &actual_speed_len;					//actual_speed_len copied to hvx structure
	
	return sd_ble_gatts_hvx(p_sss->conn_handle, &params);	//issue the PDU
}
