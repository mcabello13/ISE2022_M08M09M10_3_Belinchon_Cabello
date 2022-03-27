#include "LPC17xx.h"
#include "lcd.h"
#include "cmsis_os.h"  
#include "rtc.h"  
#include "lpc17xx_rtc.h" 
#include "time.h"
#include "sntp.h"

//Librerias para utilizar RTC obtenidas de Keil uVision:
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
struct tm timeDate;
struct tm ti;

extern struct tm devuelveTiempo(void);
extern struct tm tiempo_SNTP;
extern osThreadId tid_ThreadLCD;    
extern osThreadId tid_ThreadLED4;   
extern void c_entry(void);

/*******************************************************************************************************************************************
* EXPLICACION: la funcion RTC_getTime_Date se obtiene desde el archivo lpc17xx_rtc.c, dicha funcion																				 *
* tiene como parametros el registro LPC_RTC y el valor correspondiente a horas, minutos, segundos, dia,... etc. 													 *
* Estos valores se definen como etiquetas dentro de una estructura typedef definida en el archivo lpc17xx_rtc.h														 *
* Volviendo a la funcion, en esta se realiza un switch-case para identificar que unidad de tiempo se quiere adquirir,											 *
* y para poder hacerlo, realiza una "and" con el contador correspondiente a la unidad de tiempo con su mascara. Por ejemplo:							 *
* para los segundos: #define RTC_SEC_MASK (0x0000003F), por lo tanto realiza --> 	case RTC_TIMETYPE_SECOND:																 *
*																																										 return (RTCx->SEC & RTC_SEC_MASK);									   *
*																																																																	         *																																									 
* Siendo "RTCx->SEC" el contador correspondiente a los segundos de todo el registro del RTC.																						   *																			 
* ¿Porqué la mascara tiene ese valor? --> porque en la Table 515 del Capitulo 27 del manual, nos dicen "5:0" para seconds, de ahi que      *
* para el registro entero, solo coja "3F", que son los seis primeros bits.																															   *																																									
********************************************************************************************************************************************/

void guardarTiempo(struct tm timeDate)
{		
	ti = timeDate;
}

struct tm devuelveTiempo(void)
{
	return ti;
}

//Funcion que obtiene el tiempo y fecha del RTC y lo escribe en el LCD:
void RTC_getTime_Date(void)
{
  segundos = RTC_GetTime (LPC_RTC, RTC_TIMETYPE_SECOND);
  minutos = RTC_GetTime (LPC_RTC, RTC_TIMETYPE_MINUTE);
  horas = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_HOUR);
  
  dia = RTC_GetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH);
  mes = RTC_GetTime (LPC_RTC, RTC_TIMETYPE_MONTH);
  ano = RTC_GetTime(LPC_RTC, RTC_TIMETYPE_YEAR);
	
	timeDate.tm_sec = segundos;
	timeDate.tm_min = minutos;
	timeDate.tm_hour = horas;
	timeDate.tm_mday = dia;
	timeDate.tm_mon = mes;
	timeDate.tm_year = ano;
	
	guardarTiempo(timeDate);
	
	//Una vez hemos obtenido el tiempo, limpiamos el LCD y con "sprintf"... 
 //...escribimos la hora.
	limpiardisplay();
  
	sprintf(tiempo,"%.2d:%.2d:%.2d", horas, minutos, segundos);
	EscribeTiempo(tiempo);
	
	sprintf(fecha,"%.2d/%.2d/%.2d",dia, mes, ano);
	EscribeFecha(fecha);
}

/**********************************************************************************************************************************
* EXPLICACION: ¿Cómo diferenciar entre una interrupcion de la cuenta del reloj y la de alarma? el registro												*
* encargado de comprobar el tipo de interrupcion es el ILR. En la Table 509 del manual se citan los dos bits para									*
* diferenciarlas: RTCCIF para la cuenta del reloj y RTCALF para la alarma. Por lo tanto, llamando a la funcion										*	
* expuesta en el archivo lpc17xx_rtc.c llamada "RTC_GetIntPending", pasamos como parametro el registro LPC_RTC junto 							*
* a "RTC_INT_COUNTER_INCREASE" (que es "RTC_IRL_RTCCIF", que activa el bit cero) o "RTC_INT_ALARM" (que es "RTC_IRL_RTCALF", que  *
* activa el bit uno). Dicha descripcion esta en la Table 509 del manual.																													*
***********************************************************************************************************************************/

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
	RTC_Cmd(LPC_RTC, DISABLE);
	RTC_ResetClockTickCounter(LPC_RTC);
	//RTC_CalibCounterCmd(LPC_RTC, DISABLE);

	/* Set current time for RTC */
	// Current time is 8:00:00PM, 2009-04-24
	
//	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, 0);
//	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MINUTE, 0);
//	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_HOUR, 0);
//	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MONTH, 1);
//	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_YEAR, 2000);
//	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, 1);

  //RTC_setTime_Date();

	/*************************************************************************************************************************
	 * EXPLICACION: Para programar la alarma, simplemente pasamos como parametro a la funcion "RTC_SetAlarmTime" expuesta    *
   * en el archivo lpc17xx_rtc.c, pasamos como parametro la unidad de tiempo para la que queremos que salte la alarma      *
	 * (en nuetro caso los segundos --> RTC_TIMETYPE_SECOND), y tambien pasamos la cifra numerica para la cual queremos que  *
	 * se active, es decir, en nuetro caso se activara cada vez que los segundos sean cero. Este razonamiento es valido      *
	 * para cualquier unidad de tiempo.																																											 *
	 *************************************************************************************************************************/

	RTC_Cmd(LPC_RTC, ENABLE);
	RTC_CalibCounterCmd(LPC_RTC, DISABLE);

	/* Set ALARM time for second */
	RTC_SetAlarmTime (LPC_RTC, RTC_TIMETYPE_SECOND, 0);// Alarma cada 1 min
	//RTC_SetAlarmTime (LPC_RTC, RTC_TIMETYPE_MINUTE, 0);
	
	// Get and print current time
	RTC_GetFullTime (LPC_RTC, &RTCFullTime);
	
 	/* Set the CIIR for second counter interrupt*/
	RTC_CntIncrIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, ENABLE);
	/* Set the AMR for 10s match alarm interrupt */
	RTC_AlarmIntConfig (LPC_RTC, RTC_TIMETYPE_SECOND, ENABLE);

  /* Enable RTC interrupt */
  NVIC_EnableIRQ(RTC_IRQn);
}

void RTC_setTime_Date(void)
{
 	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, tiempo_SNTP.tm_sec);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MINUTE, tiempo_SNTP.tm_min);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_HOUR,tiempo_SNTP.tm_hour);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MONTH, tiempo_SNTP.tm_mon+1);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_YEAR, tiempo_SNTP.tm_year+1900);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, tiempo_SNTP.tm_mday);
}

void RTC_JOY(void)
{
 	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_SECOND, 0);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MINUTE, 0);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_HOUR,0);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_MONTH, 1);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_YEAR, 2000);
	RTC_SetTime (LPC_RTC, RTC_TIMETYPE_DAYOFMONTH, 1);
}

