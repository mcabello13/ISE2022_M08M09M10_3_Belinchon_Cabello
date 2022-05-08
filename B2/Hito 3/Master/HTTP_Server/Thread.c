/****************************
 *Alumnos:                  *
 * Eduardo Belinchon Vera.  *
 * Miguel Cabello Adrada.   *
 ****************************/
 
 //MASTER AGP.
 
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
extern uint8_t bufFlash[20]; 
extern bool LEDrun;
bool LEDaux;
uint8_t modoLeds;
IAP_STATUS_CODE status2;
uint8_t modoL;
uint32_t k = 0;
extern char cadenaReloj [20];
extern char cadenaFecha [20];
int m, n;
extern int tx_over_gain;
extern uint8_t envio;
extern uint8_t ganancia;
extern double overload;
extern bool intOverload;


//Flash:
#define FLASH_PROG_AREA_START 0x00078000       
#define FLASH_PROG_AREA_SIZE 0x7FF 
#define BUFF_SIZE 256

//I2C:
extern ARM_DRIVER_I2C            Driver_I2C2;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C2;
static volatile uint32_t I2C_Event;
uint32_t addr = SLAVE_I2C_ADDR;
int32_t statusI2C = 0, nlect = 0;
uint8_t cmd, buf[10], cmd_env, cmd_rec;

//Funciones:
extern int Init_Thread(void);
extern void Init_i2c(void);

//Definicion de Hilos:                                         
void ThreadLCD (void const *argument);                            
osThreadId tid_ThreadLCD;                                          
osThreadDef (ThreadLCD, osPriorityNormal, 1, 0);                  

void ThreadMasterI2C (void const *argument);                            
osThreadId tid_ThreadMasterI2C;                                          
osThreadDef (ThreadMasterI2C, osPriorityNormal, 1, 0);

void ThreadSNTP (void const *argument);                            
osThreadId tid_ThreadSNTP;                                          
osThreadDef (ThreadSNTP, osPriorityNormal, 1, 0); 

void ThreadFlash (void const *argument);                            
osThreadId tid_ThreadFlash;                                          
osThreadDef (ThreadFlash, osPriorityNormal, 1, 0); 

//Timers:
void obtenerTiempo (void const *argument); //Callback
osTimerId tid_obTiempo;
osTimerDef (obTiempo, obtenerTiempo);

/*void guardaFlash (void const *argument); //Callback
osTimerId tid_flash;
osTimerDef (flash, guardaFlash);*/

//Funcion que inicializa los hilos:
int Init_Thread (void) 
{
	//Hilos:
	tid_ThreadLCD = osThreadCreate (osThread(ThreadLCD), NULL);
  if (!tid_ThreadLCD) return(-1);
	
	tid_ThreadSNTP = osThreadCreate (osThread(ThreadSNTP), NULL);
  if (!tid_ThreadSNTP) return(-1);
	
	tid_ThreadFlash = osThreadCreate (osThread(ThreadFlash), NULL);
  if (!tid_ThreadFlash) return(-1);
	
	tid_ThreadMasterI2C = osThreadCreate (osThread(ThreadMasterI2C), NULL);
  if (!tid_ThreadMasterI2C) return(-1);
	
	//Timers:
	tid_obTiempo = osTimerCreate (osTimer(obTiempo), osTimerPeriodic, NULL);
  if (!tid_obTiempo) return(-1);
	
	/*tid_flash = osTimerCreate (osTimer(flash), osTimerPeriodic, NULL);
  if (!tid_flash) return(-1);*/
	
	osTimerStart(tid_obTiempo, 6000); //OJO, ESTA PUESTO PARA 6s, HABRIA QUE PONERLO CADA 3 MIN!!!!!!
	//osTimerStart(tid_flash, 4000);
	
	return 0;
}

//Timer que gestiona los 3 min:
void obtenerTiempo(void const *argument) 
{
	osSignalSet(tid_ThreadSNTP, lecturaHoraSNTP);
}

//Timer que guarda en la Flash cada 5 segundos:
/*void guardaFlash(void const *argument)
{
	osSignalSet(tid_ThreadFlash, guardarFechaHoraGanancia);
}*/

//Hilo que gestiona la escritura del tiempo en el LCD:
void ThreadLCD (void const *argument) 
{	
	while(1)
	{
		 osSignalWait(EscrituraLCD, osWaitForever);
		 
		 RTC_getTime_Date();
	}
}

/* I2C Signal Event function callback */
void I2C_SignalEvent (uint32_t event) 
{ 
  /* Save received events */
  I2C_Event |= event;
 
  /* Optionally, user can define specific actions for an event */
 
  if (event & ARM_I2C_EVENT_TRANSFER_INCOMPLETE) {
    /* Less data was transferred than requested */
  }
 
  if (event & ARM_I2C_EVENT_TRANSFER_DONE) {
    /* Transfer or receive is finished */
		
		if(tx_over_gain == 1){
			envio = ganancia;
					
		}else if(tx_over_gain == 2){
			envio = overload;					
		}
		else if(tx_over_gain == 3){			
			envio = intOverload;
		}
		
		osSignalSet (tid_ThreadMasterI2C, RECIBIDO);
		
  }
 
  if (event & ARM_I2C_EVENT_ADDRESS_NACK) {
    /* Slave address was not acknowledged */
  }
 
  if (event & ARM_I2C_EVENT_ARBITRATION_LOST) {
    /* Master lost bus arbitration */
  }
 
  if (event & ARM_I2C_EVENT_BUS_ERROR) {
    /* Invalid start/stop position detected */
  }
 
  if (event & ARM_I2C_EVENT_BUS_CLEAR) {
    /* Bus clear operation completed */
  }
 
  if (event & ARM_I2C_EVENT_GENERAL_CALL) {
    /* Slave was addressed with a general call address */
  }
 
  if (event & ARM_I2C_EVENT_SLAVE_RECEIVE) {
    /* Slave addressed as receiver but SlaveReceive operation is not started */
  }
 
  if (event & ARM_I2C_EVENT_SLAVE_TRANSMIT) {
    /* Slave addressed as transmitter but SlaveTransmit operation is not started */
  }
}

//Funcion que inicializa el I2C:
void Init_i2c(void)
{
	int32_t status;
	
  status = I2Cdrv->Initialize (I2C_SignalEvent);
  status = I2Cdrv->PowerControl (ARM_POWER_FULL);
  status = I2Cdrv->Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD);
  status = I2Cdrv->Control      (ARM_I2C_BUS_CLEAR, 0);	
}

//Hilo que gestiona los envios y ACK de la Ganancia y Overload mediante I2C del Master:
void ThreadMasterI2C (void const *argument) 
{
  while (1) 
	{				
		osDelay(3000);		
		
		statusI2C = I2Cdrv->MasterTransmit(addr, &envio, 1, true);
		osSignalWait (RECIBIDO, osWaitForever); 
		
		statusI2C = I2Cdrv->MasterReceive(addr, buf, 2, true);
		osSignalWait (RECIBIDO, osWaitForever); 	
		
		LPC_I2C2->I2CONSET |= (1<<3);		
				
		osThreadYield ();                                          
  }
}

//Hilo que gestiona la actualizacion del tiempo:
void ThreadSNTP (void const *argument) 
{	
	while(1)
	{			
		osSignalWait(lecturaHoraSNTP, osWaitForever);
		
		get_time_SNTP();	
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
		osSignalWait(guardarFechaHoraGanancia, osWaitForever);
		
		for(m=0; m<20; m++)
		{
			bufFlash[m] = cadenaFecha[m];
		}
		
		flash_prog_area_sec_start = GetSecNum(FLASH_PROG_AREA_START); //Preparado de Flash.
		flash_prog_area_sec_end =  GetSecNum(FLASH_PROG_AREA_START + FLASH_PROG_AREA_SIZE);
  
		status2 = EraseSector(flash_prog_area_sec_start, flash_prog_area_sec_end); //Borrado del sector.
		
		ptr = (uint8_t*)(FLASH_PROG_AREA_START); //Colocamos el puntero en el inicio de la direccion porque se escriben IP y MAC, no solo estado y modo.
		status2 = CopyRAM2Flash(ptr, bufFlash, IAP_WRITE_256);
		
		//Primero se copia la Fecha y a continuacion la hora. Siempre se queda SOLO con la ultima (se copia en el mismo sector siempre).
		osDelay(2000);
		
		for(m=0; m<20; m++)
		{
			bufFlash[m] = cadenaReloj[m];
		}
		
		flash_prog_area_sec_start = GetSecNum(FLASH_PROG_AREA_START); //Preparado de Flash.
		flash_prog_area_sec_end =  GetSecNum(FLASH_PROG_AREA_START + FLASH_PROG_AREA_SIZE);
  
		status2 = EraseSector(flash_prog_area_sec_start, flash_prog_area_sec_end); //Borrado del sector.
		
		ptr = (uint8_t*)(FLASH_PROG_AREA_START); //Colocamos el puntero en el inicio de la direccion porque se escriben IP y MAC, no solo estado y modo.
		status2 = CopyRAM2Flash(ptr, bufFlash, IAP_WRITE_256);
		
		if(status2 != CMD_SUCCESS)
		{   
			 if(status2 == SECTOR_NOT_BLANK)
			 {
				 
			 }
			 while(1); 
		}
	}
}

