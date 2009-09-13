/************************************************
Experiencia 2: Configura o clock, PLL e a porta serial
Módulo uart.c: Configura UART e o clock
Marcos Augusto Stemmer
*************************************************/
#include <arch/nxp/lpc23xx.h>
#include "uart.h"
/* cpuclock e' uma variavel global usada para armazenar a freq. efetiva da cpu */
int cpuclock;

/* Configura o clock com o PLL. */
int PLL_Init(int pllmulti, int cpudiv)
{
int k;
cpudiv &= -2;	/* cpudiv deve ser par */
if(PLLSTAT & (1 << 25)){	/* Se PLL nao estava configurado */
	PLLCON = 1;	/* Habilita PLL, desconectado */
	PLLFEED=0xAA;PLLFEED=0x55;
	}
PLLCON=0;	/* Desconecta e desabilita o PLL */
PLLFEED=0xAA;PLLFEED=0x55;
/* Depois deste comando deve-se usar FIO1PIN para acessar a porta P1 */
SCS = 0x21;	/* Liga o oscilador externo (12MHz) e o GPIO */
while( !(SCS & 0x40) );	/* Espera estabilizar o oscilador */
CLKSRCSEL = 0x1;	/* Usa o 12MHz como oscilador principal */
PLLCFG = (pllmulti/2 - 1);
PLLFEED=0xAA;PLLFEED=0x55;
CCLKCFG=0;	/* Divisor do clock da CPU = 1 */
PLLCON=1;	/* Habilita o PLL	*/
PLLFEED=0xAA;PLLFEED=0x55;
k=80000;	/* No maximo 80000 voltas esperando o PLL lock */
while((!(PLLSTAT & (1<<26))) && k) k--;	/* espera PLL Lock */
/* Se conseguiu PLL Lock, conecta CPU clock ao PLL */
CCLKCFG=cpudiv-1;	/* Divisor por cpudiv na CPU */
if(k)	{
	cpuclock= pllmulti*CRYSTALFREQ/cpudiv;
	PLLCON=3;	/* Conecta o PLL ao clock da CPU */
	}
else	cpuclock=CRYSTALFREQ/cpudiv;
PLLFEED=0xAA;PLLFEED=0x55;
return cpuclock;
}

/* Configuracao da Porta Serial 0 (Parecido com o 16550) */
void U0init(int baudrate)
{
int cpudiv;
PINSEL0 |= 0x50;	/* Seleciona pinos TxD0 e RxD0 */
U0FCR   = 0x7;		/* Habilita as FIFOs e reset */
U0LCR   = 0x83;		/* Habilita acesso ao divisor de baud-rate (DLAB) */
cpudiv = (cpuclock/baudrate + 8) >> 4;
U0DLL   = cpudiv & 0xff;
U0DLM   = (cpudiv >> 8) & 0xff;
U0LCR    = 3;		/* Desabilita DLAB */
}

char U0getchar(void)
{
int k;
static int m;
while(!(U0LSR & 1)){
	if(k!=T0TC){	/* Sequencial de LEDs na P4.0 a P4.7 */
		FIO4SET=0xff;
		FIO4CLR=m;
		m <<= 1;
		if(!(m & 0xff)) m=1;
		k=T0TC;
		}
	}
return U0RBR;
}

void U0putchar(char c)
{
while(!(U0LSR & 0x20));
U0THR = c;
}

void U0puts(char *s)
{
while(*s) U0putchar(*s++);
}
