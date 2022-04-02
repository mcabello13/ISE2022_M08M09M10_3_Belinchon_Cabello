/*-----------------------------------------------------------------------------
 * Name:    LED_MCB1700.c
 * Purpose: LED interface for MCB1700 evaluation board
 * Rev.:    1.0.1
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2013 - 2017 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/


#include "PIN_LPC17xx.h"
#include "GPIO_LPC17xx.h"

/**********************************************************************************************
 * EXPLICACION IP Y MASCARA EN CASA:																													*
 * Adaptador Ethernet Ethernet --> 255.255.0.0 para mascara ; 169.254.xx.xx para IP del micro	*
 **********************************************************************************************/

/***************************************************************************************************************************************************************
* EXPLICACION: en primer lugar, hemos habilitado la parte de los LED correspondiente al Board. Como																														 *
* los archivos correspondientes al Board son solo de lectura, no podemos modificarlos, de ahi que 																														 *
* tengamos que crear un .h y un .c exactamente iguales a los del board pero lincados a nuestro proyecto para poder 																						 *
* trabajar. Una vez tenemos nuestros dos archivos, editamos el nombre de las funciones referentes al uso de los LED, ya 																			 *
* que si no, dara el error por estar duplicadas. Cuando accedemos desde el navegador a la IP del micro, debemos darnos cuenta																	 *
* de que estamos accediendo al archivo que registra las peticiones del servidor "HTTP_Server_CGI.c", y que en la funcion "cgi_process_data",									 *
* se esta comprobando el dato recibido para encender o apagar los LED. Tras el "do-while", se espera indefinidamente a recibir otro dato (hasta que el usuario *
* haga otra peticion), y ahi debemos poner "LED_SetOut_lpc1768(P2)", porque esta funcion lo que hace es, segun lo que se reciba, enciende o apaga el LED 			 *
* correspondiente, por tanto estara indefinidamente funcionando tras el "do-while" y por tanto, esperando a que se reciba un nuevo dato.											 *
****************************************************************************************************************************************************************/


/* Do not use LED0--LED3, if trace pins are used! */

 #define LED_COUNT (4)

const PIN LED_PIN_lpc1768[] = {
  {1,18},
  {1,20},
  {1,21},
  {1,23},
};

/**
  \fn          int32_t LED_Initialize (void)
  \brief       Initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Initialize_lpc1768 (void) {
  uint32_t n;

  /* Enable GPIO clock */
  GPIO_PortClock     (1);

  /* Configure pins: Output Mode with Pull-down resistors */
  for (n = 0; n < LED_COUNT; n++) {
    PIN_Configure (LED_PIN_lpc1768[n].Portnum, LED_PIN_lpc1768[n].Pinnum, PIN_FUNC_0, PIN_PINMODE_PULLDOWN, PIN_PINMODE_NORMAL);
    GPIO_SetDir   (LED_PIN_lpc1768[n].Portnum, LED_PIN_lpc1768[n].Pinnum, GPIO_DIR_OUTPUT);
    GPIO_PinWrite (LED_PIN_lpc1768[n].Portnum, LED_PIN_lpc1768[n].Pinnum, 0);
  }

  return 0;
}

/**
  \fn          int32_t LED_Uninitialize (void)
  \brief       De-initialize LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Uninitialize_lpc1768 (void) {
  uint32_t n;

  /* Unconfigure pins: turn off Pull-up/down resistors */
  for (n = 0; n < LED_COUNT; n++) {
    PIN_Configure (LED_PIN_lpc1768[n].Portnum, LED_PIN_lpc1768[n].Pinnum, 0 , 0, 0);
  }

  return 0;
}

/**
  \fn          int32_t LED_On (uint32_t num)
  \brief       Turn on requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_On_lpc1768 (uint32_t num) {
  int32_t retCode = 0;

  if (num < LED_COUNT) {
    GPIO_PinWrite (LED_PIN_lpc1768[num].Portnum, LED_PIN_lpc1768[num].Pinnum, 1);
  }
  else {
    retCode = -1;
  }

  return retCode;
}

/**
  \fn          int32_t LED_Off (uint32_t num)
  \brief       Turn off requested LED
  \param[in]   num  LED number
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_Off_lpc1768 (uint32_t num) {
  int32_t retCode = 0;

  if (num < LED_COUNT) {
    GPIO_PinWrite (LED_PIN_lpc1768[num].Portnum, LED_PIN_lpc1768[num].Pinnum, 0);
  }
  else {
    retCode = -1;
  }

  return retCode;
}

/**
  \fn          int32_t LED_SetOut (uint32_t val)
  \brief       Write value to LEDs
  \param[in]   val  value to be displayed on LEDs
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t LED_SetOut_lpc1768 (uint32_t val) {
  uint32_t n;

  for (n = 0; n < LED_COUNT; n++) {
    if (val & (1 << n)) LED_On_lpc1768 (n);
    else                LED_Off_lpc1768(n);
  }

  return 0;
}

/**
  \fn          uint32_t LED_GetCount (void)
  \brief       Get number of LEDs
  \return      Number of available LEDs
*/
uint32_t LED_GetCount_lpc1768 (void) {

  return LED_COUNT;
}
