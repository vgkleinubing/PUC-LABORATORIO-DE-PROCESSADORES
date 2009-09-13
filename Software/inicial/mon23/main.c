/************************************************
Monitor LPC2148
Marcos Augusto Stemmer
*************************************************/

#include <arch/nxp/lpc23xx.h>
#include "monarm.h"

/* Configuracao da Porta Serial 0 (Parecido com o 16550) */
void UART0_Init()
{
PINSEL0 |= 0x50;	/* Seleciona pinos TxD0 e RxD0 */
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

int main (void)
{
int c, tf;
union address ad;
UART0_Init();
/* Reset and enable T0 */
T0TCR = 2; T0TCR=0;
/* Set T0 prescaler to increment every 1/32 second */
T0PR = (CRYSTALFREQ / 32)-1;
T0TCR=1;
LCDinit();
LCDprompt();
ad0config();
UART0puts("\r\nMonitor LPC2378 PUCRS-NXP 0.09\r\n");
if(!(CCR & 0x10)){
	CCR=0; CCR=2; CCR=0;
	CCR=0x11;
	}
do	{
	UART0puts("\r\n>");
	tf=T0TC+16;
	while(!(U0LSR & RXDREADY)){
		if(tf!=T0TC) continue;
		tf=T0TC+32;
		UART0putchar('\r');
		LCDcomando(0x8c);
		escreve_num(le_adc0());
		LCDputchar(' ');
		LCDcomando(0xc8);
		pr2dec(HOUR);
		LCDputchar(':');
		pr2dec(MIN);
		LCDputchar(':');
		pr2dec(SEC);
		UART0putchar('>');
		}
	c=U0RBR;
	if(c > 'Z') c ^= ('A' ^ 'a');
	switch(c){
		case 0x1b: c=UART0getchar();
			if(c==0x5b) c=UART0getchar();
			break;
		case 'C': copy(); break;
		case 'D': dump(); break;
		case 'W': escreve_word(); break;
		case 'X': execute(); break;
		case 'A': acerta_relogio(); break;
		case 'I': IAP_functions(); LCDprompt(); break;
		case 'E': escreve_mem(); break;
		case ':': lehex(0x40000000); UART0putchar('#'); break;
		case '?': case 'H':
			UART0puts("\r\nA Acerta relogio\r\nC Copia\r\nD Dump\r\n"
			"E Escreve na memoria\r\nX eXecuta\r\n"
			"L Le ADC0\r\n"
			"I IAP In Application programming\r\n"
			"P Executa programa em 0x40000000\r\n"
			"W Le/escreve Word (32 bit) na memoria\r\n"
			"<ESC> Cancela ou termina um comando\r\n");
			break;
		case 'P': ad.w=0x40000000; 
			*ad.pw=0xe59ff018;
			(ad.f)(); break;
		case '0': UART0puts("T0 = "); prword(T0TC); break;
		}
	} while(1);
return 0;
}

void UNDEF_Routine(){}
void SWI_Routine(){}
void FIQ_Routine(){}
