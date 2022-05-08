/****************************
 *Alumnos:                  *
 * Eduardo Belinchon Vera.  *
 * Miguel Cabello Adrada.   *
 ****************************/

//SLAVE AGP.
 
#include "cmsis_os.h"                                        
#include "Driver_I2C.h"
#include "lpc17xx.h"
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "comun.h"

#define REG_TEMP    					0x00
#define REG_CONF	    				0x01
#define	REG_THYST   					0x02
#define REG_TOS     					0x03

#define RECIBIDO2							0x0001
#define RECIBIDO3							0x0010

int32_t status = 0, nlect = 0;
uint8_t cmd, buf;
uint8_t buf_slave;//, val;
uint16_t temp;
uint8_t contador = 0;
int i;

//Driver I2C
extern ARM_DRIVER_I2C            Driver_I2C2;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C2;

static volatile uint32_t I2C_Event;

void ThreadSlaveI2C (void const *argument);                            
osThreadId tid_ThreadSlaveI2C;                                          
osThreadDef (ThreadSlaveI2C, osPriorityNormal, 1, 0);                   
 
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
    
    osSignalSet (tid_ThreadSlaveI2C, RECIBIDO2);
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
  status = I2Cdrv->Control      (ARM_I2C_OWN_ADDRESS, 0x28);
	
}

//Funcion que inicializa los hilos:
int Init_Thread (void) 
{
  tid_ThreadSlaveI2C = osThreadCreate (osThread(ThreadSlaveI2C), NULL);
  if (!tid_ThreadSlaveI2C) return(-1);
  
  return(0);
}

//Hilo que gestiona el dato recibido desde el Master:
void ThreadSlaveI2C (void const *argument)
{
  while (1) 
	{	
		status = I2Cdrv->SlaveReceive(&buf, 1);
		osSignalWait (RECIBIDO2, osWaitForever); 
    
		//INFORMACION --> https://www.electronicshub.org/how-to-use-dac-in-lpc1768/
		//val = LPC_DAC->DACR = (buf<<6); //Convierte --> D/A.			
		
		buf_slave = buf;
		
		//MUX --> LPC1768: S1-->21, S2-->22, S3-->23.
		if(buf_slave == 1)
		{
			GPIO_PinWrite (PUERTO_MUX, PIN_23, 0);
			GPIO_PinWrite (PUERTO_MUX, PIN_22, 0);
			GPIO_PinWrite (PUERTO_MUX, PIN_21, 1);
		}
		else if (buf_slave == 5)
		{
			GPIO_PinWrite (PUERTO_MUX, PIN_23, 0);
			GPIO_PinWrite (PUERTO_MUX, PIN_22, 1);
			GPIO_PinWrite (PUERTO_MUX, PIN_21, 0);
		}
		else if(buf_slave == 10)
		{
			GPIO_PinWrite (PUERTO_MUX, PIN_23, 0);
			GPIO_PinWrite (PUERTO_MUX, PIN_22, 1);
			GPIO_PinWrite (PUERTO_MUX, PIN_21, 1);
		}
		else if(buf_slave == 50)
		{
			GPIO_PinWrite (PUERTO_MUX, PIN_23, 1);
			GPIO_PinWrite (PUERTO_MUX, PIN_22, 0);
			GPIO_PinWrite (PUERTO_MUX, PIN_21, 0);
		}
		else if(buf_slave == 100)
		{
			GPIO_PinWrite (PUERTO_MUX, PIN_23, 1);
			GPIO_PinWrite (PUERTO_MUX, PIN_22, 0);
			GPIO_PinWrite (PUERTO_MUX, PIN_21, 1);
		}
		
		status = I2Cdrv->SlaveTransmit(&buf_slave, 1);
		osSignalWait (RECIBIDO2, osWaitForever); 
				
    osThreadYield ();                                           
  }
}
