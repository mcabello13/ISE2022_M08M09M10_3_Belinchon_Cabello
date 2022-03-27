#include "cmsis_os.h"  
#include "rl_net.h"                     
#include "lcd.h"
#include "time.h"
#include "rtc.h"
#include "sntp.h"
#include "time.h"
#include "lpc17xx_rtc.h" 

extern void get_time_SNTP (void);
extern osThreadId tid_ThreadLED3; 
void time_callback_SNTP (uint32_t seconds);

char frase [20];
char tempo[24];
char t_SNTP[20];
//const uint8_t ntp_server[4] = {130,206,3,166};
struct tm tiempo_SNTP;
uint32_t s = 0;

void get_time_SNTP (void) 
{
	if (sntp_get_time(NULL, time_callback_SNTP) == netOK) 
	{
		//sprintf(frase,"SNTP request sent.\n");		
	}
	else 
	{    
		//sprintf(frase,"SNTP not ready or bad parameters.\n");
	}	
}
 
void time_callback_SNTP (uint32_t seconds) 
{	
	s = seconds;
	
  if (s == 0) 
	{
 		//sprintf(tempo,"%-20s", "Error");
    //EscribeFraseL1(tempo);
  }
  else 
	{		
		tiempo_SNTP = *localtime(&s);	
		//RTC_setTime_Date();		
  }
}
