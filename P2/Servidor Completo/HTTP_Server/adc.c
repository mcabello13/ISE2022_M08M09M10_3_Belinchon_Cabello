/*-----------------------------------------------------------------------------
 * Name:    ADC_MCB1700.c
 * Purpose: A/D Converter interface for MCB1700 evaluation board
 * Rev.:    1.00
 *----------------------------------------------------------------------------*/

/* Copyright (c) 2013 - 2014 ARM LIMITED

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

#include "LPC17xx.h"
#include "PIN_LPC17xx.h"

#include "Board_ADC.h"

#define ADC_RESOLUTION        12        /* Number of A/D converter bits       */
#define PDN 21
#define SBIT_CLCKDIV 8
static volatile uint16_t AD_last;       /* Last converted value               */
static volatile uint8_t  AD_done;       /* AD conversion done flag            */

const PIN ADC_PIN[] = {
  {1, 30},
};

/**
  \fn          int32_t ADC_Initialize (void)
  \brief       Initialize Analog-to-Digital Converter
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/


/************************************************************************************
 * EXPLICACION: en esta funcion se utilizaron los recursos de la asignatura de SBM  *
 * para poder configurar el Potenciometro 1.																			  *
 ************************************************************************************/
int32_t ADC_Initialize_lpc1768  (void) 
{
	
	  LPC_SC->PCONP |= (1 << 12); //A/D converter activado (Tabla 46)
	  PIN_Configure(1,30,PIN_FUNC_3,PIN_PINMODE_PULLUP,PIN_PINMODE_NORMAL); //CUIDADO: Funcion 3 y pulldown!!!
	
	  LPC_ADC->ADCR |= (1<<4); //DIP 19 --> P1.30 --> Registro AD0.4 correspondiente al POT 1
	  LPC_ADC->ADCR |= (4<<8); //ADC clock is 25 MHz/s
	  LPC_ADC->ADCR |= (1<<21); //Enable ADC
	  LPC_ADC->ADINTEN |= (1<<8); //Global ADC enable interrupt 
	  
	  NVIC_EnableIRQ(ADC_IRQn); //Enable ADC Interrupt
		return 0;
}

/**
  \fn          int32_t ADC_Uninitialize (void)
  \brief       De-initialize Analog-to-Digital Converter
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_Uninitialize_lpc1768  (void) {

  NVIC_DisableIRQ (ADC_IRQn);            /* Disable ADC Interrupt               */
  LPC_ADC->ADINTEN &= ~( 1 <<  8);       /* Disable global ADC enable interrupt */

  PIN_Configure (ADC_PIN[0].Portnum, ADC_PIN[0].Pinnum, 0, 0, 0);
  
  LPC_SC->PCONP &= ~(1 << 12);           /* Disable ADC clock */

  return 0;
}

/**
  \fn          int32_t ADC_StartConversion (void)
  \brief       Start conversion
  \returns
   - \b  0: function succeeded
   - \b -1: function failed
*/
int32_t ADC_StartConversion_lpc1768  (void) {

  LPC_ADC->ADCR &= ~( 7 << 24);              /* stop conversion               */
  LPC_ADC->ADCR |=  ( 1 << 24);              /* start conversion              */

  return 0;
}

/**
  \fn          int32_t ADC_ConversionDone (void)
  \brief       Check if conversion finished
  \returns
   - \b  0: conversion finished
   - \b -1: conversion in progress
*/
int32_t ADC_ConversionDone_lpc1768  (void) {
  return (AD_done ? 0 : -1);
}

/**
  \fn          int32_t ADC_GetValue (void)
  \brief       Get converted value
  \returns
   - <b> >=0</b>: converted value
   - \b -1: conversion in progress or failed
*/
int32_t ADC_GetValue_lpc1768  (void) {

  if (AD_done) {
    AD_done = 0;
    return AD_last;
  }
  return -1;
}

/**
  \fn          uint32_t ADC_GetResolution (void)
  \brief       Get resolution of Analog-to-Digital Converter
  \returns     Resolution (in bits)
*/
uint32_t ADC_GetResolution_lpc1768  (void) {
  return ADC_RESOLUTION;
}

/**
  \fn          void ADC_IRQHandler (void)
  \brief       Analog-to-Digital Converter Interrupt Handler
*/
void ADC_IRQHandler(void) {
  volatile uint32_t adstat;

  adstat = LPC_ADC->ADSTAT;                        /* Read ADC clears interrupt     */

  AD_last = (LPC_ADC->ADGDR >> 4) & 0xFFF;         /* Store converted value   */

  AD_done = 1;
}
