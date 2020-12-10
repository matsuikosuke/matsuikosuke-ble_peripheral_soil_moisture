/**
 ***************************************************************************************************
 * @file    cpu_set.c
 * @brief   CPU Setting
 ***************************************************************************************************
 **************************************************************************************************/
#define   CPU_SET_GLOBAL
#include  "common.h"


static void gpio_init(void);
static void wdt_init(void);

/**
 ***************************************************************************************************
 *	@brief			CPU Modules initialization function
 *	@details		Initialize CPU Modules
 **************************************************************************************************/
void cpu_init(void)
{
    //Timer initialize setting
    timers_init();

    //RAM initialize setting
    ram_init();
    
    //WDT initialize setting
    //wdt_init();

    // AD initialize setting
    saadc_sampling_event_init();
    saadc_init();
    saadc_sampling_event_enable();
}


 /**
 ***************************************************************************************************
 *	@brief			WDT initialization function
 *	@details		Initialize WDT Module
 **************************************************************************************************/
static void wdt_init(void)
{
    NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | ( WDT_CONFIG_SLEEP_Pause << WDT_CONFIG_SLEEP_Pos);
    NRF_WDT->CRV = 3 * 32768;								// 3sec
    NRF_WDT->RREN |= WDT_RREN_RR0_Msk;						//Enable reload register 0
    NRF_WDT->TASKS_START = 1;
}