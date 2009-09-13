/************************************************
Experiencia 5: Relogio usando o RTC
Usa o display LCD, a UART0 o timer T0 e o relogio RTC
Marcos Augusto Stemmer
*************************************************/

#include <arch/nxp/lpc23xx.h>
#include "lcd.h"
#define SW1	0x100
#define SW2	0x200
#define SW3	0x400
#define SW4	0x800

/* Se CLOCKRTC for definido usa-se oscilsdor espencial de 32768Hz */
/* Se nao for definido usa o clock de 12MHz -*/
#define CLOCKRTC

void escreve2dec(int val)
{
LCDputchar((val / 10)  + '0');
LCDputchar((val % 10) + '0');
}

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
int k,h,m;
T0TCR = 0;              /* Desabilita T0 */
T0PR = CRYSTALFREQ/1000 - 1; /* Incrementa o contador T0TC 1000 vezes por segundo */
T0TCR = 2;      /* Inicializa T0 */
T0TCR = 1;      /* Habilita T0  */

LCDinit();	/* Inicializa o Timer 0 e o display LCD */
LCDputs("RELOGIO PUCRS");
acerta();
FIO4DIR |= 6;	/* LED2=P4.1 e LED3=2 como saida */
FIO4SET = 6;
do	{
/* Faz piscar os LEDs */
	k=T0TC % 500;
	switch(k){
		case 20: FIO4CLR = 2;	/* liga LED2 */
			break;
		case 70: FIO4SET = 2;	/* desliga LED2 */
			break;
		case 200:FIO4CLR = 4;	/* liga LED3 */
			break;
		case 250:FIO4SET = 4;	/* desliga LED3 */
			break;
		}
	if(k) continue;
/* Acerta o relogio usando os botoes */
	k=((~FIO4PIN) & (SW1 | SW2| SW3 | SW4));
	if(k)	{
		h=HOUR; m=MIN;
		switch(k){
			case SW1: h++; if(h>23) h=0; break;	/* SW1 Adianta hora */
			case SW2: h--; if(h<0) h=23; break;	/* SW2 Atrasa hora */
			case SW3: m++; if(m>59) m=0; break;	/* SW3 Adianta minuto */
			case SW4: m--; if(m<0) m=59; break;	/* SW4 Atrasa minuto */
			}
		HOUR=h; MIN=m; SEC=0;
		}
/* Escreve Hora:Minuto:Segundo no display LCD */
	LCDcomando(0xc0);
	escreve2dec(HOUR);
	LCDputchar(':');
	escreve2dec(MIN);
	LCDputchar(':');
	escreve2dec(SEC);
	} while(1);
return 0;
}

/* Estas rotinas sao chamados pelo crt.s. 
Devem existir, mas ainda nao estao sendo usadas */
void UNDEF_Routine(){}
void SWI_Routine(){}
void FIQ_Routine(){}
