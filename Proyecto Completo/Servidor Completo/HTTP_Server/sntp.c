#include "cmsis_os.h"  
#include "rl_net.h"                     
#include "lcd.h"
#include "time.h"
#include "rtc.h"
#include "sntp.h"
#include "time.h"
#include "lpc17xx_rtc.h" 

#define signal_led3 0x003

extern osThreadId tid_ThreadLED3; 
void time_callback_SNTP (uint32_t seconds);

char frase [20];
char tempo[24];
char t_SNTP[20];
const uint8_t ntp_server[4] = {193,145,15,15};
struct tm tiempo_SNTP;

void get_time_SNTP (void) 
{
	if (sntp_get_time(ntp_server, time_callback_SNTP) == netOK) 
	{
		sprintf(frase,"SNTP request sent.\n");		
	}
	else 
	{    
		sprintf(frase,"SNTP not ready or bad parameters.\n");
	}	
}
 
void time_callback_SNTP (uint32_t seconds) 
{	
	time_t karabanchel;
	
  if (seconds == 0) 
	{
 		sprintf(tempo,"%-20s", "Error");
    EscribeFraseL1(tempo);
  }
  else 
	{		
		karabanchel = seconds;
		tiempo_SNTP = *localtime(&karabanchel);
		RTC_setTime_Date();		
  }
}
