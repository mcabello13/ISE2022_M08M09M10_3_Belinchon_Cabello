#ifndef _LCD_H_
#define _LCD_H_

#define PORT_PIN 0
#define PIN_A0 6
#define PIN_CS 18
#define PIN_RESET 8

extern void retardo(unsigned int microsegundos);
extern void init(void);
extern void wr_data(unsigned char data);
extern void wr_cmd(unsigned char cmd);
extern void LCD_reset(void);
extern void limpiardisplay(void);
extern void borraL1(void);
extern void borraL2(void);
extern void copy_to_lcd(void);
extern int EscribeLetra_L1(unsigned char letra );
extern int EscribeLetra_L2(unsigned char  letra);
extern void EscribeFraseL1(const char *frase1);
extern void EscribeFraseL2(const char *frase2);
extern void EscribeEntero(int entero);
extern void EscribeFloat(float decimal);
extern void EscribeReloj(int tiempo);
extern void EscribeTiempo(char tiempo[]);
extern void EscribeFecha(char fecha[]);
extern void EscribeGanancia(char ganancia);		

#endif
