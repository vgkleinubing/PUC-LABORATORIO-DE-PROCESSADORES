/************************************************
Módulo uart.c: Configura UART
Marcos Augusto Stemmer
*************************************************/
#include <arch/nxp/lpc23xx.h>
#include "uart.h"
/* Configuracao da Porta Serial 0 */
void U0init(void)
{
PINSEL0 |= 0x50;	/* Seleciona pinos TxD0 e RxD0 */
U0FCR   = 0x7;		/* Habilita as FIFOs e reset */
U0LCR   = 0x83;		/* Habilita acesso ao divisor de baud-rate (DLAB) */
U0DLL   = ((SYSCLK/BAUDRATE+8) >> 4) & 0xff;
U0DLM   = ((SYSCLK/BAUDRATE) >> 12) & 0xff;
U0LCR    = 0x03;	/* Configura UART0 como 8N1 */
}

/* Recebe um caractere da porta serial */
int U0getchar(void)
{
while((U0LSR & 1) == 0);	/* Espera ate receber algo */
return U0RBR;
}

/* Envia um caractere */
void U0putchar(int c)
{
while((U0LSR & 0x20) == 0);	/* Tespera transmissor disponivel */
U0THR = c;	/* Transmite */
}

/* Envia uma mensagem pela UART0 */
void U0puts(char *txt)
{
while(*txt) U0putchar(*txt++);
}
