/************************************
Modulo para configurar o display LCD
Tutorial LPC2378-gcc parte 4
*************************************/
#include <arch/nxp/lpc23xx.h>
#include "lcd.h"

/* Espera t milisegundos */
void espera(int t)
{
int tf;
tf = T0TC + t;          /* tf = Valor futuro do T0TC */
while(tf != T0TC);      /* espera ate que x==T0TC */
}

/* Escreve um comando para o LCD */
void LCDcomando(int c)
{
FIO3PIN0 = c;
FIO4CLR = LCD_RS;
FIO4SET = LCD_E;
c++; c--;
FIO4CLR = LCD_E;
espera(20);
}

/* Escreve um caractere no LCD */
void LCDputchar(int c)
{
FIO3PIN0 = c;
FIO4SET = LCD_RS;
FIO4SET = LCD_E;
c++; c--;
FIO4CLR = LCD_E;
espera(8);
}

/* Configura Timer e LCD */
void LCDinit(void)
{
/* Configura portas I/O do LCD */
FIO3DIR |= 0xff;
FIO4DIR  |= LCD_E | LCD_RS;
espera(20);
LCDcomando(0x38);	/* Configura LCD para 2 linhas */
LCDcomando(1);		/* Limpa display */
LCDcomando(0x0c);	/* Apaga cursor */
}

/* Envia uma mensagem no LCD */
void LCDputs(char *txt)
{
while(*txt) LCDputchar(*txt++);
}
