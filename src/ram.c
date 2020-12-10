/**
 ***************************************************************************************************
 * @file        ram.c
 * @brief       global variables
 ***************************************************************************************************
 **************************************************************************************************/
#include <stdint.h>
#include <string.h>
#include "smart_agri_service.h"
#include "common.h"

uint8_t device_id[8];

// BLE Command
uint8_t write_buf[20];
uint16_t write_len;
uint8_t indication_buf[BLE_INDICATION_DATA_LENGTH];

bool ble_connect_flag;
bool ble_indicate_ack;
bool ble_write_flag;

uint8_t indication_pattern;
uint8_t indication_fail_count;

ble_smart_agri_t stOptexDoorService;

// app system timer
bool sys_timer_flag[TIMER_NUM];
bool sys_timer_limit[TIMER_NUM];
uint16_t sys_timer_count[TIMER_NUM];
uint16_t active_mode_count_num;

// BLE ctrl
uint8_t identification_num[4] = "1234";

// Soil Moisture
uint8_t soil_moisture_check_count;
uint16_t soil_moisture_value[4];

// initialize RAM variables 
void ram_init(void) 
  {
    // BLE Command
    ble_indicate_ack = false;
    ble_write_flag = false;

    // Device Information
    uint32_t temp_device_id = NRF_FICR->DEVICEID[1];
    device_id[0] = (uint8_t)((0xFF000000 & temp_device_id) >> 24);
    device_id[1] = (uint8_t)((0x00FF0000 & temp_device_id) >> 16);
    device_id[2] = (uint8_t)((0x0000FF00 & temp_device_id) >> 8);
    device_id[3] = (uint8_t)(0x000000FF & temp_device_id);
    temp_device_id = NRF_FICR->DEVICEID[0];
    device_id[4] = (uint8_t)((0xFF000000 & temp_device_id) >> 24);
    device_id[5] = (uint8_t)((0x00FF0000 & temp_device_id) >> 16);
    device_id[6] = (uint8_t)((0x0000FF00 & temp_device_id) >> 8);
    device_id[7] = (uint8_t)(0x000000FF & temp_device_id);
}
