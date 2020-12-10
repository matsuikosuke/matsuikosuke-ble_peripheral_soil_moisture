/*----------------------------------------------------------
  Debug
----------------------------------------------------------*/
#define TEST_MODE
#define TEST_WITHOUT_DFU


/*----------------------------------------------------------
  BLE COMMAND
----------------------------------------------------------*/
// order
#define BLE_COMID_BYTE (0)
#define BLE_COMDATA_BYTE (1)
#define BLE_CHECKSUM_BYTE (19)

// length
#define BLE_COMID_LENGTH (1)
#define BLE_COMDATA_LENGTH (18)
#define BLE_CHECKSUM_LENGTH (1)

// value
#define BLE_COMID_SOIL_MOISTURE 0x01
#define BLE_COMID_CHECKSUM_ERROR  0x80
#define BLE_COMID_MAX    (3)

// indication
#define MAX_INDICATION_RETRY 3
#define INDICATION_SOIL_MOISTURE   0x10

// max length
#define BLE_WRITE_DATA_LENGTH (20)
#define BLE_INDICATION_DATA_LENGTH (20)
#define BLE_NOTIFY_DATA_LENGTH (20)

// BLE_COMID_STATUS Response Data
#define STATUS_SUCCESS        0x00

// ACK PROCESS
#define RESET_ACK (0)
#define WAITING_FOR_ACK (1)



/*----------------------------------------------------------
  Timer
----------------------------------------------------------*/
// If all packets cannot be received within 1 second, an error event occurs
#define TIMER_NUM  (4)

#define BLE_ON_TIMER (0)
#if defined (TEST_MODE)
#define BLE_ON_LIMIT TM_3SEC
#else
#define BLE_ON_LIMIT TM_30MIN
#endif

#define BLE_NOTIFY_TIMER (1)
#define BLE_NOTIFY_LIMIT TM_10SEC

#define TEST_TIMER (2)
#define TEST_LIMIT TM_10SEC

#define INDICATION_ACK_TIMER (3)
#define INDICATION_ACK_LIMIT TM_10SEC


#define APP_TIMER_PRESCALER		( 0 ) // Value of the RTC1 PRESCALER register
//#define APP_TIMER_MAX_TIMERS	( 3+1 ) // Maximum number of simultaneously created timers
#define APP_TIMER_OP_QUEUE_SIZE	( 10 )  // Size of timer operation queues

#define TM_STOP ( 0xFFFF )  // Timer Stop
#define TM_TMOUT  ( 0 )     // Timeout
#define TM_100MS  ( 2 )     // 50ms software timebase  : 100ms	
#define TM_300MS  ( 6 )     // 50ms software timebase  : 300ms	
#define	TM_500MS  ( 10 )    // 50ms software timebase  : 500ms
#define	TM_600MS  ( 12 )    // 50ms software timebase  : 600msec
#define	TM_1SEC   ( 20 )    // 50ms software timebase  : 1sec		
#define	TM_1500MS ( 30 )    // 50ms software timebase  : 1.5sec	
#define	TM_2SEC   ( 40 )    // 50ms software timebase  : 2sec		
#define	TM_3SEC   ( 60 )    // 50ms software timebase  : 3sec		
#define	TM_4SEC   ( 80 )    // 50ms software timebase  : 4sec	
#define	TM_4400MS ( 88 )    // 50ms software timebase  : 4400msec	
#define	TM_5SEC   ( 100 )   // 50ms software timebase  : 5sec		
#define	TM_10SEC  ( 200 )   // 50ms software timebase  : 10sec		
#define	TM_30SEC  ( 600 )   // 50ms software timebase  : 30sec	
#define	TM_50SEC  ( 1000 )  // 50ms software timebase  : 50sec	
#define	TM_60SEC  ( 1200 )  // 50ms software timebase  : 60sec
#define	TM_3MIN   ( 3600 )  // 50ms software timebase  : 3min
#define	TM_5MIN   ( 6000 )  // 50ms software timebase  : 5min
#define	TM_10MIN   ( 12000 )  // 50ms software timebase  : 10min
#define	TM_20MIN   ( 24000 )  // 50ms software timebase  : 20min
#define	TM_30MIN   ( 36000 )  // 50ms software timebase  : 20min

#define MAIN_INTERVAL APP_TIMER_TICKS(50) // 50ms main interval
#define NOTIFY_INTERVAL APP_TIMER_TICKS(3000) // 3.0 seconds
#define PERIOD_INTERVAL APP_TIMER_TICKS(60000) // 60.0 seconds
#define PWM_INTERVAL APP_TIMER_TICKS(10) // 10ms main interval