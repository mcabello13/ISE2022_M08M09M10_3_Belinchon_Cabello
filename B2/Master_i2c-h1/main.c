/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "LPC17xx.h"

//RGB
#define PUERTO_RGB 2
#define AZUL 1	
#define VERDE 2  
#define ROJO 3 

extern void Init_i2c(void);
extern int Init_Thread (void);

/*
 * main: initialize and start the system
 */
int main (void) {
	
	GPIO_SetDir (PUERTO_RGB,ROJO,GPIO_DIR_OUTPUT); //Configuramos los LED como salida.
	GPIO_SetDir (PUERTO_RGB,VERDE,GPIO_DIR_OUTPUT);
	//GPIO_SetDir (PUERTO_RGB,AZUL,GPIO_DIR_OUTPUT); 

	//Programacion del Timer 3 para un segundo:
	
	LPC_SC -> PCONP |= (1<<23); //Timer 3 Power Control (Tabla 46).
	LPC_SC->PCLKSEL1 |= (1<<14); //Reloj del Timer 3 --> 100 MHz (Tabla 41).
	LPC_PINCON->PINSEL0 |= (1<<23) | (1<<22); //MAT 3.1	(Tabla 80)
	LPC_TIM3->EMR |= (1<<7) | (1<<6); //Configuramos la habilitacion del MAT 3.1 para el toggle (Tabla 432).
	LPC_TIM3->TC = 0; //Ponemos el Timer 3 a cero.
	LPC_TIM3->MR1 = 100000000; //Valor del toggle (cuenta) --> Cada un segundo.

	LPC_TIM3->MCR |= 1; //Interrupcion por comparacion --> OJO CON ESO PARA CUANDO SE UTILIZA EL TIMER COMO INTERRUPCION

	LPC_TIM3->MCR |= (1<<4); //Reset para MR1 (Tabla 430).
	LPC_TIM3->TCR = 0; //Habilitamos el Timer 3.

	NVIC_EnableIRQ(TIMER3_IRQn); //--> OJO CON ESO PARA CUANDO SE UTILIZA EL TIMER COMO INTERRUPCION
	
	Init_i2c();
	
  osKernelInitialize ();                    // initialize CMSIS-RTOS

  // initialize peripherals here

  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
	
	Init_Thread();

  osKernelStart ();                         // start thread execution 
}
