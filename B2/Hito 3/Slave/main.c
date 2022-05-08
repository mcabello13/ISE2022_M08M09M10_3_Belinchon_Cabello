#include "osObjects.h"                   
#include "lpc17xx.h"
#include "GPIO_LPC17xx.h"
#include "LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "comun.h"

#define osObjectsPublic  

extern void Init_i2c(void);
extern int Init_Thread (void);

int main (void) 
{	
	GPIO_SetDir (PUERTO_MUX,PIN_21,GPIO_DIR_OUTPUT);
	GPIO_SetDir (PUERTO_MUX,PIN_22,GPIO_DIR_OUTPUT);
	GPIO_SetDir (PUERTO_MUX,PIN_23,GPIO_DIR_OUTPUT);
	
	//DAC
	//LPC_PINCON->PINSEL1 |= (1<<21);
	//GPIO_SetDir (PUERTO_MUX,PIN_MUX,GPIO_DIR_OUTPUT);
	
	Init_i2c();
	
  osKernelInitialize ();                  
	
	Init_Thread();

  osKernelStart ();                         
}
