/************************************************
Monitor LPC2378
*************************************************/
#include <arch/nxp/lpc23xx.h>
#include "monarm.h"
/* LCD_E em P4.24 */
#define LCD_E	0x01000000
/* LCD_RS em P4.25 */
#define LCD_RS	0x02000000

void espera(long int k)
{
volatile int x;
while(k--){
	x++; x--;
	if(U0LSR & 1) break;
	}
}

void LCDcomando(int c)
{
FIO3PIN0=c;
FIO4CLR=LCD_RS;
espera(8);
FIO4SET=LCD_E;
espera(8);
FIO4CLR=LCD_E;
espera(800);
}

void LCDputchar(int c)
{
while(!(U0LSR & THREMPTY));
U0THR = c;
FIO3PIN0=c;
FIO4SET=LCD_RS;
espera(8);
FIO4SET=LCD_E;
espera(8);
FIO4CLR=LCD_E;
espera(800);
}

void LCDinit(void)
{
FIO3DIR |= 0xff;	/* P3.0 a P3.7 como saidas (dados do LCD) */
FIO4DIR=LCD_E | LCD_RS;
FIO4CLR=LCD_E;
espera(4000);
LCDcomando(0x38);
LCDcomando(1);
LCDcomando(0x0e);
}

void LCDprompt(void)
{
LCDcomando(1);
LCDputs("FENG   ADC0=\r\n");
LCDcomando(0xc0);
LCDputs("PUCRS  ");
}

void LCDputs(char *s)
{
while(*s && !(U0LSR & 1)) LCDputchar(*s++);
}
