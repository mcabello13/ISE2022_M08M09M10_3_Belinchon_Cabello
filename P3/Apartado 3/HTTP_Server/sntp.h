#include "cmsis_os.h"   
#include "rl_net.h"                     
#include "lcd.h"
#include "time.h"
#include "RTC.h"

#define signal_led3 0x040
#define signal_center 0x00001

extern int Init_Thread_led3(void);
extern void Init_Timer_3min(void);

static void time_callback_SNTP (uint32_t seconds);
 
extern void get_time_SNTP (void);
extern void RTC_setTime_Date(void);
static void time_callback_SNTP (uint32_t seconds);
extern void get_time_SNTP (void); 
extern void get_time_SNTP (void);
extern struct tm devuelveTiempo(void);
