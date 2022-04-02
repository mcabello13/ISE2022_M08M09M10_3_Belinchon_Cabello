#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "LPC17xx.h"
#include "lpc17xx_wdt.h"

//Leds
#define PUERTO_LED 1 
#define LED_1      18
#define LED_2      20
#define LED_3      21
#define LED_4      23

#define PUERTO_LED_RGB 		2 
#define LED_ROJO      		3 
#define LED_VERDE     	  2 

//Joystick
#define PUERTO_INT 0 
#define SW_DOWN    17

uint8_t toggle_led3 = 0;
uint8_t estado = 0;
static uint32_t cuenta;
			
//Rutina de atencion a la interrupcion del Timer 2:			
void TIMER2_init (int mseg)			
{ 		
	LPC_SC->PCONP |= 1<<22; //Power Control para el reloj del Timer 2.
	LPC_SC->PCLKSEL1 |= 1<<12;// Se habilita el reloj del Timer 2.
	
	LPC_TIM2->MR0= 100000 *mseg; //Valor de la cuenta.
	LPC_TIM2->MCR |= 1; //Interrupcion del Match.
	LPC_TIM2->TCR = 1; //Se habilita el timer.
	
	while(LPC_TIM2 -> IR != 0x01);
	LPC_TIM2-> IR |=1; //Limpia flag.
	LPC_TIM2->TCR &= 0; //Se deshabilita la cuenta del timer.
	LPC_TIM2->TC= 0; //Contador del timer a cero.
}

//Funcion que realiza un retardo de 3 segundos:
void delay_3s(void)
{
	int retardo = 25000; //40ns*25000 = 1ms ; CKL/4 = 25 MHz.
	int i;
	
	for(i=0; i<retardo*3000; i++)
	{
		
	}
}

void EINT3_IRQHandler (void) 
{
		if (LPC_GPIOINT->IO0IntStatR & (1 << SW_DOWN))
		{ 	
			 if (cuenta < 10) 
			 {
					estado = ~estado;
					GPIO_PinWrite(PUERTO_LED,LED_4, estado);
					LPC_GPIOINT->IO0IntClr |= 1<<SW_DOWN; //APUNTE --> Provocamos el fallo del anterior apartado para que salte el LED ROJO.
				  cuenta++;														 //APUNTE --> Para que salte el LED VERDE, basta con pulsar el Reset del micro.
			 }
			 //LPC_GPIOINT->IO0IntClr |= 1<<SW_DOWN; //El Watchdog saltaba debido a que el flag de la interrupcion se estaba limpiando dentro del bucle "if"...
																						//...y esto provocaba que al llegar la cuenta a 10, el flag quedara sin limpiar, algo erroneo, por lo tanto ese... 
																					 //...fallo lo estaba detectando el Watchdog y de ahi que saltara la interrupcion de este.
		}
}

//Rutina de atencion a la interrupcion del Watchdog:
void WDT_IRQHandler (void) 	
{ 
		NVIC_DisableIRQ(WDT_IRQn); //Deshabilita el Watchdog.
	
		toggle_led3=~toggle_led3;
		GPIO_PinWrite(PUERTO_LED, LED_3, toggle_led3);
	
		WDT_ClrTimeOutFlag (); //Se limpia el flag de la interrupcion.
}
 
int main (void) 
{	
	GPIO_SetDir (PUERTO_LED, LED_1, GPIO_DIR_OUTPUT);
	GPIO_SetDir (PUERTO_LED, LED_2, GPIO_DIR_OUTPUT); 
	GPIO_SetDir (PUERTO_LED, LED_3, GPIO_DIR_OUTPUT); 
	GPIO_SetDir (PUERTO_LED, LED_4, GPIO_DIR_OUTPUT); 
	
	GPIO_SetDir(PUERTO_LED_RGB,LED_ROJO,GPIO_DIR_OUTPUT);
  GPIO_SetDir(PUERTO_LED_RGB,LED_VERDE,GPIO_DIR_OUTPUT);	
	PIN_Configure(PUERTO_LED_RGB,LED_ROJO,PIN_FUNC_0,PIN_PINMODE_PULLDOWN,PIN_PINMODE_NORMAL);
  PIN_Configure(PUERTO_LED_RGB,LED_VERDE,PIN_FUNC_0,PIN_PINMODE_PULLDOWN,PIN_PINMODE_NORMAL);
	
	PIN_Configure(PUERTO_INT, SW_DOWN, PIN_FUNC_0, PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
	LPC_GPIOINT->IO0IntEnR = (1 << SW_DOWN) ; 

	NVIC_EnableIRQ(EINT3_IRQn); 
	
	/******************************************************************************************************************************************************
	 * WATCHDOG: Primero se inicializa con la funcion Init, que tiene como parametros el reloj (se coge el predeterminado, el del oscilador interno),			*
	 * y el modo de funcionamiento (en este caso, modo interrupcion). Se habilita su interrupcion con el registro NVIC y se realiza el start con 					*
	 * un tiempo de cinco segundos. Para evitar que la cuenta interna llegue a cero y se interrumpa el funcionamiento normal de la aplicacion, hay que		*
	 * alimentar continuamente al Watchdog con la funcion WDT_Feed:																																												*
	 *		void WDT_Feed (void)																																																														*
	 *		{																																																																								*
	 *			// Disable irq interrupt																																																											*
	 *			__disable_irq();																																																															*
	 *			LPC_WDT->WDFEED = 0xAA;																																																												*
	 *			LPC_WDT->WDFEED = 0x55;																																																												*
	 *			// Then enable irq interrupt																																																									*
	 *			__enable_irq();																																																																*
	 *		}																																																																								*
	 *	Esta funcion se encarga de escribir 0xAA y 0x55 para recargar el temporizador del Watchdog.																												*
	 ******************************************************************************************************************************************************/
	
	//Capitulo 28 del Manual --> Apartados 28.4.1 , 28.4.3
	
  WDT_Init (WDT_CLKSRC_IRC, WDT_MODE_RESET); //AHORA ESTA EN MODO RESET.
	NVIC_EnableIRQ(WDT_IRQn); //Se habilita la interrupcion del Watchdog
  WDT_Start(5000000); //5 segundos.
	
	/******************************************************************************************************************************************************
	 * WATCHDOG: para diferenciar la causa del reset, consultamos la funcion definida como "WDT_ReadTimeOutFlag", que devuelve el estado del flag WDTOF		*
	 * (time-out-flag de Table 523 del manual), si esta a uno, el error ha sido provocado por el Watchdog (LED rojo), en caso contrario, el reset					*																										*
	 * habra sido provocado por la alimentacion, entonces se encendera el LED verde.																																			*
	 ******************************************************************************************************************************************************/
	
	if(WDT_ReadTimeOutFlag()) //Provocado por el Watchdog.
	{
			GPIO_PinWrite(PUERTO_LED_RGB, LED_VERDE, 1);
			GPIO_PinWrite(PUERTO_LED_RGB, LED_ROJO, 0);
		
			WDT_ClrTimeOutFlag(); //Se limpia el flag.
	}
	else //Provocado por la alimentacion.
	{
			GPIO_PinWrite(PUERTO_LED_RGB, LED_VERDE, 0);
			GPIO_PinWrite(PUERTO_LED_RGB, LED_ROJO, 1);
	}
	
	WDT_Feed(); //Se alimenta al Watchdog.
	
	delay_3s(); //Antes de iniciar la aplicacion, realizamos el testeo durante tres segundos.
	
	GPIO_PinWrite(PUERTO_LED_RGB, LED_VERDE, 1);
	GPIO_PinWrite(PUERTO_LED_RGB, LED_ROJO, 1);
	
  while(1)
	{		
		WDT_Feed(); //Dentro del while(1) se alimenta continuamente al Watchdog para evitar que se active.
		
		GPIO_PinWrite(PUERTO_LED, LED_1, 1);
		GPIO_PinWrite(PUERTO_LED, LED_2, 0);
		
		TIMER2_init(500);
		
		GPIO_PinWrite(PUERTO_LED, LED_1, 0);
		GPIO_PinWrite(PUERTO_LED, LED_2, 1);
		
		TIMER2_init(500); //APUNTE --> Colocar en esta linea un punto de ruptura para ver saltar el Watchdog.
	}			
}
