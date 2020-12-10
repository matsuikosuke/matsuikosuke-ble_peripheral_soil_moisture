/**
 ***************************************************************************************************
 * @file        smart_agri_service.c
 * @brief       Set Test Service
 ***************************************************************************************************
 **************************************************************************************************/
#include "sdk_common.h"
#if NRF_MODULE_ENABLED(BLE_SMART_AGRI)
#include "smart_agri_service.h"
#include "ble_srv_common.h"
#include  "common.h"

static uint8_t ack_flag;
static bool ble_command_receive( const uint8_t *data, uint16_t len);

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_smart_agri       SMART_AGRI Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_smart_agri_t * p_smart_agri, ble_evt_t const * p_ble_evt)
{
    p_smart_agri->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_smart_agri       Heart Rate Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_smart_agri_t * p_smart_agri, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_smart_agri->conn_handle = BLE_CONN_HANDLE_INVALID;
}


/**
 ***************************************************************************************************
 *	@brief			on read function
 *	@details		Function for handling the Read event.
 *      @param[in] p_smart_agri  Test Service structure.
 *      @param[in] p_ble_evt    Event received from the BLE stack.
 *      @param[in] data         Response data.
 **************************************************************************************************/
static void on_read(ble_smart_agri_t * p_smart_agri, ble_evt_t const * p_ble_evt, uint8_t* data, uint8_t data_size)
{
    uint32_t err_code;

    ble_gatts_rw_authorize_reply_params_t reply_params;

    memset(&reply_params, 0, sizeof(reply_params));
      
    reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
    reply_params.params.read.p_data    = data;
    reply_params.params.read.len       = data_size;//sizeof(&data);//
    reply_params.params.read.offset    = 0;
    reply_params.params.read.update    = 1;
    reply_params.params.read.gatt_status = NRF_SUCCESS;
      
    sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gap_evt.conn_handle, &reply_params);
}

/**
 ***************************************************************************************************
 *	@brief			on write function
 *	@details		Function for handling the Write event.
 *      @param[in] p_smart_agri  Test Service structure.
 *      @param[in] p_ble_evt    Event received from the BLE stack.
 **************************************************************************************************/
static void on_write(ble_smart_agri_t * p_smart_agri, ble_evt_t const * p_ble_evt)
{
    bool result;
    bool analysis_result;
    uint32_t err_code;
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	
    if ((p_evt_write->handle == p_smart_agri->command_trans_handles.value_handle)
        && (p_evt_write->len >= 1)
        && (p_smart_agri->smart_agri_write_handler != NULL))
    {
        p_smart_agri->smart_agri_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_smart_agri, &p_evt_write->data[0], p_evt_write->len);

        // Assert ble_write_flag if command is received successfully
        ble_write_flag = ble_command_receive(&p_evt_write->data[0], p_evt_write->len);
    }
}

/**
 ***************************************************************************************************
 *	@brief			on write function
 *	@details		Function for handling the Write event.
 *      @param[in] p_smart_agri  Test Service structure.
 *      @param[in] p_ble_evt    Event received from the BLE stack.
 **************************************************************************************************/
static void on_write_res(ble_smart_agri_t * p_smart_agri, ble_evt_t const * p_ble_evt)
{
    bool result;
    bool analysis_result;
    uint32_t err_code;
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.write;

    uint8_t handle=p_ble_evt->evt.gatts_evt.params.authorize_request.request.write.handle;
    ble_gatts_rw_authorize_reply_params_t auth_reply;
    memset(&auth_reply, 0, sizeof(auth_reply));
    auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
    auth_reply.params.write.offset    = 0;
    auth_reply.params.write.update    = 1;

    if(handle == p_smart_agri->command_trans_handles.value_handle
        && (p_evt_write->len >= 1)
        && (p_smart_agri->smart_agri_write_handler != NULL))
    {
        auth_reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
        //err_code=sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gatts_evt.conn_handle,&auth_reply);
        err_code=sd_ble_gatts_rw_authorize_reply(p_ble_evt->evt.gap_evt.conn_handle, &auth_reply);
        APP_ERROR_CHECK(err_code);

        p_smart_agri->smart_agri_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_smart_agri, &p_evt_write->data[0], p_evt_write->len);

        // Assert ble_write_flag if command is received successfully
        ble_write_flag = ble_command_receive(&p_evt_write->data[0], p_evt_write->len);
    }
}

/**
 ***************************************************************************************************
 *	@brief			on BLE EVENT function
 *	@details		Function for handling the BLE event.
 *      @param[in] p_ble_evt    Event received from the BLE stack.
 **************************************************************************************************/
void ble_smart_agri_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t err_code;
    ble_smart_agri_t * p_smart_agri = (ble_smart_agri_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_smart_agri, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_smart_agri, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_smart_agri, p_ble_evt);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            if ( p_ble_evt->evt.gatts_evt.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_READ  )
            {
                if (p_ble_evt->evt.gatts_evt.params.authorize_request.request.read.handle == p_smart_agri->device_info_handles.value_handle)
                {
                    on_read(p_smart_agri, p_ble_evt, &device_id[0], 8);
                }

                if (p_ble_evt->evt.gatts_evt.params.authorize_request.request.read.handle == p_smart_agri->soil_moisture_handles.value_handle)
                {                
                    uint8_t send_data[8];
                    for (int i = 0; i < 4; i++)
                    {
                        send_data[2*i + 0] = (uint8_t)((0xFF00 & soil_moisture_value[i]) >> 8);
                        send_data[2*i + 1] = (uint8_t)(0x00FF & soil_moisture_value[i]);
                    }
                    on_read(p_smart_agri, p_ble_evt, &send_data[0], 8);
                }
            }
            else if(p_ble_evt->evt.gatts_evt.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
            {
                on_write_res(p_smart_agri, p_ble_evt);
            }
            break;

        case BLE_GATTS_EVT_HVC:
            ble_indicate_ack = true;
            break;


        default:
            // No implementation needed.
            break;
    }
}

/**
 ***************************************************************************************************
 *	@brief			Read Characteristic Setting
 *	@details		Setting Read Characteristic
 *      @param[in] p_smart_agri  Service structure.
 **************************************************************************************************/
static uint32_t soil_moisture_char_add(ble_smart_agri_t * p_smart_agri, const ble_smart_agri_init_t * p_smart_agri_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 0;
    char_md.char_props.notify   = 0;
    char_md.char_props.indicate = 0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_smart_agri->uuid_type;
    ble_uuid.uuid = SOIL_MOISTURE_UUID_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 1;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
    //attr_md.read_perm.sm = 1;      // Security Mode 1 Level 1: Open link
    //attr_md.read_perm.lv = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_READ_NUM;
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_smart_agri->service_handle, 
                                               &char_md,
                                               &attr_char_value,
                                               &p_smart_agri->soil_moisture_handles);
}

/**
 ***************************************************************************************************
 *	@brief			
 *	@details		
 *      @param[in] p_smart_agri  
 **************************************************************************************************/
static uint32_t command_write_char_add(ble_smart_agri_t * p_smart_agri, const ble_smart_agri_init_t * p_smart_agri_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 0;
    char_md.char_props.write  = 1;
    char_md.char_props.notify = 0;
    char_md.char_props.indicate = 0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_smart_agri->uuid_type;
    ble_uuid.uuid = WRITE_UUID_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 1;  //Write with Response
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_WRITE_NUM;//sizeof(uint8_t);
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_smart_agri->service_handle, 
                                               &char_md,
                                               &attr_char_value,
                                               &p_smart_agri->command_trans_handles);
}

/**
 ***************************************************************************************************
 *	@brief			
 *	@details		
 *      @param[in] p_smart_agri  
 **************************************************************************************************/
static uint32_t command_notify_char_add(ble_smart_agri_t * p_smart_agri, const ble_smart_agri_init_t * p_smart_agri_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 0;
    char_md.char_props.write  = 0;
    char_md.char_props.notify = 1;
    char_md.char_props.indicate = 0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         =  &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_smart_agri->uuid_type;
    ble_uuid.uuid = NOTIFY_UUID_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_NOTIFY_NUM;//sizeof(uint8_t);
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_smart_agri->service_handle,                       
    &char_md,
    &attr_char_value,
    &p_smart_agri->command_res_handles);
}

/**
 ***************************************************************************************************
 *	@brief			CommandTransmission Characteristic Setting
 *	@details		Setting CommandTransmission Characteristic
 *      @param[in] p_smart_agri  Test Service structure.
 **************************************************************************************************/
static uint32_t command_indication_char_add(ble_smart_agri_t * p_smart_agri, const ble_smart_agri_init_t * p_smart_agri_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc       = BLE_GATTS_VLOC_STACK;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 0;
    char_md.char_props.write  = 0;
    char_md.char_props.notify = 0;
    char_md.char_props.indicate = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         =  &cccd_md;
    char_md.p_sccd_md         = NULL;

    ble_uuid.type = p_smart_agri->uuid_type;
    ble_uuid.uuid = INDICATION_UUID_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_INDICATION_NUM;//sizeof(uint8_t);
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_smart_agri->service_handle,                       
    &char_md,
    &attr_char_value,
    &p_smart_agri->command_res_handles);
}

/**
 ***************************************************************************************************
 *	@brief			DeviceInfo Characteristic Setting
 *	@details		Setting DeviceInfo Characteristic
 *      @param[in] p_smart_agri  Test Service structure.
 **************************************************************************************************/
static uint32_t device_info_char_add(ble_smart_agri_t * p_smart_agri, const ble_smart_agri_init_t * p_smart_agri_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&char_md, 0, sizeof(char_md));

    char_md.char_props.read   = 1;
    char_md.char_props.write  = 0;
    char_md.char_props.notify   = 0;
    char_md.char_props.indicate = 0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = NULL;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_smart_agri->uuid_type;
    ble_uuid.uuid = DEVICE_INFO_UUID_CHAR;

    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 1;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
    //attr_md.read_perm.sm = 1;      // Security Mode 1 Level 1: Open link
    //attr_md.read_perm.lv = 1;

    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid    = &ble_uuid;
    attr_char_value.p_attr_md = &attr_md;
    attr_char_value.init_len  = sizeof(uint8_t);
    attr_char_value.init_offs = 0;
    attr_char_value.max_len   = MAX_READ_NUM;
    attr_char_value.p_value   = NULL;

    return sd_ble_gatts_characteristic_add(p_smart_agri->service_handle, 
                                               &char_md,
                                               &attr_char_value,
                                               &p_smart_agri->device_info_handles);
}


/**
 ***************************************************************************************************
 *	@brief			Test Service initialization
 *	@details		Initializing Test Service
 *      @param[in] p_smart_agri  Test Service structure.
 **************************************************************************************************/
uint32_t ble_smart_agri_init(ble_smart_agri_t * p_smart_agri, const ble_smart_agri_init_t * p_smart_agri_init)
{
    uint32_t              err_code;
    ble_uuid_t            ble_uuid;
    ble_add_char_params_t add_char_params;

    // Initialize service structure.
    p_smart_agri->conn_handle       = BLE_CONN_HANDLE_INVALID;
    p_smart_agri->smart_agri_write_handler = p_smart_agri_init->smart_agri_write_handler;
    //p_smart_agri->p_smart_agri_state = p_smart_agri_init->p_smart_agri_state;

    // Add service.
    ble_uuid128_t base_uuid = {SMART_AGRI_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_smart_agri->uuid_type);
    VERIFY_SUCCESS(err_code);

    ble_uuid.type = p_smart_agri->uuid_type;
    ble_uuid.uuid = SMART_AGRI_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_smart_agri->service_handle);
    VERIFY_SUCCESS(err_code);

    // Add Read State characteristic
    err_code = soil_moisture_char_add(p_smart_agri, p_smart_agri_init);
    VERIFY_SUCCESS(err_code);

    // Add Notify characteristic
//    err_code = command_notify_char_add(p_smart_agri, p_smart_agri_init);
//    VERIFY_SUCCESS(err_code);

    // Add Indication characteristic
    err_code = command_indication_char_add(p_smart_agri, p_smart_agri_init);
    VERIFY_SUCCESS(err_code);

    // Add Write characteristic
    err_code = command_write_char_add(p_smart_agri, p_smart_agri_init);
    VERIFY_SUCCESS(err_code);

    // Add Device Information characteristic
    err_code = device_info_char_add(p_smart_agri, p_smart_agri_init);
    VERIFY_SUCCESS(err_code);

    return err_code;
}

/**
 ***************************************************************************************************
 *	@brief			Notify transmission function
 *	@details		transmission Notify
 *      @param[in] p_smart_agri  Service structure.
 **************************************************************************************************/
uint32_t ble_smart_agri_notify(ble_smart_agri_t * p_smart_agri, uint8_t *data, uint16_t len)
{
    ble_gatts_hvx_params_t params;

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_smart_agri->command_res_handles.value_handle;
    params.p_data = data;
    params.p_len = &len;

    return sd_ble_gatts_hvx(p_smart_agri->conn_handle, &params);
}


/**
 ***************************************************************************************************
 *	@brief			Indication transmission function
 *	@details		transmission Indication
 *      @param[in] p_smart_agri  Service structure.
 **************************************************************************************************/
uint32_t ble_smart_agri_indication(ble_smart_agri_t * p_smart_agri, uint8_t *data, uint16_t len)
{
    ble_gatts_hvx_params_t params;

    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_INDICATION;
    params.handle = p_smart_agri->command_res_handles.value_handle;
    params.p_data = data;
    params.p_len = &len;

    return sd_ble_gatts_hvx(p_smart_agri->conn_handle, &params);
}


/**
 ***************************************************************************************************
 *	@brief			BLE command Receive process
 *	@details		Checks the received data for errors, 
 *				stores the data in the command buffer, 
 *				and starts counting the data receive timer.
 **************************************************************************************************/ 
static bool ble_command_receive( const uint8_t *data, uint16_t len)
{
    // If the received data is less than 20 bytes, an error event occurs
    if(len != BLE_WRITE_DATA_LENGTH)
    {
        return false;
    }

    // Store received data in command buffer
    for(int i=0; i<BLE_WRITE_DATA_LENGTH; i++)
    {
        write_buf[i] = data[i];
    }

    return true;
}


/**
 ***************************************************************************************************
 *	@brief			BLE response process
 *	@details		Returns the response when the BLE command is received successfully
 **************************************************************************************************/ 
static void ble_notify_command( uint8_t cmd_id, uint8_t *cmd_data )
{
    // step1: Setting Indication data
    uint8_t notify_buf[BLE_INDICATION_DATA_LENGTH];

    notify_buf[BLE_COMID_BYTE] = cmd_id;
  
    for(int i=0; i<BLE_COMDATA_LENGTH; i++)
    {
      notify_buf[i+BLE_COMDATA_BYTE] = *cmd_data;
      cmd_data += 1;
    }

    // setting checksum
    uint8_t checksum = 0;
  
    for(int i=0; i<BLE_CHECKSUM_BYTE; i++)
    {
      checksum += notify_buf[i];
    }

    notify_buf[BLE_CHECKSUM_BYTE] = checksum;
  
    // step2: Transmit Notify
    notify_exe(&notify_buf[0], BLE_NOTIFY_DATA_LENGTH);
}


/**
 ***************************************************************************************************
 *	@brief			BLE response process
 *	@details		Returns the response when the BLE command is received successfully
 **************************************************************************************************/ 
static void ble_indication_command( uint8_t cmd_id, uint8_t *cmd_data )
{
    // step1: Setting Indication data
    uint8_t inidication_buf[BLE_INDICATION_DATA_LENGTH];

    inidication_buf[BLE_COMID_BYTE] = cmd_id;
  
    for(int i=0; i<BLE_COMDATA_LENGTH; i++)
    {
      inidication_buf[i+BLE_COMDATA_BYTE] = *cmd_data;
      cmd_data += 1;
    }

    // setting checksum
    uint8_t checksum = 0;
  
    for(int i=0; i<BLE_CHECKSUM_BYTE; i++)
    {
      checksum += inidication_buf[i];
    }

    inidication_buf[BLE_CHECKSUM_BYTE] = checksum;
  
    // step2: Transmit Notify
    indication_exe(&inidication_buf[0], BLE_INDICATION_DATA_LENGTH);
}

/**
 ***************************************************************************************************
 *	@brief			BLE response process
 *	@details		Returns the response when the BLE command is received successfully
 **************************************************************************************************/ 
static void ble_soil_moisture_status( void )
{
    uint8_t send_data[BLE_COMDATA_LENGTH];
    for (int i = 0; i < BLE_COMDATA_LENGTH; i++) {
        send_data[i] = 0x00;
    }
    
    send_data[BLE_COMID_BYTE] = BLE_COMID_SOIL_MOISTURE;

    for (int i = 0; i < 4; i++)
    {
        send_data[BLE_COMDATA_BYTE + 2*i + 0] = (uint8_t)((0xFF00 & soil_moisture_value[i]) >> 8);
        send_data[BLE_COMDATA_BYTE + 2*i + 1] = (uint8_t)(0x00FF & soil_moisture_value[i]);
    }

    ble_indication_command(BLE_COMID_SOIL_MOISTURE, &send_data[0]);
}


/**
 ***************************************************************************************************
 *	@brief			BLE response process
 *	@details		Returns the response when the BLE command is received successfully
 **************************************************************************************************/ 
static void ble_ack_reset(void)
{
    ble_indicate_ack = false;
    sys_timer_flag[INDICATION_ACK_TIMER] = false;
    sys_timer_count[INDICATION_ACK_TIMER] = 0x00;
}

/**
 ***************************************************************************************************
 *	@brief			
 *	@details		
 **************************************************************************************************/ 
static void ble_start (void)
{
    if(true == sys_timer_limit[BLE_ON_TIMER])
    {        
        sys_timer_limit[BLE_ON_TIMER] = false;
        advertising_start();
        ack_flag = 0;
    }
}

/**
 ***************************************************************************************************
 *	@brief			
 *	@details		
 **************************************************************************************************/
static void monitoring_ble_check(void)
{
    if(RESET_ACK != ack_flag)
    {
        if(true == ble_indicate_ack || WAITING_FOR_ACK+3 == ack_flag)
        {
            ble_ack_reset();
            ble_req_disconnect();
            advertising_stop();
            ack_flag = RESET_ACK;
        }

        if(true == sys_timer_limit[INDICATION_ACK_TIMER])
        {
            sys_timer_limit[INDICATION_ACK_TIMER] = false;
            ble_soil_moisture_status();
            ack_flag += 1;
        }
    }
}


/**
 ***************************************************************************************************
 *	@brief			
 *	@details		
 **************************************************************************************************/
static void test_ble_check(void)
{
    if(RESET_ACK != ack_flag)
    {
        if(true == ble_indicate_ack || WAITING_FOR_ACK+3 == ack_flag)
        {
            ble_ack_reset();
            ack_flag = RESET_ACK;
        }

        if(true == sys_timer_limit[INDICATION_ACK_TIMER])
        {
            sys_timer_limit[INDICATION_ACK_TIMER] = false;
            ble_soil_moisture_status();
            ack_flag += 1;
        }
    }
}

/**
 ***************************************************************************************************
 *	@brief			
 *	@details		
 **************************************************************************************************/
void ble_soil_moisture_monitoring(void)
{
    ble_start();

    if(true == ble_connect_flag && RESET_ACK == ack_flag)
    {
        ble_soil_moisture_status();
        ack_flag = WAITING_FOR_ACK;
    }

    monitoring_ble_check();
}

/**
 ***************************************************************************************************
 *	@brief			
 *	@details		
 **************************************************************************************************/
void ble_soil_moisture_test(void)
{
    if(true == sys_timer_limit[BLE_ON_TIMER])
    {        
        sys_timer_limit[BLE_ON_TIMER] = false;
        ble_soil_moisture_status();
        ack_flag = WAITING_FOR_ACK;
    }
    
    test_ble_check();
}

#endif // NRF_MODULE_ENABLED(BLE_SMART_AGRI)
