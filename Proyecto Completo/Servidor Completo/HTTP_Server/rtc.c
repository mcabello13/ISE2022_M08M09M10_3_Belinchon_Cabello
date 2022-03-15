#include "LPC17xx.h"
#include "lcd.h"
#include "cmsis_os.h"  
#include "rtc.h"  
#include "lpc17xx_rtc.h" 
#include "time.h"

//INFORMACION --> https://github.com/scottellis/lpc17xx.cmsis.driver.library/blob/master/Examples/RTC/Calibration/rtc_calib.c
//L:\ING_SIS_ELE\lpc175x_6x_cmsis_driver_library\Drivers\source
//L:\ING_SIS_ELE\lpc175x_6x_cmsis_driver_library\Drivers\include
//L:\ING_SIS_ELE\lpc175x_6x_cmsis_driver_library\Examples\RTC

//Variables:
int segundos;
int minutos;
int horas;
int dia;
int mes;
int ano;
char tiempo[20];
char fecha[20];

extern osThreadId tid_ThreadLCD;    
extern osThreadId tid_ThreadLED4;   
extern void c_entry(void);
extern void RTC_IRQHandler(void);

//Funcion que obtiene el tiempo y fecha del RTC y lo escribe en el LCD:
void RTC_getTime_Date(void)
{
  segundos = RTC_GetTime (LPC_RTC, RTC_TIMETYPE_SECOND);
  minutos = RTC_GetTime (LPC_RTC, RTC_TIMETYPE_MINUTE);
  horas = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_HOUR);
  
  dia = RTC_GetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH);
  mes = RTC_GetTime (LPC_RTC, RTC_TIMETYPE_MONTH);
  ano = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_YEAR);
	
	limpiardisplay();
  
	sprintf(tiempo,"%.2d:%.2d:%.2d", horas, minutos, segundos);
	EscribeTiempo(tiempo);
	
	sprintf(fecha,"%.2d/%.2d/%.2d",dia, mes, ano);
	EscribeFecha(fecha);
	
  //EscribeTiempo(horas, minutos, segundos);
  //EscribeFecha(dia, mes, ano);
}

//Rutina de atencion a la interrupcion del RTC:
void RTC_IRQHandler(void)
{
	if (RTC_GetIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE))
	{		
		osSignalSet (tid_ThreadLCD, EscrituraLCD);
		RTC_ClearIntPending(LPC_RTC, RTC_INT_COUNTER_INCREASE); //Bajamos el flag.
	}

	if (RTC_GetIntPending(LPC_RTC, RTC_INT_ALARM))
	{
    osSignalSet (tid_ThreadLED4, AlarmaLed4);
		RTC_ClearIntPending(LPC_RTC, RTC_INT_ALARM); //Bajamos el flag.
	}
}

/*-------------------------MAIN FUNCTION------------------------------*/
/*********************************************************************//**
 * @brief		c_entry: Main RTC program body
 * @param[in]	None
 * @return 		int
 **********************************************************************/
void c_entry(void)
{
	
		RTC_TIME_Type RTCFullTime;

	/* Initialize debug via UART0
	 * – 115200bps
	 * – 8 data bit
	 * – No parity
	 * – 1 stop bit
	 * – No flow control
	 */
		/* RTC Block section ------------------------------------------------------ */
	// Init RTC module
	RTC_Init(LPC_RTC);

    /* Disable RTC interrupt */
    NVIC_DisableIRQ(RTC_IRQn);
    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(RTC_IRQn, ((0x01<<3)|0x01));

	/* Enable rtc (starts increase the tick counter and second counter register) */
	RTC_ResetClockTickCounter(LPC_RTC);
	RTC_Cmd(LPC_RTC, ENABLE);
	RTC_CalibCounterCmd(LPC_RTC, DISABLE);

	/* Set current time for RTC */
	// Current time is 8:00:00PM, 2009-04-24
	
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, 0);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MINUTE, 0);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_HOUR, 20);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MONTH, 3);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_YEAR, 2022);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, 24);

//  RTC_setTime_Date();

	/* Set ALARM time for second */
	RTC_SetAlarmTime (LPC_RTC, RTC_TIMETYPE_SECOND, 0);// Alarma cada 1 min. que es 0 segundos
	//RTC_SetAlarmTime (LPC_RTC, RTC_TIMETYPE_MINUTE, 0);
	

	// Get and print current time
	RTC_GetFullTime (LPC_RTC, &RTCFullTime);
	
 	/* Set the CIIR for second counter interrupt*/
	RTC_CntIncrIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, ENABLE);
	/* Set the AMR for 10s match alarm interrupt */
	RTC_AlarmIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, ENABLE);

  /* Enable RTC interrupt */
  NVIC_EnableIRQ(RTC_IRQn);

    /* Loop forever */
    //while(1);

}

/*void RTC_setTime_Date()
{
 	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, tiempo_SNTP.tm_sec);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MINUTE, tiempo_SNTP.tm_min);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_HOUR,tiempo_SNTP.tm_hour+2);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MONTH, tiempo_SNTP.tm_mon+1);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_YEAR, tiempo_SNTP.tm_year+1900);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, tiempo_SNTP.tm_mday);
}*/

/*void RTC_JOY()
{
 	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, 0);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MINUTE, 0);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_HOUR,0);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MONTH, 1);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_YEAR, 2000);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, 1);
}*/

