/**
 ***************************************************************************************************
 * @file        smart_agri_service.h
 * @brief       Set Test Service
 ***************************************************************************************************
 **************************************************************************************************/

#ifndef BLE_SMART_AGRI_H__
#define BLE_SMART_AGRI_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief   Macro for defining a ble_smart_agri instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_SMART_AGRI_DEF(_name)                                                                          \
static ble_smart_agri_t _name;                                                                            \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_SMART_AGRI_BLE_OBSERVER_PRIO,                                                     \
                     ble_smart_agri_on_ble_evt, &_name)

//BASE-UUID:27B6xxxx-0671-C476-B66E-E119A4F76DE2
#define SMART_AGRI_UUID_BASE {0xE2, 0x6D, 0xF7, 0xA4, 0x19, 0xE1, 0x6E, 0xB6, \
                              0x76, 0xC4, 0x71, 0x06, 0x00, 0x00, 0xB6, 0x27}
#define SMART_AGRI_UUID_SERVICE      0x8171
#define SOIL_MOISTURE_UUID_CHAR      0x8172  //Read
#define DEVICE_INFO_UUID_CHAR        0x8173  //Read
#define NOTIFY_UUID_CHAR             0x8174  //Notify
#define INDICATION_UUID_CHAR         0x8175  //Indicate
#define WRITE_UUID_CHAR              0x8176  //Write

#define	MAX_READ_NUM	20
#define	MAX_NOTIFY_NUM	20
#define	MAX_INDICATION_NUM	20
#define	MAX_WRITE_NUM           20
#define BLE_MAX_DATA_LEN (BLE_GATT_ATT_MTU_DEFAULT - 3) /**< Maximum length of data (in bytes) that can be transmitted to the peer by the Nordic UART service module. */

typedef enum
{
    BLE_SMART_AGRI_EVT_NOTIFICATION_ENABLED,  
    BLE_SMART_AGRI_EVT_NOTIFICATION_DISABLED  
} ble_smart_agri_evt_type_t;

typedef struct
{
    ble_smart_agri_evt_type_t evt_type; 
} ble_smart_agri_evt_t;

// Forward declaration of the ble_smart_agri_t type.
typedef struct ble_smart_agri_s ble_smart_agri_t;

typedef void (*ble_smart_agri_write_handler_t) (uint16_t conn_handle, ble_smart_agri_t * p_smart_agri, const uint8_t *receive_buff, uint8_t length);

/**
 ***************************************************************************************************
 *	@brief			SMART_AGRI Service init structure.
 *	@details		This structure contains all options and data needed for
 *                              initialization of the service.
 **************************************************************************************************/
typedef struct
{
    ble_smart_agri_write_handler_t smart_agri_write_handler; /**< Event handler to be called when the Characteristic is written. */
    //uint8_t *                   p_smart_agri_state;
} ble_smart_agri_init_t;

/**
 ***************************************************************************************************
 *	@brief			SMART_AGRI Service structure.
 *	@details		This structure contains various status information for the service.
 **************************************************************************************************/
struct ble_smart_agri_s
{
    uint16_t                    service_handle;      /**< Handle of SMART_AGRI Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t    soil_moisture_handles;  /**< Handles related to the Soil Moisture Characteristic. */
    ble_gatts_char_handles_t    command_res_handles; /**< Handles related to the CommandResponse Characteristic. */
    ble_gatts_char_handles_t    command_trans_handles;  /**< Handles related to the CommandTransmission Characteristic. */
    ble_gatts_char_handles_t    device_info_handles;  /**< Handles related to the DeviceInfo Characteristic. */
    uint8_t                     uuid_type;           /**< UUID type for the SMART_AGRI Service. */
    uint16_t                    conn_handle; 
    //uint8_t *                   p_smart_agri_state;
    ble_smart_agri_write_handler_t smart_agri_write_handler;   /**< Event handler to be called when the LED Characteristic is written. */
};

 /**
 ***************************************************************************************************
 *	@brief			Function for initializing the SMART_AGRI Service.
 *	@param[out] p_smart_agri
 *                SMART_AGRI Service structure. This structure must be supplied by
 *                the application. It is initialized by this function and will later
 *                be used to identify this particular service instance.
 *	@param[in] p_smart_agri_init
 *                Information needed to initialize the service.
 *	@retval NRF_SUCCESS 
 *                If the service was initialized successfully. Otherwise, an error code is returned.
 **************************************************************************************************/
uint32_t ble_smart_agri_init(ble_smart_agri_t * p_smart_agri, const ble_smart_agri_init_t * p_smart_agri_init);


/**@brief Function for handling the application's BLE stack events.
 *
 * @details This function handles all events from the BLE stack that are of interest to the LED Button Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  SMART_AGRI Service structure.
 */
void ble_smart_agri_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);


/**@brief Function for sending a button state notification.
 *
 * @param[in] p_smart_agri SMART_AGRI Service structure.
 * @param[in] data  Notification data.
 * @param[in] len  length of Notification data.
 *
 * @retval NRF_SUCCESS If the notification was sent successfully. Otherwise, an error code is returned.
 */
uint32_t ble_smart_agri_notify(ble_smart_agri_t * p_smart_agri, uint8_t *data, uint16_t len);
uint32_t ble_smart_agri_indication(ble_smart_agri_t * p_smart_agri, uint8_t *data, uint16_t len);
void ble_soil_moisture_monitoring(void);
void ble_soil_moisture_test(void);

#ifdef __cplusplus
}
#endif

#endif // BLE_SMART_AGRI_H__

/** @} */
