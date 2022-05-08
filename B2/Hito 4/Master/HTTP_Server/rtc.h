#include "cmsis_os.h"  
#include "Driver_I2C.h"

//I2C:
#define SLAVE_I2C_ADDR       	0x28      
#define REG_TEMP    					0x00
#define REG_CONF	    				0x01
#define	REG_THYST   					0x02
#define REG_TOS     					0x03
#define RECIBIDO					    0x0001

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
#define guardarFechaHoraGanancia 0x040
#define enviarSlaveGanancia 0x100
#define enviarSlaveOverload 0x200

//Variables:
extern int segundos;
extern int minutos;
extern int horas;
extern int dia;
extern int mes;
extern int ano;
extern char cadenaReloj [20];
extern char cadenaFecha [20];

//Funciones:
extern void c_entry(void);
extern void RTC_getTime_Date(void);
extern int Init_Thread(void);
extern void RTC_setTime_Date(void);
extern void RTC_JOY(void);   
extern void Init_i2c(void);
