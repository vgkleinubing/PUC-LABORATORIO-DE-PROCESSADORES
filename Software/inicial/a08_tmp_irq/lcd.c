/************************************
Modulo para configurar o display LCD
Tutorial LPC2378-gcc parte 7
Marcos A. Stemmer
*************************************/
#include <arch/nxp/lpc23xx.h>
#include "lcd.h"
int linha;

/* Configuracao da Porta Serial 0 (Parecido com o 16550) */
void UART0_Init()
{
PINSEL0 = 0x50;	/* Habilita TxD0 e RxD0 */
U0FCR   = 0x7;		/* Habilita as FIFOs e reset */
U0LCR   = 0x83;	/* Habilita DLAB */
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

/* Espera t milisegundos */
void espera(int t)
{
unsigned int tf;
while(t--) { tf=T0TC; while(tf==T0TC); }
}

/* Escreve um caractere no display LCD e na porta serial*/
void LCDputchar(int c)
{
while(!(U0LSR & THREMPTY));     /* Epera liberar o transmissor */
U0THR = c;                      /* Envia pela porta serial */
if(c=='\n') { linha ^= 0x40; LCDcomando(linha); }
if(c < 0x20) return;    /* Se for car. de controle termina aqui */
FIO3DIR |= LCD_D;       /* Dados do LCD como saída */
FIO3MASK = ~LCD_D;      /* Permite escrita apenas nos dados do LCD */
FIO3PIN = c;            /* Escreve dados no LCD */
FIO4SET = LCD_RS;       /* Para caractere LCD_RS=1 */
FIO4SET = LCD_E;        /* E=1 */
c++; c--;               /* Tempo */
FIO4CLR = LCD_E;        /* E=0 */
FIO3MASK = 0;           /* Libera a escrita em todos os bits da P3 */
espera(1);              /* Espera 1ms */
}

/* Escreve um comando no display LCD */
void LCDcomando(int c)
{
FIO3DIR |= LCD_D;       /* Dados do LCD como saída */
FIO3MASK = ~LCD_D;      /* Permite escrita apenas nos dados do LCD */
FIO3PIN = c;            /* Escreve dados no LCD */
FIO4CLR = LCD_RS;       /* Para comando LCD_RS=0 */
FIO4SET = LCD_E;        /* E=1 */
c++; c--;               /* Tempo */
FIO4CLR = LCD_E;        /* E=0 */
FIO3MASK = 0;           /* Libera a escrita em todos os bits da P3 */
espera(2);              /* Espera 2ms */
}

/* Inicializa o display LCD e o timer T0 */
void LCDinit(void)
{
T0TCR = 0;               /* Desabilita T0 */
 /* Prescaler para incrementar o timer a cada milisegundo */
T0PR = CRYSTALFREQ/1000 - 1;
T0TCR = 2; T0TCR=0;       /* Inicializa T0 */
T0TCR = 1;               /* Habilita T0  */
FIO4DIR |= (LCD_RS | LCD_E);    /* LCD_RS e LCD_E como saídas */
FIO3CLR = LCD_E;        /* LCD_E deve ficar normalmente em 0 */
linha=0x80;
espera(10);
LCDcomando(0x38);       /* Configura LCD */
LCDcomando(1);          /* Limpa */
LCDcomando(0x0e);       /* Cursor como _ */
}

void LCDputs(char *s)
{
while(*s) LCDputchar(*s++);
}
