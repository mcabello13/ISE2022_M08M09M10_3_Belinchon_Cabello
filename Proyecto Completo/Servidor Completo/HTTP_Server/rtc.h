#include "cmsis_os.h"  

//Puertos y Pines de los LED:
#define PUERTO_LED 1
#define LED1 18
#define LED2 20
#define LED3 21
#define LED4 23

//Se�ales:
#define AlarmaLed4 0x001
#define EscrituraLCD 0x002
#define Led3 0x004
#define esc 0x00001
#define lecturaHoraSNTP 0x00002

//Funciones:
extern void c_entry(void);
extern void RTC_getTime_Date(void);
extern int Init_Thread(void);
extern void RTC_setTime_Date(void);
   
