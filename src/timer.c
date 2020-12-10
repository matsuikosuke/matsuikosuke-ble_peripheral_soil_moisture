/**
 ***************************************************************************************************
 * @file    timer.c
 * @brief   TIMER Setting
 ***************************************************************************************************
 **************************************************************************************************/
#define   TIMER_GLOBAL
#include  "common.h"

/*--------------------------------------------------------------------------------------------------
 Static RAM
--------------------------------------------------------------------------------------------------*/
APP_TIMER_DEF(sys_timer_id);     //TimerID: for System
APP_TIMER_DEF(notify_timer_id);  //TimerID: for Notify Request
APP_TIMER_DEF(period_timer_id);  //TimerID: for Periodical transmission
APP_TIMER_DEF(pwm_timer_id);     //TimerID: for PWM Output

/*--------------------------------------------------------------------------------------------------
 Static Function
--------------------------------------------------------------------------------------------------*/
static void sys_timer_handler( void* p_context );     // System Timer Handler

/**
 ***************************************************************************************************
 *	@brief			Timer initialization function
 *	@details		Initialize timer module and create Timers
 **************************************************************************************************/
void timers_init(void)
{
    // Initialize timer module.
    ret_code_t err_code  = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create system timer.
    err_code = app_timer_create(&sys_timer_id, APP_TIMER_MODE_REPEATED, sys_timer_handler);
    APP_ERROR_CHECK(err_code);
}



/**
 ***************************************************************************************************
*	@brief			System timer starting function
*	@details		Start system timer
 **************************************************************************************************/
void system_timer_start(void)
{
  ret_code_t err_code;

  //Start application timers
  err_code = app_timer_start(sys_timer_id, MAIN_INTERVAL, NULL);
  APP_ERROR_CHECK(err_code);
}

/**
 ***************************************************************************************************
*	@brief			System timer stopping function
*	@details		Stop System timer
 **************************************************************************************************/
void system_timer_stop(void)
{
  ret_code_t err_code;

  //Stop application timers
  err_code = app_timer_stop(sys_timer_id);
  APP_ERROR_CHECK(err_code);
}

/**
 ***************************************************************************************************
 *	@brief			System Timer Handler
 *	@details		Perform System Timer Handler processing
 **************************************************************************************************/
static void sys_timer_handler( void* p_context )
{
  for(int i=0; i<TIMER_NUM; i++)
  {
    if(true == sys_timer_flag[i]) {
      sys_timer_count[i] += 1;
    } else  {      
      sys_timer_count[i] = 0;
    }
  }

  if(sys_timer_count[BLE_ON_TIMER] >= BLE_ON_LIMIT) {
    sys_timer_count[BLE_ON_TIMER] = 0;
    sys_timer_limit[BLE_ON_TIMER] = true;
  } 
  
  if(sys_timer_count[BLE_NOTIFY_TIMER] >= BLE_NOTIFY_LIMIT) {
    sys_timer_count[BLE_NOTIFY_TIMER] = 0;
    sys_timer_limit[BLE_NOTIFY_TIMER] = true;
  } 
    
  if(sys_timer_count[TEST_TIMER] >= TEST_LIMIT) {
    sys_timer_count[TEST_TIMER] = 0;
    sys_timer_limit[TEST_TIMER] = true;
  }  
   
  
  if(sys_timer_count[INDICATION_ACK_TIMER] >= INDICATION_ACK_LIMIT) {
    sys_timer_count[INDICATION_ACK_TIMER] = 0;
    sys_timer_limit[INDICATION_ACK_TIMER] = true;
  }
}


/***************************************************************************************************
 **************************************************************************************************/
