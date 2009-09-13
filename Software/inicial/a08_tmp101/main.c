/************************************************
Experiencia 8: Relogio com termometro
Usa o termometro TMP101 com interface i2c (sem interrupcao)
Versao 2009-1
Marcos Augusto Stemmer
*************************************************/

#include <arch/nxp/lpc23xx.h>
#include "lcd.h"

int letemp(void);
void escrevenum(int);
void ini_i2c(void);

/* Configuracao da Porta Serial 0 (Parecido com o 16550) */
void UART0_Init()
{
PINSEL0 |= 0x50;	/* Seleciona pinos TxD0 e RxD0 */
U0FCR   = 0x7;		/* Habilita as FIFOs e reset */
U0LCR   = 0x83;		/* Habilita acesso ao divisor de baud-rate (DLAB) */
U0DLL   = ((CRYSTALFREQ/BAUDRATE + 8) >> 4) & 0xff;
U0DLM   = ((CRYSTALFREQ/BAUDRATE) >> 12) & 0xff;
U0LCR    = 3;		/* Desabilita DLAB */
}

char UART0getchar(void)
{
while(!(U0LSR & 1));
return U0RBR;
}

void UART0putchar(char c)
{
while(!(U0LSR & 0x20));
U0THR = c;
}

void UART0puts(char *s)
{
while(*s) UART0putchar(*s++);
}

void escreve2dec(int val)
{
LCDputchar((val / 10)  + '0');
LCDputchar((val % 10) + '0');
}


/* Se CLOCKRTC for definido usa-se oscilsdor espencial de 32768Hz */
/* Se nao for definido usa o clock de 12MHz -*/
#define CLOCKRTC

void acerta(void)
{
CCR=0;	/* Para o relogio	*/
if(HOUR>23) HOUR=12;
if(MIN>59) MIN=0;
if(SEC>59) SEC=0;
#ifdef CLOCKRTC
CCR=0x11;	/* Usa o oscilador de 32768Hz do RTC */
#else
PREINT =  (CRYSTALFREQ >> 17)-1;
PREFRAC = ((CRYSTALFREQ >> 2) & 0x7fff);
CCR=1;		/* Usa o clock de 12MHz com pre-escalonador */
#endif
}

int main (void)
{
unsigned int k, mask=0;
short h, m;
UART0_Init();	/* Configura a porta serial UART0 */
LCDinit();
LCDputs("RELOGIO PUCRS\n");
UART0puts("\r\nLab. Processdores PUCRS 2009/2\r\n");
ini_i2c();
if(!CCR & 0x10) acerta();
FIO4DIR|=0xff;
FIO4SET=0xff;
do	{
	if(CTC & 0x7fff) continue;
/* Acerta o relogio usando os botoes */
	mask >>= 1;
	if(!mask) mask=0x80;
	FIO4SET0=0xff;
	FIO4CLR0=mask;
	k=((~FIO4PIN) & (SW1 | SW2| SW3 | SW4));
	if(k)	{
		 h=HOUR; m=MIN;
		switch(k){
			case SW1: h++; if(h>23) h=0; break;	/* Adianta hora */
			case SW2: h--; if(h<0) h=23; break;	/* Atrasa hora */
			case SW3: m++; if(m>59) m=0; break;	/* Adianta minuto */
			case SW4: m--; if(m<0) m=59; break;	/* Atrasa minuto */
			}
		HOUR=h; MIN=m;
		}
	LCDcomando(0xc0);
	UART0putchar('\r');
	escreve2dec(HOUR);
	LCDputchar(':');
	escreve2dec(MIN);
	LCDputchar(':');
	escreve2dec(SEC);
	LCDputs(" T=");
/* Mostra a temperatura */
	escrevenum(letemp() >> 4); 
	LCDputchar(' ');
	} while(1);
return 0;
}

/* Estas rotinas sao chamados pelo crt.s. 
Devem existir, mas ainda nao estao sendo usadas */
void UNDEF_Routine(){}
void SWI_Routine(){}
void FIQ_Routine(){}
