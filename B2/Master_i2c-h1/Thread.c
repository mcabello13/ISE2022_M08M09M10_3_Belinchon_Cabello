 /****************************
 *Alumnos:                  *
 * Eduardo Belinchon Vera.  *
 * Miguel Cabello Adrada.   *
 ****************************/
 
 //MASTER.
 
#include "cmsis_os.h"                                           
#include "Driver_I2C.h"
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "LPC17xx.h"

#define LM75B_I2C_ADDR       	0x28      
 
#define REG_TEMP    					0x00
#define REG_CONF	    				0x01
#define	REG_THYST   					0x02
#define REG_TOS     					0x03

#define RECIBIDO							0x0001

//RGB
#define PUERTO_RGB 2
#define AZUL 1	
#define VERDE 2  
#define ROJO 3 

uint32_t addr = LM75B_I2C_ADDR;
int32_t status = 0, nlect = 0;
uint8_t cmd, buf[10], cmd_env, cmd_rec;
uint16_t temp;
uint8_t contador = 0;
int valido = 0;

//Driver I2C
extern ARM_DRIVER_I2C            Driver_I2C2;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C2;

/************************************************************************************************************************
 * EXPLICACION: Se conectan dos tarjetas LPC1768 a traves del bus I2C2. Utilizando el callback proporcionado por        *
 * CMSIS, hacemos que el hilo de despierte cada vez que se produzca el evento "transmit" o "receive". Como el dato      *
 * que se recibe desde el Slave esta en complemento a uno, deshacemos esa conversion para comprobar que el dato enviado *
 * es igual que el recibido. En consecuencia, se encendera el RGB verde (iguales) o el rojo (distintos).                *
 ************************************************************************************************************************/

static volatile uint32_t I2C_Event;

void Thread (void const *argument);                             
osThreadId tid_Thread;                                          
osThreadDef (Thread, osPriorityNormal, 1, 0);                  
 
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
		
		osSignalSet (tid_Thread, RECIBIDO);
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

//Rutina de atencion a la interrupcion del Timer 3:
void TIMER3_IRQHandler(void)
{
	LPC_TIM3->IR |= 1; //Bajamos el flag.
	
	if(valido == 1)
	{
		GPIO_PinWrite (PUERTO_RGB, VERDE, 0);
		GPIO_PinWrite (PUERTO_RGB, ROJO, 1);
	}
	else
	{
		GPIO_PinWrite (PUERTO_RGB, ROJO, 0);
		GPIO_PinWrite (PUERTO_RGB, VERDE, 1);	
	}
}

void Init_i2c(void)
{
	int32_t status;
	
  status = I2Cdrv->Initialize (I2C_SignalEvent);
  status = I2Cdrv->PowerControl (ARM_POWER_FULL);
  status = I2Cdrv->Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD);
  status = I2Cdrv->Control      (ARM_I2C_BUS_CLEAR, 0);
	
}

int Init_Thread (void) 
{
  tid_Thread = osThreadCreate (osThread(Thread), NULL);
  if (!tid_Thread) return(-1);
  
  return(0);
}

void Thread (void const *argument) 
{
  while (1) 
	{
    osDelay(3000);
		
		cmd = contador++; //Se incrementa...
		cmd_env = cmd; //...se guarda el nuevo valor...
		status = I2Cdrv->MasterTransmit(addr, &cmd, 1, true);
		osSignalWait (RECIBIDO, osWaitForever); 
		
		status = I2Cdrv->MasterReceive(addr, buf, 2, true);
		osSignalWait (RECIBIDO, osWaitForever); 

		cmd_rec=~ buf[0]; //...el dato recibido se vuelve a convertir...
		
		GPIO_PinWrite (PUERTO_RGB, VERDE, 1);
    GPIO_PinWrite (PUERTO_RGB, ROJO, 1);
		
		LPC_I2C2->I2CONSET |= (1<<3);
		
		//...y comparamos con el que se guardó al inicio para el LED RGB.
		if(cmd_env==cmd_rec) //LED VERDE
		{
			valido = 1;
			LPC_TIM3->TCR = 1;
		}
		else //LED ROJO
		{
			valido = 0;
			LPC_TIM3->TCR = 1;
		}		
				
    osThreadYield ();                                          
  }
}
