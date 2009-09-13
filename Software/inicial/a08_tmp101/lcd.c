/************************************
Modulo para configurar o display LCD
Tutorial LPC2378-gcc parte 7
Marcos A. Stemmer
*************************************/
#include <arch/nxp/lpc23xx.h>
#include "lcd.h"
int linha;

/* Espera t milisegundos */
void espera(int t)
{
unsigned int tf;
tf = T0TC + t;		/* tf = Valor futuro do T0TC */
while(tf != T0TC)	/* espera ate que x==T0TC */
	if(U0LSR & 1) break;
}

/* Escreve um caractere no display LCD e na porta serial*/
void LCDputchar(int c)
{
while(!(U0LSR & 0x20));	/* Epera liberar o transmissor */
U0THR = c;		/* Envia pela porta serial */
if(c=='\n') { linha ^= 0x40; LCDcomando(linha); }
if(c < 0x20) return;	/* Se for car. de controle termina aqui */
FIO3PIN0 = c;		/* Escreve dados no LCD */
FIO4SET = LCD_RS;	/* Para caractere LCD_RS=1 */
FIO4SET = LCD_E;	/* E=1 */
c++; c--;		/* Tempo */
FIO4CLR = LCD_E;	/* E=0 */
espera(1);	/* Espera 1ms */
}

/* Escreve um comando no display LCD */
void LCDcomando(int c)
{
FIO3PIN0 = c;
FIO4CLR = LCD_RS;	/* Para comando LCD_RS=0 */
FIO4SET = LCD_E;	/* E=1 */
c++; c--;		/* Tempo */
FIO4CLR = LCD_E;	/* E=0 */
espera(2);		/* Espera 2ms */
}

/* Inicializa o display LCD e o timer T0 */
void LCDinit(void)
{
T0TCR = 0;	/* Desabilita T0 */
T0PR = CRYSTALFREQ/1000 - 1;	/* Incrementa T0TC 1000 vezes por segundo */
T0TCR = 2; T0TCR=0;	/* Inicializa T0 */
T0TCR = 1;		/* Habilita T0  */
FIO4DIR |= (LCD_RS | LCD_E);	/* LCD_RS e LCD_E como saídas */
FIO3DIR = 0xff;		/* dados LCD como saidas */
FIO4CLR = LCD_E;
linha=0x80;
espera(10);
LCDcomando(0x38);	/* Configura LCD */
LCDcomando(1);		/* Limpa */
LCDcomando(0x0e);	/* Cursor como _ */
}

void LCDputs(char *s)
{
while(*s) LCDputchar(*s++);
}
