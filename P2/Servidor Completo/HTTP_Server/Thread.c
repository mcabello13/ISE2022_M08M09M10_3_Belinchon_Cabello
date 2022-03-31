/****************************
 *Alumnos:                  *
 * Eduardo Belinchon Vera.  *
 * Miguel Cabello Adrada.   *
 ****************************/
 
#include "cmsis_os.h"                                          
#include "GPIO_LPC17xx.h"
#include "lcd.h"
#include "rtc.h"
#include "sntp.h"
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "LPC17xx.h"

//Variables:
int valor = 1;
int j=0;

//Funciones:
extern int Init_Thread(void);

//Definicion de Hilos:                                         
void ThreadLCD (void const *argument);                            
osThreadId tid_ThreadLCD;                                          
osThreadDef (ThreadLCD, osPriorityNormal, 1, 0);                  

void ThreadLED4 (void const *argument);                            
osThreadId tid_ThreadLED4;                                          
osThreadDef (ThreadLED4, osPriorityNormal, 1, 0); 

void ThreadLED3 (void const *argument);                            
osThreadId tid_ThreadLED3;                                          
osThreadDef (ThreadLED3, osPriorityNormal, 1, 0); 

void ThreadSNTP (void const *argument);                            
osThreadId tid_ThreadSNTP;                                          
osThreadDef (ThreadSNTP, osPriorityNormal, 1, 0); 

void ThreadPulsador (void const *argument);                            
osThreadId tid_ThreadPulsador;                                          
osThreadDef (ThreadPulsador, osPriorityNormal, 1, 0); 

//Timers:
void obtenerTiempo (void const *argument); //Callback
osTimerId tid_obTiempo;
osTimerDef (obTiempo, obtenerTiempo);

//Funcion que inicializa los hilos:
int Init_Thread (void) 
{
	//Hilos:
	tid_ThreadLCD = osThreadCreate (osThread(ThreadLCD), NULL);
  if (!tid_ThreadLCD) return(-1);
	
	tid_ThreadLED4 = osThreadCreate (osThread(ThreadLED4), NULL);
  if (!tid_ThreadLED4) return(-1);
	
	tid_ThreadLED3 = osThreadCreate (osThread(ThreadLED3), NULL);
  if (!tid_ThreadLED3) return(-1);
	
	tid_ThreadSNTP = osThreadCreate (osThread(ThreadSNTP), NULL);
  if (!tid_ThreadSNTP) return(-1);
	
	tid_ThreadPulsador = osThreadCreate (osThread(ThreadPulsador), NULL);
  if (!tid_ThreadPulsador) return(-1);
	
	//Timers:
	tid_obTiempo = osTimerCreate (osTimer(obTiempo), osTimerPeriodic, NULL);
  if (!tid_obTiempo) return(-1);
	
	osTimerStart(tid_obTiempo, 10000); //OJO, ESTA PUESTO PARA 10s, HAY QUE PONERLO PARA 3 MIN!!!!!!
	
	return 0;
}

//Timer que gestiona los 3 min:
void obtenerTiempo(void const *argument) 
{
	osSignalSet(tid_ThreadSNTP, lecturaHoraSNTP);
}

//Hilo que gestiona la escritura en el LCD:
void ThreadLCD (void const *argument) 
{	
	while(1)
	{
		 osSignalWait(EscrituraLCD, osWaitForever);
		 
		 RTC_getTime_Date();
	}
}

//Hilo que gestiona el LED4:
void ThreadLED4 (void const *argument) 
{	
	while(1)
	{			
		 osSignalWait(AlarmaLed4, osWaitForever);
		
		 for(j=0;j<5;j++) //Parpadeo de cinco segundos.
		 {
				GPIO_PinWrite (PUERTO_LED, LED4, 1 ); 
				osDelay(500);
				GPIO_PinWrite (PUERTO_LED, LED4, 0 ); 
				osDelay(500);
	   }
	}
}

//Hilo que gestiona el LED3:
void ThreadLED3 (void const *argument) 
{	
	while(1)
	{			
			osSignalWait(Led3, osWaitForever);

			GPIO_PinWrite (PUERTO_LED, LED3, 1 ); 
			osDelay(100);
			GPIO_PinWrite (PUERTO_LED, LED3, 0 ); 
			osDelay(100);
	}
}

//Hilo que gestiona la actualizacion del tiempo:
void ThreadSNTP (void const *argument) 
{	
	while(1)
	{			
		osSignalWait(lecturaHoraSNTP, osWaitForever);
		
		get_time_SNTP();
			
		osSignalSet(tid_ThreadLED3, Led3);	
	}
}

//Hilo que gestiona la pulsacion del Joystick:
void ThreadPulsador (void const *argument) 
{
	while(1)
	{
		osSignalWait(signal_center, osWaitForever);
	  
		RTC_JOY();
		osSignalSet(tid_ThreadLED3, Led3);	
	}
}
