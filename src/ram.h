/**
 ***************************************************************************************************
 * @file        ram.h
 * @brief       global variables
 ***************************************************************************************************
 **************************************************************************************************/
#include "smart_agri_service.h"

#include        "define.h"

extern uint8_t device_id[8];

// BLE Command
extern uint8_t write_buf[20];
extern uint16_t write_len;
extern uint8_t indication_buf[BLE_INDICATION_DATA_LENGTH];

extern bool ble_connect_flag;
extern bool ble_indicate_ack;
extern bool ble_write_flag;

extern uint8_t indication_pattern;
extern uint8_t indication_fail_count;

// app system timet
extern bool sys_timer_flag[TIMER_NUM];
extern bool sys_timer_limit[TIMER_NUM];
extern uint16_t sys_timer_count[TIMER_NUM];
extern uint16_t active_mode_count_num;


// BLE ctrl
extern uint8_t identification_num[4];

// Soil Moisture
extern uint8_t soil_moisture_check_count;
extern uint16_t soil_moisture_value[4];

// initialize RAM variables 
extern void ram_init(void);

// Test
extern uint8_t test_step;