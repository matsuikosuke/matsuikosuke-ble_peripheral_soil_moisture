/**
 ***************************************************************************************************
 * @file        common.h
 * @brief       ヘッダーファイル
 ***************************************************************************************************
 **************************************************************************************************/
#ifndef COMMON_H		/* COMMON_H */
#define COMMON_H

#include	<stdbool.h>

#include	"define.h"
#include	"if.h"
#include	"ram.h"


//app
#include	"app_util_platform.h"
#include	"app_timer.h"
#include	"app_error.h"

//nrf drivers
#include	"nrf_drv_pwm.h"
#include	"nrf_drv_clock.h"
#include	"nrf_drv_gpiote.h"
#include	"nrf_drv_spi.h"
#include        "nrf_drv_ppi.h"
#include        "nrf_drv_timer.h"
#include        "nrf_drv_twi.h"
#include	"nrf.h"
#include        "nrf_delay.h"
#include        "nrf_drv_saadc.h"

// modules
#include	"cpu_set.h"
#include	"timer.h"
#include	"ble_ctrl.h"
#include	"ad_ctrl.h"

// dfu
#include "ble_dfu.h"

#endif						/* COMMON_H */
/***************************************************************************************************
 **************************************************************************************************/
