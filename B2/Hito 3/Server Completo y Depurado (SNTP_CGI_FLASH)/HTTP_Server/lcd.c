#include "LPC17xx.h"
#include "GPIO_LPC17XX.h"
#include "PIN_LPC17xx.h"
#include <LPC17xx.H>
#include "SPI_LPC17xx.h"
#include "RTE_Device.h"
#include "Driver_SPI.h"
#include "Arial12x12.h"
#include <stdio.h>
#include <string.h>
#include "lcd.h"

extern ARM_DRIVER_SPI Driver_SPI1; //Sentencias para poder usar SPI
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;

uint8_t buffer[512];
uint8_t posicionL1 = 0;
uint8_t posicionL2 = 0;
int z, r, t, f;


//Funcion que provoca un retardo:
//(para realizar esperas para que el LCD finalice su configuracion o para que finalice su proceso de reset)	
void retardo(uint32_t microsegundos)
{
	
	int i;
	
	for(i=0;i<microsegundos;i++)
	{
		
	}
	
}


//Funcion que configura la interfaz SPI:
void init(void)
{
	
	PIN_Configure(PORT_PIN,PIN_A0,PIN_FUNC_0,PIN_PINMODE_PULLUP,PIN_PINMODE_NORMAL);
	PIN_Configure(PORT_PIN,PIN_RESET,PIN_FUNC_0,PIN_PINMODE_PULLUP,PIN_PINMODE_NORMAL);
	PIN_Configure(PORT_PIN,PIN_CS,PIN_FUNC_0,PIN_PINMODE_PULLUP,PIN_PINMODE_NORMAL);
	
	GPIO_SetDir(PORT_PIN, PIN_A0,GPIO_DIR_OUTPUT);
	GPIO_SetDir (PORT_PIN, PIN_RESET, GPIO_DIR_OUTPUT);
	GPIO_SetDir (PORT_PIN, PIN_CS, GPIO_DIR_OUTPUT);
	
	SPIdrv->Initialize(NULL);//Inicializamos SPI
	SPIdrv->PowerControl(ARM_POWER_FULL);//Modos de potencia de SPI
	SPIdrv->Control(ARM_SPI_MODE_MASTER|ARM_SPI_CPOL1_CPHA1|ARM_SPI_MSB_LSB|ARM_SPI_DATA_BITS(8),20000000);
    //Modo master, CPOL1 y CPHA1
    //8 bits de datos y frecuencia del SCLK 20 MHz
	
	GPIO_PinWrite (PORT_PIN,PIN_RESET,0); //Generamos el pulso del reset
	retardo(100); //Retardo correspondiente
	GPIO_PinWrite (PORT_PIN,PIN_RESET,1); //Activamos el reset
	retardo(100); //Otro retardo antes de comenzar la secuencia de comandos
	
}


//Funcion que escribe un dato en el LCD:
void wr_data(unsigned char data)
{
		
	GPIO_PinWrite(PORT_PIN,PIN_CS,0); //CS = 0
	GPIO_PinWrite(PORT_PIN,PIN_A0,1); //A0 = 1
	
	SPIdrv -> Send(&data, sizeof(data));
	//Escribimos un dato
	
	GPIO_PinWrite(PORT_PIN,PIN_CS,1); //CS = 1
	
}


//Funcion que escribe un comando en el LCD:
void wr_cmd (unsigned char cmd)
{

	GPIO_PinWrite(PORT_PIN,PIN_CS,0); //CS = 0;
	GPIO_PinWrite(PORT_PIN,PIN_A0,0); //A0 = 0;
	
	SPIdrv->Send(&cmd, sizeof(cmd));
	//Escribimos el comando
	
	GPIO_PinWrite(PORT_PIN,PIN_CS,1); //CS = 1;
	
}


//Funcion que envia al LCD un conjunto de operaciones:
void LCD_reset(void)
{

	wr_cmd(0xAE); //Display off
	wr_cmd(0xA2); //Fija el valor de la relación de la tensión de polarización del LCD a 1/9
	wr_cmd(0xA0); //El direccionamiento de la RAM de datos del display es la normal
	wr_cmd(0xC8); //El scan en las salidas COM es el normal
	wr_cmd(0x22); //Fija la relación de resistencias interna a 2
	wr_cmd(0x2F); //Power on
	wr_cmd(0x40); //Display empieza en la línea 0
	wr_cmd(0xAF); //Enciende el display
	wr_cmd(0x81); //Fija el contraste
	wr_cmd(0x17); //Establecer el contraste
	wr_cmd(0xA4); //Todos los pixels en modo normal
	wr_cmd(0xA6); //LCD Display normal

}


//Funcion que pasa la informacion al LCD:
void copy_to_lcd(void)
{
  
	int i;

	wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
	wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
	wr_cmd(0xB0); // Pagina 0
	
	for(i=0;i<128;i++)
	{
		 wr_data(buffer[i]);
	}
	
	wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
	wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
	wr_cmd(0xB1); // Pagina 1
	
	for(i=128;i<256;i++)
	{
		 wr_data(buffer[i]);
	}
	
	wr_cmd(0x00);
	wr_cmd(0x10);
	wr_cmd(0xB2); //Pagina 2
	
	for(i=256;i<384;i++)
	{
		 wr_data(buffer[i]);
	}
	
	wr_cmd(0x00);
	wr_cmd(0x10);
	wr_cmd(0xB3); // Pagina 3
	
	for(i=384;i<512;i++)
	{
		 wr_data(buffer[i]);
	}
  
}


//Funcion que limpia el display:
void limpiardisplay(void)
{
	memset(buffer,0x00,512);
	copy_to_lcd();
}

//Funcion que escribe un caracter en la linea superior:
int EscribeLetra_L1(uint8_t letra)
{
	
	uint8_t i, valor1, valor2;
	uint16_t comienzo=0;
	
	comienzo = 25*(letra - ' ');
	
	for(i=0; i<12; i++)
	{
		valor1=Arial12x12[comienzo+i*2+1];
		valor2=Arial12x12[comienzo+i*2+2];
		
		buffer[i+posicionL1] = valor1;//valores para la pagina 1
		buffer[i+128+posicionL1]= valor2;//valores para la pagina 2
	}
	
	posicionL1=posicionL1+Arial12x12[comienzo];

	return 0;
	
}


//Funcion que escribe una letra en la linea inferior:
int EscribeLetra_L2(uint8_t letra2){
	
	uint8_t i, valor1, valor2;
	uint16_t comienzo = 0;
	
	comienzo = 25*(letra2 - ' ');
  
	for(i=0; i<12; i++)
  {
		valor1 = Arial12x12[comienzo+i*2+1];
		valor2 = Arial12x12[comienzo+i*2+2];
		
		buffer[i+256+posicionL2] = valor1;
		buffer[i+384+posicionL2]= valor2;
		
	}
  
	posicionL2 = posicionL2 + Arial12x12[comienzo];
	//Suma a donde se quedo al escribir la ultima letra y le suma el ancho para
 //que no salgan todas las letras apelotonadas.
  
	return 0;
}


//Funcion que escribe una frase en la linea superior:
void EscribeFraseL1(const char *frase1)
{
	int longitud = strlen(frase1);
	int z;
  
	posicionL1 = 0; //OJO: RESETEAMOS posicionL1 PARA EVITAR QUE EL TEXTO SE VAYA DESPLAZANDO POR TODA LA LINEA CADA VEZ QUE PULSEMOS Send!!!
	
	for(z=0; z<longitud; z++)
  {	
    if(posicionL1<128-12) //OJO: REALIZAMOS ESTO PARA EVITAR EL DESBORDAMIENTO DEL BUFFER!!!
		{
			EscribeLetra_L1(frase1[z]);
			copy_to_lcd();
		}
	}
}


//Funcion que escribe una frase en la linea inferior:
void EscribeFraseL2(const char *frase2)
{
	int longitud = strlen(frase2);
  int z;
	
	posicionL2 = 0; //OJO: RESETEAMOS posicionL2 PARA EVITAR QUE EL TEXTO SE VAYA DESPLAZANDO POR TODA LA LINEA CADA VEZ QUE PULSEMOS Send!!!
	
	for(z=0; z<longitud; z++)
  {
    if(posicionL2<128-12)	//OJO: REALIZAMOS ESTO PARA EVITAR EL DESBORDAMIENTO DEL BUFFER!!!	
		{		
			EscribeLetra_L2(frase2[z]);
			copy_to_lcd();
		}
	}
}


//Funcion que escribe un entero en el LCD:
void EscribeEntero(int entero)
{
	char str[80];
  
	sprintf(str,"Prueba valor1: %d",entero);
	EscribeFraseL1(str);
  copy_to_lcd();
}


//Funcion que escribe un float en el LCD:
void EscribeFloat(float decimal)
{
	char str1[80];
  
	sprintf(str1,"Prueba valor2: %.5f",decimal);
	EscribeFraseL2(str1);
  copy_to_lcd();
}


//Funcion que escribe el cronometro en el LCD:
void EscribeReloj(int tiempo)										
{
	char reloj[80];
	
	posicionL1 = 0;
	
	sprintf(reloj,"Reloj -> %02d:%02d ",tiempo/60, tiempo%60);
	for(r=0;r <strlen(reloj);r++)
	{	
		EscribeLetra_L1(reloj[r]);
	}
	
	copy_to_lcd();
	
}

//--------------------------------------------------------------------

//Funcion que escribe H:M:S en el LCD:
void EscribeTiempo(char tiempo[])										
{
	//char tiempo[80];
	
	posicionL1 = 0;
	
  //sprintf(tiempo,"%.2d:%.2d:%.2d",horas, minutos,segundos);
	for(t=0; t<strlen(tiempo); t++)
	{	
		EscribeLetra_L1(tiempo[t]);
	}
	
	copy_to_lcd();
}

//Funcion que escribe la fecha en el LCD:
void EscribeFecha(char fecha[])										
{
	//char fecha[80];
	
	posicionL2 = 0;
	
  //sprintf(fecha,"%.2d/%.2d/%.2d",dia, mes, ano);
	for(f=0; f<strlen(fecha); f++)
	{	
		EscribeLetra_L2(fecha[f]);
	}
	
	copy_to_lcd();
}

