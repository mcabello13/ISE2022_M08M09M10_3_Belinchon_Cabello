/**********************************************************************
* $Id$		iaptest.c			2012-04-18
*//**
* @file		lpc17xx_iap.h
 * @brief	IAP demo
* @version	1.0
* @date		18. April. 2012
* @author	NXP MCU SW Application Team
* 
* Copyright(C) 2011, NXP Semiconductor
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

#include "lpc17xx.h"
#include "lpc_types.h"
#include "lpc17xx_iap.h"
#include "debug_frmwrk.h"
//#include "lpc17xx_libcfg.h"

/** The area will be erase and program */
#define FLASH_PROG_AREA_START       0x8000  //Sector 8, 4 kB, 8000-7FFF
#define FLASH_PROG_AREA_SIZE		0x1000 // tamano area 1000

/** The origin buffer on RAM */
#define BUFF_SIZE           1024
#ifdef __IAR_SYSTEMS_ICC__
#pragma data_alignment=4
uint8_t buffer[BUFF_SIZE];
#else
uint8_t __attribute__ ((aligned (4))) buffer[BUFF_SIZE];
#endif

/** @defgroup IAP_Demo	IAP Demo
 * @ingroup IAP_Examples
 * @{
 */
 
/** Main menu */
uint8_t menu[]=
"\n\r********************************************************************************\n\r"
" Hello NXP Semiconductors \n\r"
" IAP Demotrastion \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM CORTEX-M3 \n\r"
"\t - UART Communication: 115200 bps \n\r"
"********************************************************************************\n\r";

/*********************************************************************//**
 * @brief		The entry of the program
 *
 * @param[in]None
 *
 * @return 	None.
 *
 **********************************************************************/
 
void c_entry (void)
{	    		
  uint32_t result[4];
  uint8_t ver_major, ver_minor;
  uint32_t i;
  uint8_t *ptr;
  uint32_t flash_prog_area_sec_start;
  uint32_t flash_prog_area_sec_end;
  IAP_STATUS_CODE status;

  //Initialize
  //debug_frmwrk_init();
  for (i = 0;i < sizeof(buffer);i++)
  {
    buffer[i] = (uint8_t)i; //Guarda valores en el buffer.
  }
	
	//Principio y final del Sector
  flash_prog_area_sec_start = GetSecNum(FLASH_PROG_AREA_START); //Pasa la direccion del sector y devuelve el numero de este.
  flash_prog_area_sec_end =  GetSecNum(FLASH_PROG_AREA_START + FLASH_PROG_AREA_SIZE); //Pasa la direccion del primer sector (sector 8) y le suma la RAM para calcular... 
																																										 //...el numero del sector final.
  //Lectura del Numero de Identificacion
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

	//Borrado de un sector de la Memoria Flash. Parametros: "Start Sector Number" y el "End Sector Number" 
  status = EraseSector(flash_prog_area_sec_start, flash_prog_area_sec_end); //--> Recordar la info de Table 591 del Manual (Erase Sector(s))
  if(status != CMD_SUCCESS) 
  {
     while(1); 
  }
	
  //Comprobacion de si un Sector esta en blanco
  status = BlankCheckSector(flash_prog_area_sec_start, flash_prog_area_sec_end,&result[0], &result[1]);
																			//--> Table 592 del Manual: devuelve la posicion del siguiente sector NO BLANCO y su contenido.
  if(status != CMD_SUCCESS)
  {
     
	 if(status == SECTOR_NOT_BLANK)
	 {
		 
	 }
     while(1); 
  }
  
  /* Be aware that Program and ErasePage take long time to complete!!! If bigger
  RAM is present, allocate big buffer and reduce the number of Program blocks. */

  /* Program flash block by block until the end of the flash. */
  for ( i = 0; i < FLASH_PROG_AREA_SIZE/BUFF_SIZE; i++ )
  {
		ptr = (uint8_t*)(FLASH_PROG_AREA_START + i*BUFF_SIZE);
		status =  CopyRAM2Flash(ptr, buffer,IAP_WRITE_1024);//Copia de la RAM a la FLASH, escribe en el area elegida el buffer con 1024 B
		
		if(status != CMD_SUCCESS)
		{
				while(1);
		}
  }
  // Compare
  for ( i = 0; i < FLASH_PROG_AREA_SIZE/BUFF_SIZE; i++ )
  {
		ptr = (uint8_t*)(FLASH_PROG_AREA_START + i*BUFF_SIZE);
		status =  Compare(ptr, buffer,BUFF_SIZE);//Compara contenidos de dos localizaciones, ya sea en RAM o FLASH...
																						//... --> Table 596 del Manual (Compare).
		
		if(status != CMD_SUCCESS)
		{
				while(1);
		}
  }
  while (1);
}


 int main (void)
{
   c_entry();
   return 0;
}
