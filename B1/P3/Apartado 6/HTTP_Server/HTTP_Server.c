/****************************
 *Alumnos:                  *
 * Eduardo Belinchon Vera.  *
 * Miguel Cabello Adrada.   *
 ****************************/

/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2014 ARM Germany GmbH. All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>
#include "cmsis_os.h"                   /* CMSIS RTOS definitions             */
#include "rl_net.h"                     /* Network definitions                */
#include "leds1768.h"                     /* Network definitions                */
#include "lcd.h"
#include "adc.h"
#include "adc.h"
#include "rtc.h"
#include "sntp.h"
#include "GPIO_LPC17xx.h"
#include "PIN_LPC17xx.h"
#include "LPC17xx.h"
//#include "lpc17xx_wdt.h"
#include "lpc_types.h"
#include "lpc17xx_iap.h"
#include "debug_frmwrk.h"
#include "Net_Config_ETH_0.h" 

//Pulsador:
#define PUERTO_PULSADOR 0
#define CENTER 16
#define PULLDOWN ((uint32_t)(3))
#define NORMAL ((uint32_t)(0))

/*******************************************************************************************************************************************************
* EXPLICACION: queremos almacenar la informacion en los 10 primeros bytes del sector, es decir: 2^10 = 1024, que en hexadecimal es "0x400". Este       *
* va a ser el tamaño del area para guardar. Escogemos el Sector 19 (Start: 0x00028000 End: 0x0002FFFF). El tamaño del Buffer sera de 256 bytes, porque *
* la informacion se guardara en cuatro bloques de 256 bytes.																																													 *
********************************************************************************************************************************************************/

#define FLASH_PROG_AREA_START 0x00028000 
#define FLASH_PROG_AREA_SIZE 0x400 
#define FLASH_LED 0x0A //La tarjeta tiene cuatro LED, pero guardamos 1 byte (un octeto).
#define BUFF_SIZE 256

//Variables:
bool LEDrun;
bool LCDupdate;
char lcd_text[2][20+1];
char lcd_buf[20+1]; 
int i;
IAP_STATUS_CODE status;
uint8_t bufFlash[256]; 
extern uint8_t modoLeds;
uint8_t *ptr;

//Hilos:
extern osThreadId tid_ThreadPulsador; 

static void BlinkLed (void const *arg);
static void Display (void const *arg);

osThreadDef(BlinkLed, osPriorityNormal, 1, 0);
osThreadDef(Display, osPriorityNormal, 1, 0);

//Funcion que inicializa la Flash:
void flash (void)
{	
  uint32_t flash_prog_area_sec_start;
  uint32_t flash_prog_area_sec_end;
	uint32_t i;
	uint32_t result[4];
	uint8_t ver_major, ver_minor;
	
	for (i = 0;i < sizeof(bufFlash);i++)
  {
    bufFlash[i] = (uint8_t)i; //Guarda valores en el buffer.
  }
	
	status = ReadPartID(result); //--> Recordar que cada sector tiene un numero de identificacion (Table 567 del Manual).
  if(status != CMD_SUCCESS)
  {
     while(1);
  }
	
	//Lectura del Numero de Version del Codigo de Arranque
  status = ReadBootCodeVer(&ver_major, &ver_minor); //--> Recordar el concepto de SSOO cuando se arranca el PC y se lee el codigo de arranque.
  if(status != CMD_SUCCESS)
  {
     while(1);
  }

 //Lectura del Numero de Identifiacion del Dispositivo (Serial Number)
  status = ReadDeviceSerialNum(result);
  if(status != CMD_SUCCESS)
  {
     while(1);
  }
	
	//Lectura del Numero de Identifiacion del Dispositivo (Serial Number)
  status = ReadDeviceSerialNum(result);
  if(status != CMD_SUCCESS)
  {
     while(1);
  }
	
  ptr = (uint8_t*)(FLASH_PROG_AREA_START + FLASH_PROG_AREA_SIZE + 1); //Se prepara el puntero...
	modoLeds = *ptr; //...y se lee el valor de la variable que almacena el estado de los LED...

	if((modoLeds <= 0x08)) //...segun su valor, se encienden los que correspondan.
	{
		if(modoLeds == 0x01)
		{
			GPIO_PinWrite(PUERTO_LED,LED1,1);
			
		}else if(modoLeds == 0x02){
			GPIO_PinWrite(PUERTO_LED,LED2, 1);
		
		}else if(modoLeds == 0x04){
			GPIO_PinWrite(PUERTO_LED,LED3,1);			
			
		}else if(modoLeds == 0x08){
			GPIO_PinWrite(PUERTO_LED,LED4,1);
		}
	}
	else //Si el valor es mayor, quiere decir que estaba en modo "barrido".
	{
		LEDrun = true;
	}
	
	bufFlash[0] = ETH0_IP1; //Direccion IP.
	bufFlash[1] = ETH0_IP2;
	bufFlash[2] = ETH0_IP3;
	bufFlash[3] = ETH0_IP4;
	bufFlash[4] = ETH0_MAC1; //Direccion MAC.
	bufFlash[5] = ETH0_MAC2;
	bufFlash[6] = ETH0_MAC3;
	bufFlash[7] = ETH0_MAC4;
	bufFlash[8] = ETH0_MAC5;
	bufFlash[9] = ETH0_MAC6;
	
	flash_prog_area_sec_start = GetSecNum(FLASH_PROG_AREA_START); //Preparado de Flash.
  flash_prog_area_sec_end =  GetSecNum(FLASH_PROG_AREA_START + FLASH_PROG_AREA_SIZE); 
	
	status = EraseSector(flash_prog_area_sec_start, flash_prog_area_sec_end); //Borrado del sector.

	for ( i = 0; i < FLASH_PROG_AREA_SIZE/BUFF_SIZE; i++ ) //Se guardan cuatro "buffer", ya que el tamaño es 1000 y el tamaño es de 256 bytes.
  {
    ptr = (uint8_t*)(FLASH_PROG_AREA_START + i*BUFF_SIZE); 
		status =  CopyRAM2Flash(ptr, bufFlash,IAP_WRITE_256);
		
		/*if(status != CMD_SUCCESS)
		{   
			 if(status == SECTOR_NOT_BLANK)
			 {
				 
			 }
			 while(1); 
		}*/
	}
}

/********************************************************************************************************************************
* EXPLICACION POTENCIOMETRO:																																																		*
* Al igual que hicimos con los LED, cogemos los archivos que nos proporciona el Board, creamos los nuestros y podemos           *
* desactivar el Board. Cambiamos el nombre a las funciones para que no de el error de duplicidad y razonamos la funcion del AD  *
* del HTTP_Server, que es: uint16_t AD_in (uint32_t ch). En esta funcion debemos invocar ADC_StartConversion_lpc1768 () para    *
* que empiece a convertir, mientras la conversion no termine, es decir, la funcion ADC_ConversionDone_lpc1768 () no devuelva -1,* 
* estara convirtiendo. Una vez devuelva -1, entonces con ADC_GetValue_lpc1768 () guardamos el valor para devolverlo en val.     *
* En el archivo adc.c debemos modificar la funcion ADC_Initialize_lpc1768 con el puerto y el pin correspondientes								*
* al POT 1, Recordar --> DIP 19 --> P1.30 --> Registro AD0.4 correspondiente al POT 1 (ESQUEMATICO)															*
*********************************************************************************************************************************/

/// Read analog inputs (POTENCIOMETRO 1)
uint16_t AD_in (uint32_t ch) {
  int32_t val = 0;

  if (ch == 0) 
	{
    ADC_StartConversion_lpc1768 (); //Empieza la lectura...
		
    while (ADC_ConversionDone_lpc1768 () < 0); //...estara convirtiendo hasta que se devuelva -1...
    val = ADC_GetValue_lpc1768(); //...y cuando finalice, guarda el valor.
  }
  return (val);
}

/// Read digital inputs
uint8_t get_button (void) {
}

/// IP address change notification
void dhcp_client_notify (uint32_t if_num,
                         dhcpClientOption opt, const uint8_t *val, uint32_t len) {
  if (opt == dhcpClientIPaddress) {
    // IP address has changed
    sprintf (lcd_text[0],"IP address:");
    sprintf (lcd_text[1],"%s", ip4_ntoa (val));
    LCDupdate = true;
  }
}

/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
static void Display (void const *arg) {
  //char lcd_buf[20+1]; int i; --> Mejor como variable global para poder utilizar el watch.

  //sprintf (lcd_text[0], "E");
  //sprintf (lcd_text[1], "M");
  LCDupdate = true;

  while(1) 
	{
    if (LCDupdate == true) 
		{	
			limpiardisplay(); //Limpiamos el display...
			
			for(i=0; i<21; i++) //...se resetea el buffer...
			{
				lcd_buf[i] = 0;
			}
			
      sprintf (lcd_buf, "%s", lcd_text[0]); //...y se componen las cadenas de caracteres para...
			EscribeFraseL1(lcd_buf);             //...escribir en ambas lineas del LCD.
			
      sprintf (lcd_buf, "%s", lcd_text[1]);
			EscribeFraseL2(lcd_buf);
			
      LCDupdate = false;
    }
    osDelay (250);
  }
}

/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
  EXPLICACION --> Debemos retocar esta funcion para adecuarla al LPC1768, lo 
                  primero los valores de led_val, para que el barrido pueda 
                  realizarse, y por ultimo, la invocacion de SetOut para  
                  que el valor vaya cambiando, y asi conseguir que luzcan todos
                  los LED.
 *---------------------------------------------------------------------------*/
static void BlinkLed (void const *arg) {
  
	const uint8_t led_val[6] = { 0x01,0x02,0x04,0x08,0x04,0x02}; //SE HA MODIFICADO led_val CON LOS VALORES ADECUADOS PARA LOS LED.
  int cnt = 0;

  LEDrun = false; //Inicialmente en "false" para que no realice el barrido nada mas cargar la aplicacion.
	
  while(1) {
    // Every 100 ms
    if (LEDrun == true) {
			
			LED_SetOut_lpc1768(led_val[cnt]); //Esta funcion es la encargada del ON y OFF, simplemente...
                                       //...pasamos como parametro el cnt.
			
      if (++cnt >= sizeof(led_val)) {
        cnt = 0;
      }
    }
    osDelay (100);
  }
}

//Interrupciones Externas (Joystick):
void EINT3_IRQHandler(void)
{	
	////////////////CENTER////////////////////
	if ((LPC_GPIOINT->IO0IntStatR & (1 << CENTER)))
	{
			LPC_GPIOINT->IO0IntClr |= (1 << CENTER);

			osSignalSet(tid_ThreadPulsador, signal_center);
	}
}

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
int main (void) {

	osKernelInitialize();
	
	//WATCHDOG (CONFIGURACION)
	/*WDT_Init(WDT_CLKSRC_IRC, WDT_MODE_RESET);
	NVIC_EnableIRQ(WDT_IRQn);
	WDT_Start(5000000); //APUNTE --> Se coloca un punto de ruptura en la linea 188/212 para comprobar que salta el Watchdog.*/
	
	//Configuracion del pulsador (gesto CENTER del Joystick):
	GPIO_SetDir (PUERTO_PULSADOR,CENTER,GPIO_DIR_INPUT);
	PIN_Configure (PUERTO_PULSADOR, CENTER,PIN_FUNC_0,PULLDOWN,NORMAL);
	LPC_GPIOINT->IO0IntEnR |= (1 << CENTER); //Flanco de subida.
	NVIC_EnableIRQ(EINT3_IRQn);//Habilita la interrupción externa EINT3.
	
  LED_Initialize_lpc1768();
	ADC_Initialize_lpc1768();
	net_initialize();
	init();
	LCD_reset();

	c_entry();
	Init_Thread();
	
	osThreadCreate (osThread(BlinkLed), NULL);
  osThreadCreate (osThread(Display), NULL);
	
	osKernelStart();
	
	flash();
	
  while(1) {
		//WDT_Feed(); //ALIMENTACION DEL WATCHDOG.
    net_main ();
    osThreadYield ();
  }
}
