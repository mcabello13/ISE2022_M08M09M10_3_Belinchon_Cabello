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
#include "lpc_types.h"
#include "lpc17xx_iap.h"
#include "debug_frmwrk.h"

//Variables:
int valor = 1;
int j = 0;
extern uint8_t bufFlash[12]; 
extern bool LEDrun;
bool LEDaux;
uint8_t modoLeds;
IAP_STATUS_CODE status2;
uint8_t modoL;
uint32_t k=0;

//Flash:
#define FLASH_PROG_AREA_START 0x00078000       
#define FLASH_PROG_AREA_SIZE 0x400 
#define FLASH_LED 0x0A
#define BUFF_SIZE 256

//Funciones:
extern int Init_Thread(void);

//Definicion de Hilos:                                         
void ThreadLCD (void const *argument);                            
osThreadId tid_ThreadLCD;                                          
osThreadDef (ThreadLCD, osPriorityNormal, 1, 0);                  

/*void ThreadLED4 (void const *argument);                            
osThreadId tid_ThreadLED4;                                          
osThreadDef (ThreadLED4, osPriorityNormal, 1, 0); 

void ThreadLED3 (void const *argument);                            
osThreadId tid_ThreadLED3;                                          
osThreadDef (ThreadLED3, osPriorityNormal, 1, 0); */

void ThreadSNTP (void const *argument);                            
osThreadId tid_ThreadSNTP;                                          
osThreadDef (ThreadSNTP, osPriorityNormal, 1, 0); 

void ThreadPulsador (void const *argument);                            
osThreadId tid_ThreadPulsador;                                          
osThreadDef (ThreadPulsador, osPriorityNormal, 1, 0); 

void ThreadFlash (void const *argument);                            
osThreadId tid_ThreadFlash;                                          
osThreadDef (ThreadFlash, osPriorityNormal, 1, 0); 

//Timers:
void obtenerTiempo (void const *argument); //Callback
osTimerId tid_obTiempo;
osTimerDef (obTiempo, obtenerTiempo);

void guardaFlash (void const *argument); //Callback
osTimerId tid_flash;
osTimerDef (flash, guardaFlash);

//Funcion que inicializa los hilos:
int Init_Thread (void) 
{
	//Hilos:
	tid_ThreadLCD = osThreadCreate (osThread(ThreadLCD), NULL);
  if (!tid_ThreadLCD) return(-1);
	
	/*tid_ThreadLED4 = osThreadCreate (osThread(ThreadLED4), NULL);
  if (!tid_ThreadLED4) return(-1);
	
	tid_ThreadLED3 = osThreadCreate (osThread(ThreadLED3), NULL);
  if (!tid_ThreadLED3) return(-1);*/
	
	tid_ThreadSNTP = osThreadCreate (osThread(ThreadSNTP), NULL);
  if (!tid_ThreadSNTP) return(-1);
	
	tid_ThreadPulsador = osThreadCreate (osThread(ThreadPulsador), NULL);
  if (!tid_ThreadPulsador) return(-1);
	
	tid_ThreadFlash = osThreadCreate (osThread(ThreadFlash), NULL);
  if (!tid_ThreadFlash) return(-1);
	
	//Timers:
	tid_obTiempo = osTimerCreate (osTimer(obTiempo), osTimerPeriodic, NULL);
  if (!tid_obTiempo) return(-1);
	
	tid_flash = osTimerCreate (osTimer(flash), osTimerPeriodic, NULL);
  if (!tid_flash) return(-1);
	
	osTimerStart(tid_obTiempo, 6000); //OJO, ESTA PUESTO PARA 10s, HAY QUE PONERLO PARA 3 MIN!!!!!!
	osTimerStart(tid_flash, 4000);
	
	return 0;
}

//Timer que gestiona los 3 min:
void obtenerTiempo(void const *argument) 
{
	osSignalSet(tid_ThreadSNTP, lecturaHoraSNTP);
}

//Timer que guarda en la Flash cada 5 segundos:
void guardaFlash(void const *argument)
{
	osSignalSet(tid_ThreadFlash, guardarLeds);
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
/*void ThreadLED4 (void const *argument) 
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
}*/

//Hilo que gestiona el LED3:
/*void ThreadLED3 (void const *argument) 
{	
	while(1)
	{			
			osSignalWait(Led3, osWaitForever);

			GPIO_PinWrite (PUERTO_LED, LED3, 1 ); 
			osDelay(100);
			GPIO_PinWrite (PUERTO_LED, LED3, 0 ); 
			osDelay(100);
	}
}*/

//Hilo que gestiona la actualizacion del tiempo:
void ThreadSNTP (void const *argument) 
{	
	while(1)
	{			
		osSignalWait(lecturaHoraSNTP, osWaitForever);
		
		get_time_SNTP();
			
		//osSignalSet(tid_ThreadLED3, Led3);	
	}
}

//Hilo que gestiona la pulsacion del Joystick:
void ThreadPulsador (void const *argument) 
{
	while(1)
	{
		osSignalWait(signal_center, osWaitForever);
	  
		RTC_JOY();
		//osSignalSet(tid_ThreadLED3, Led3);	
	}
}

//Hilo que gestiona el guardado del modo y del estado de los LED en la Flash:
void ThreadFlash (void const *argument) 
{
	uint32_t flash_prog_area_sec_start;
	uint32_t flash_prog_area_sec_end;
	uint8_t *ptr;
	
	while(1)
	{		
		osSignalWait(guardarLeds, osWaitForever);
		
		if(LEDrun == true) //Se comprueba si se esta en el modo manual o barrido.
		{
			modoL = 1; //Barrido.
			
		}else
		{
			modoL = 0; //Manual.
		}
		
		bufFlash[10] = modoLeds; //En la posicion 10, se guarda el ON y el OFF de los LED, ya que la otras 9 posiciones estan ocupadas por IP + MAC...
		bufFlash[11] = modoL; //...mientras que en la 11, se guarda el modo de funcionamiento de estos.
		
		flash_prog_area_sec_start = GetSecNum(FLASH_PROG_AREA_START); //Preparado de Flash.
		flash_prog_area_sec_end =  GetSecNum(FLASH_PROG_AREA_START + FLASH_PROG_AREA_SIZE);
  
		status2 = EraseSector(flash_prog_area_sec_start, flash_prog_area_sec_end); //Borrado del sector.
		
		ptr = (uint8_t*)(FLASH_PROG_AREA_START); //Colocamos el puntero en el inicio de la direccion porque se escriben IP y MAC, no solo estado y modo.
		status2 = CopyRAM2Flash(ptr, bufFlash, IAP_WRITE_1024);
		
		if(status2 != CMD_SUCCESS)
		{   
			 if(status2 == SECTOR_NOT_BLANK)
			 {
				 
			 }
			 while(1); 
		}
	}
}

