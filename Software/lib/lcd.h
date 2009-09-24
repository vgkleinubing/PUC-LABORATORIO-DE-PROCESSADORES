/******************************************
Cabecalho da configuracao do LCD
M. Stemmer 2009-2
*******************************************/
/* Display LCD */
#define LCD_E	0x01000000
#define LCD_RS	0x02000000
#define LCD_D	0x000000ff
#define CRYSTALFREQ 12000000
#include "lcd.c"
/* Modulo lcd.c */
void LCDinit(void);
void LCDcomando(int c);
void LCDputchar(int c);
void LCDputs(char *s);
void espera(int t);
