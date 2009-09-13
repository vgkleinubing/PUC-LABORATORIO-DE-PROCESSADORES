/*************************************
Escreve no LCD usando interrupcao do timer 1
*******************************************/
#include <arch/nxp/lpc23xx.h>
#include "uart.h"
/* No vic_cpsr.h tem declaracoes importantes para ativar as interrupcoes */
#include "vic_cpsr.h"
#define TAMFILA 0x1ff
#define LCD_E 1
#define LCD_RS 2
/* Se CLOCKRTC for definido usa-se oscilsdor espencial de 32768Hz */
/* Se nao for definido usa o clock de 12MHz -*/
#define CLOCKRTC

/* Fila para escrever no LCD usando a IRQ do Timer0 */
short LCDfila[TAMFILA+1];
int LCDini, LCDfim;
volatile int ctempo=0;

/* A rotina de atendimento da IRQ do RTC
  Pega caracteres da fila e escreve no display LCD
  Pisca sequencialmente os LEDs
*/
void IRQ_RTC(void) __attribute__ ((interrupt("IRQ")));

void IRQ_RTC(void)
{
short k;
if((LCDini!=LCDfim) && !ctempo--) {
	FIO3DIR |= 0xff;
	k = LCDfila[LCDini++];	/* Pega caractere k da fila	*/
	if(k<0) { ctempo=8; FIO4CLR3=LCD_RS; }	/* Se k for negativo e' comando	*/
	else { FIO4SET3=LCD_RS; /*U0THR=k;*/ }	/* Caractere	*/
	FIO3PIN0=k;	/* Escreve dados	*/
	FIO4SET3=LCD_E;	/* Pulsa E	*/
	LCDini &= TAMFILA;
	FIO4CLR3=LCD_E;
	}
if(ctempo < 0) ctempo=0;
k=ILR;		/* Le o identificador de IRQ do RTC */
ILR = k;	/* Limpa a IRQ do RTC */
VICVectAddr = 0;
}

/* Configura o RTC para gerar 1024 IRQ por segundo */
void LCDinit()
{
FIO4DIR |= (LCD_E + LCD_RS) << 24;
FIO4CLR3 = LCD_E;
ctempo=40;
#ifdef CLOCKRTC
CCR=0x11;
#else
PREINT =  (CRYSTALFREQ >> 17)-1;
PREFRAC = ((CRYSTALFREQ >> 2) & 0x7fff);
CCR=1;
#endif
CISS = 0x82;	/* Ativa RTC IRQ */
ILR=0;
ctempo=0;
desabilitaIRQ();
VICVectAddr13 = (int)IRQ_RTC;
VICIntSelect &= ~0x2000;
VICIntEnable |= 0x2000;		/* Habilita IRQ do RTC */
habilitaIRQ();

LCDcomando(0x38);	/* Inicializa display LCD */
LCDcomando(1);		/* Limpa */
LCDcomando(0x0c);	/* Desliga cursor */
}

short LCDcol, lcdca;
/* Coloca um caractere no fim da fila para escrita no LCD */
void LCDputchar(int c)
{
if((lcdca ^ LCDcol)&0x10) LCDcomando(0x6 + (LCDcol > 14));
LCDfila[LCDfim++]=c;
lcdca=LCDcol;
LCDcol++;
LCDfim &= TAMFILA;
}

/* Coloca um comando na fila do LCD */
void LCDcomando(int c)
{
lcdca=LCDcol;
if(c & 0x80) LCDcol=(c & 0x1f);
switch(c){
	case 1: LCDcol=0; break;
	case 0x10: if(LCDcol) LCDcol--; break;
	case 0x14: LCDcol++; break;
	}
LCDfila[LCDfim++]= c-256;
LCDfim &= TAMFILA;
}

/* Escreve um texto no LCD */
void LCDputs(char *s)
{
while(*s) LCDputchar(*s++);
}
