/************************************************
LabProc 2009-2
a07_RTC_IRQ Testa escrita no LCD usando
a IRQ da fracao de segundo do relogio RTC
Marcos Augusto Stemmer
*************************************************/
#include <arch/nxp/lpc23xx.h>
#include "uart.h"

int main (void)
{
int c;
LCDinit();
U0init();
LCDcomando(0x80);
LCDputs("Escreve no LCD");
do	{	/* Entra num loop le-escreve	*/
	c=U0getchar();
	LCDputchar(c);
	if(c=='\n' || c=='\r'){	/* Se recebe ENTER da UART */
		/* Entao escreve esta mensagem no display LCD */
		LCDcomando(1);
		LCDputs("Usa IRQ do RTC");
		LCDcomando(0xc0);
		LCDputs("Escreve com INT");
		}
	} while(c!=0x1b);

return 0;
}

/* Estas rotinas sao chamados pelo crt.S. 
Devem existir, mas ainda nao estao sendo usadas */
void UNDEF_Routine(){}
void SWI_Routine(){}
void FIQ_Routine(){}
