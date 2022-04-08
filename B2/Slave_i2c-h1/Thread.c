/****************************
 *Alumnos:                  *
 * Eduardo Belinchon Vera.  *
 * Miguel Cabello Adrada.   *
 ****************************/

//SLAVE   
 
#include "cmsis_os.h"                                        
#include "Driver_I2C.h"

#define REG_TEMP    					0x00
#define REG_CONF	    				0x01
#define	REG_THYST   					0x02
#define REG_TOS     					0x03

#define RECIBIDO2							0x0001
#define RECIBIDO3							0x0010

int32_t status = 0, nlect = 0;
uint8_t cmd, buf;
uint8_t buf_CA1;
uint16_t temp;
uint8_t contador = 0;
int i;

//Driver I2C
extern ARM_DRIVER_I2C            Driver_I2C2;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C2;

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
    
    osSignalSet (tid_Thread, RECIBIDO2);
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

void Init_i2c(void)
{
	int32_t status;
	
  status = I2Cdrv->Initialize (I2C_SignalEvent);
  status = I2Cdrv->PowerControl (ARM_POWER_FULL);
  status = I2Cdrv->Control      (ARM_I2C_BUS_SPEED, ARM_I2C_BUS_SPEED_STANDARD);
  status = I2Cdrv->Control      (ARM_I2C_OWN_ADDRESS, 0x28);
	
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
		status = I2Cdrv->SlaveReceive(&buf, 1);
		osSignalWait (RECIBIDO2, osWaitForever); 
    
    buf_CA1=~buf; //Conversion del dato a complemento a uno.
    
		status = I2Cdrv->SlaveTransmit(&buf_CA1, 1);
		osSignalWait (RECIBIDO2, osWaitForever); 
				
    osThreadYield ();                                           
  }
}
