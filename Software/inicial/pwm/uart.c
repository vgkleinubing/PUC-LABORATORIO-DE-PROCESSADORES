/************************************************
Experiencia 2: Configura o clock e a porta serial
Módulo uart.c: Configura UART e o clock
Marcos Augusto Stemmer
*************************************************/
#include <arch/nxp/lpc23xx.h>
#include "uart.h"

/* Configuracao da Porta Serial 0 (Parecido com o 16550) */
void UART0_Init()
{
PINSEL0 = 0x50;		/* eleciona pinos TxD0 e RxD0 */
U0FCR   = 0x7;		/* Habilita as FIFOs e reset */
U0LCR   = 0x83;		/* Habilita acesso ao divisor de baud-rate (DLAB) */
U0DLL   = ((CRYSTALFREQ/BAUDRATE) >> 4) & 0xff;
U0DLM   = ((CRYSTALFREQ/BAUDRATE) >>12) & 0xff;
U0LCR    = 3;		/* Desabilita DLAB */
}

char UART0getchar(void)
{
while(!(U0LSR & RXDREADY));
return U0RBR;
}

void UART0putchar(char c)
{
while(!(U0LSR & THREMPTY));
U0THR = c;
}

void UART0puts(char *s)
{
while(*s) UART0putchar(*s++);
}
