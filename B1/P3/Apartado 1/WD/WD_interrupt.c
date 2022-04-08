/**********************************************************************
* $Id$		wdt_interrupt_test.c			2010-05-21
*//**
* @file		wdt_interrupt_test.c
* @brief	This example describes how to use Watch-dog timer application
*           in interrupt mode
* @version	2.0
* @date		21. May. 2010
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2010, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/
#include "lpc17xx_wdt.h"
//#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"

/* Example group ----------------------------------------------------------- */
/** @defgroup WDT_INTERRUPT	INTERRUPT
 * @ingroup WDT_Examples
 * @{
 */

/************************** PRIVATE DEFINITIONS *************************/
#define MCB_LPC_1768
//#define IAR_LPC_1768

//Watchodog time out in 5 seconds
#define WDT_TIMEOUT 	5000000


/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
" Watch dog timer interrupt (test or debug mode) demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: Cortex M3 \n\r"
"\t - Communicate via: UART0 -  115200 bps \n\r"
" Use WDT with Internal RC OSC, interrupt mode (test only), timeout = 1 seconds \n\r"
" To generate an interrupt, after interrupt WDT interrupt is disabled immediately! \n\r"
"********************************************************************************\n\r";
uint8_t info1[] = "BEFORE WDT interrupt!\n\r";
uint8_t info2[] = "AFTER WDT interrupt\n\r";

int wdt_flag = 0;
int LED_toggle = 0;


	

/*----------------- INTERRUPT SERVICE ROUTINES --------------------------*/
/*********************************************************************//**
 * @brief		WDT interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void WDT_IRQHandler(void)
{
	// Disable WDT interrupt
	NVIC_DisableIRQ(WDT_IRQn);
	// Set WDT flag according
	if (wdt_flag == 1)
		wdt_flag = 0;
	else
		wdt_flag = 1;
	// Clear TimeOut flag
	WDT_ClrTimeOutFlag();
}



/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main WDT program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
void init_WD_interrupt(void)
{


	/* Install interrupt for WDT interrupt */
	NVIC_SetPriority(WDT_IRQn, 0x10);
	// Set Watchdog use internal RC, just generate interrupt only in 5ms if Watchdog is not feed

	// Init WDT, IRC OSC, interrupt mode, timeout = 5000000 us = 5s
	WDT_Init(WDT_CLKSRC_IRC, WDT_MODE_INT_ONLY);

	/* Enable the Watch dog interrupt*/
	NVIC_EnableIRQ(WDT_IRQn);


			// Start watchdog with timeout given
			WDT_Start(WDT_TIMEOUT);
	
			if (WDT_ReadTimeOutFlag()){
			
			// Clear TimeOut flag
			WDT_ClrTimeOutFlag();
			} else{
				

			}
		
}

