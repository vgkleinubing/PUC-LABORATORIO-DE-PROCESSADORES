#include <arch/nxp/lpc23xx.h>
#include "lcd.h"

void espera(int t)
{
t = T0TC+t;
while(T0TC < t);
}

void LCDcomando(int c)
{
espera(5);	/* Espera 5ms */
FIO3CLR = 0xff;	/* Zera os dados do LCD */
FIO3SET = c & 0xff;
FIO4CLR = LCD_RS;	/* RS=0: Comando */
FIO4SET = LCD_E;	/* Liga LCD_E */
c++; c--;		/* tempo */
FIO4CLR = LCD_E;	/* Desliga LCD_E */
}

void LCDputchar(int c)
{
espera(5);	/* Espera 1ms */
FIO3CLR = 0xff;	/* Zera os dados do LCD */
FIO3SET = c & 0xff;
FIO4SET = LCD_RS;	/* RS=1: Caractere ASCII */
FIO4SET = LCD_E;	/* Liga LCD_E */
c++; c--;		/* tempo */
FIO4CLR = LCD_E;	/* Desliga LCD_E */
}
/* Escreve texto no LCD */
void LCDputs(char *txt)
{
while(*txt) LCDputchar(*txt++);
}

void LCDinit(void)
{
FIO4DIR = LCD_E + LCD_RS;	/* Controle do LCD na P4 */
FIO4CLR = LCD_E;
FIO3DIR = 0xff;		/* Dados do LCD como saidas */
T0TCR = 0;              /* Desabilita T0 */
T0PR = CRYSTALFREQ/1000 - 1; /* Incrementa o contador T0TC 1000 vezes por segundo */
T0TCR = 2;      /* Inicializa T0 */
T0TCR = 1;      /* Habilita T0  */
LCDcomando(0x38);	/* Configura LCD */
LCDcomando(1);		/* Limpa o display */
LCDcomando(14);		/* Liga cursor */
}
