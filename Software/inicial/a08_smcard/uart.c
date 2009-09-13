/*************
Modulo de acesso a porta serial
Programador: Marcos A. Stemmer
***************/
#include <arch/nxp/lpc23xx.h>
#include "lcd.h"

void U0init(void)
{
PINSEL0 |= 0x50;	/* Configura pinos RxD0 e TxD0 */
U0FCR   = 0x7;
U0LCR   = 0x83;	/* Habilita acesso ao divisor */
U0DLL   = ((CRYSTALFREQ/BAUDRATE+8) >> 4) & 0xff;
U0DLM   = ((CRYSTALFREQ/BAUDRATE) >>12) & 0xff;
U0LCR    = 3;		/* Desabilita DLAB */
}

/* Pega caractere da porta serial */
/* Bit 0 do U0LSR indica que tem dado recebido */
int U0getchar(void)
{
while(!(U0LSR & 1));	/* Espera ate receber dado */
return U0RBR;		/* Retorna o dado recebido */
}

/* Envia caractere pela porta serial */
/* Bit 5 do U0LSR indica que pode transmitir */
void U0putchar(int c)
{
while(!(U0LSR & 0x20));	/* Espera ate ter lugar no transmissor */
U0THR = c;	/* Escreve o dado */
}

/* Escreve texto na porta serial */
void U0puts(char *txt)
{
while(*txt) U0putchar(*txt++);
}
