#include "cmsis_os.h"                                          
#include "GPIO_LPC17xx.h"
#include "lcd.h"
#include "rtc.h"

//Variables:
int valor = 1;

//Funciones:
extern int Init_Thread(void);

//Definicion de Hilos:                                         
void ThreadLCD (void const *argument);                            
osThreadId tid_ThreadLCD;                                          
osThreadDef (ThreadLCD, osPriorityNormal, 1, 0);                  

void ThreadLED4 (void const *argument);                            
osThreadId tid_ThreadLED4;                                          
osThreadDef (ThreadLED4, osPriorityNormal, 1, 0); 

//Timers:
void parpadeoLED4 (void const *argument); //Callback
osTimerId tid_parpLed4;
osTimerDef (parpLed4, parpadeoLED4);   

//Funcion que inicializa los hilos:
int Init_Thread (void) 
{
	tid_ThreadLCD = osThreadCreate (osThread(ThreadLCD), NULL);
  if (!tid_ThreadLCD) return(-1);
	
	tid_ThreadLED4 = osThreadCreate (osThread(ThreadLED4), NULL);
  if (!tid_ThreadLED4) return(-1);
	
	return 0;
}

//Timer que gestiona el parpadeo del LED 4:
void parpadeoLED4(void const *argument) 
{
  GPIO_PinWrite(PUERTO_LED, LED4, valor);
	valor =! valor;
}

//Hilo que gestiona la escritura en el LCD:
void ThreadLCD (void const *argument) 
{
	
	//osEvent evento;
	
	while(1)
	{
		 osSignalWait(EscrituraLCD, osWaitForever);
		
		 RTC_getTime_Date();
	}
}

//Hilo que gestiona el LED4:
void ThreadLED4 (void const *argument) 
{
	//osEvent evento;
	
	while(1)
	{
		 osSignalWait(AlarmaLed4, osWaitForever);
		
		GPIO_PinWrite (PUERTO_LED, LED4, 1 ); 
		osDelay(100);
		GPIO_PinWrite (PUERTO_LED, LED4, 0 ); 
		osDelay(100);
	}
}
































