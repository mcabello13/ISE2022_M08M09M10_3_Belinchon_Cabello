/****************************
 *Alumnos:                  *
 * Eduardo Belinchon Vera.  *
 * Miguel Cabello Adrada.   *
 ****************************/

#include "cmsis_os.h"  
#include "rl_net.h"                     
#include "lcd.h"
#include "time.h"
#include "rtc.h"
#include "sntp.h"
#include "time.h"

//Funciones:
extern void get_time_SNTP (void);
extern osThreadId tid_ThreadLED3; 
void time_callback_SNTP (uint32_t seconds);

//Variables:
char frase [20];
char tempo[24];
char t_SNTP[20];
//const uint8_t ntp_server[4] = {130,206,3,166}; --> Otra forma de pasar la direccion IP del servidor.
struct tm tiempo_SNTP;
uint32_t s;

//Funcion que obtiene el la hora y fecha del servidor de internet:
void get_time_SNTP (void) 
{
	if (sntp_get_time(NULL, time_callback_SNTP) == netOK) 
	{
		//sprintf(frase,"SNTP request sent.\n");	
		osDelay(3000);
	}
	else 
	{    
		//sprintf(frase,"SNTP not ready or bad parameters.\n");
	}	
}

//Manejador de la funcion que obtiene el tiempo del servidor de internet:
void time_callback_SNTP (uint32_t seconds) 
{	
	s = seconds;
	osDelay(1000);
	
  if (s == 0) 
	{
 		//sprintf(tempo,"%-20s", "Error");
    //EscribeFraseL1(tempo);
  }
  else 
	{		
		tiempo_SNTP = *localtime(&s);	//Si el tiempo que se recibe es distinto de cero, se convierte...
		RTC_setTime_Date();					 //...para poder mostrarlo como HH:MM:SS y DIA/MES/AÑO. 
  }
}
