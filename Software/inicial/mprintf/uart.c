
#include <arch/nxp/lpc23xx.h>
#include "uart.h"

/* Configuracao da Porta Serial 0 (Parecido com o 16550) */
void U0init()
{
PINSEL0 |= 0x50;	/* Seleciona pinos TxD0 e RxD0 */
U0FCR   = 0x7;		/* Habilita as FIFOs e reset */
U0LCR   = 0x83;		/* Habilita acesso ao divisor de baud-rate (DLAB) */
U0DLL   = ((CRYSTALFREQ/BAUDRATE + 8) >> 4) & 0xff;
U0DLM   = ((CRYSTALFREQ/BAUDRATE) >>12) & 0xff;
U0LCR    = 3;		/* Desabilita DLAB */
}

char U0getchar(void)
{
while(!(U0LSR & 1));
return U0RBR;
}

void U0putchar(int c)
{
while(!(U0LSR & 0x20));
U0THR = c;
}

void U0puts(char *s)
{
while(*s) U0putchar(*s++);
}

/* Entrada de texto */
char *U0gets(char *txt, int nmax)
{
int k;	/* Numero de caracteres armazenados */
int c;	/* caractere lido */
k=0;
do	{
	c=U0getchar();
	if(c==0x7f) c=8;
	U0putchar(c);
	/* se for backspace retira um caractere do buffer */
	if(c==8 && (k!=0)) { k--; U0putchar(' '); U0putchar(8); }
	else if(k<nmax) txt[k++]=c;
	} while(c!='\n' && c!='\r');
txt[k-1]='\0';
return txt;
}
