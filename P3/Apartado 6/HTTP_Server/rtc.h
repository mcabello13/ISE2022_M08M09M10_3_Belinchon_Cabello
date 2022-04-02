#include "cmsis_os.h"  

//Puertos y Pines de los LED:
#define PUERTO_LED 1
#define LED1 18
#define LED2 20
#define LED3 21
#define LED4 23

//Señales:
#define AlarmaLed4 0x001
#define EscrituraLCD 0x002
#define Led3 0x004
#define lecturaHoraSNTP 0x008
#define senalCenR	0x010
#define senalCenF	0x020
#define guardarLeds 0x040

//Variables:
extern int segundos;
extern int minutos;
extern int horas;
extern int dia;
extern int mes;
extern int ano;
extern char cadenaReloj [20+1];
extern char cadenaFecha [20+1];

//Funciones:
extern void c_entry(void);
extern void RTC_getTime_Date(void);
extern int Init_Thread(void);
extern void RTC_setTime_Date(void);
extern void RTC_JOY(void);   
