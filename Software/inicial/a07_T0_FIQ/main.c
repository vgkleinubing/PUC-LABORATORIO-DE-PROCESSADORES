/************************************************
Experiencia 07: Interrpcao FIQ
Sequencial de LEDs na placa de labproc 2009-2
Marcos Augusto Stemmer
*************************************************/

#include <arch/nxp/lpc23xx.h>
#include "uart.h"
/* No vic_cpsr.h tem declaracoes importantes para ativar as interrupcoes */
#include "vic_cpsr.h"

int main (void)
{
char c;
U0init();	/* Configura a porta serial UART0 */
U0puts("\r\nDemonstra Timer 0 com Interrupcao tipo FIQ - Executando na ");
#ifdef RAM_RUN		/* Dedinir externamente RAM_RUN para executar na RAM */
U0puts("RAM\r\n");
MEMMAP = 2;	/* 2 Mapeia para RAM */
#else
U0puts("FLASH\r\n");
MEMMAP = 1;	/* 1 Mapeia vetores para a FLASH */
#endif
FIO4DIR = 0xff;	/* Configura LEDs como saida	*/
FIO4SET = 0xff;		/* LEDs iniciam apagados */
T0TCR = 2; T0TCR=0;	/* Limpa e habilita T0 */
/* Preescalonador do T0 para incrementar 32 vezes por segundo */
T0PR = (SYSCLK / 256);
T0TCR=1;
/* Configura T0 para gerar int na contagem 8 e 24 e resetar em 32 */
T0MCR=0x89;	/* Interrompe em T0MR0 e T0MR1; Reset em T0MR2 */
T0IR=3;		/* Interrompe quando encontrar T0MR0, T0MR1 */
T0MR0 = 7;	/* Interrompe quando T0TC==8	*/
T0MR1 = 23;	/* Interrompe quando T0TC==24	*/
T0MR2 = 31;	/* Reinicia quando T0TC==32	*/
/* Habilita a interrupcao do Timer0 como FIQ no VIC	*/
VICIntSelect = 0x10;	/* Timer0=bit 4 como FIQ	*/
VICIntEnable = 0x10;	/* Habilita int do Timer0	*/
/* Fica em loop. So faz alguma coisa na interrupcao	*/
habilitaFIQ();	/* Definida no #include "vic_cpsr.h"	*/
while(1) {	/* Entra num loop le-escreve	*/
	c=U0getchar();
	U0putchar(c);
	}	/* A interrupcao faz piscar o LED */
return 0;
}

/* Estas rotinas sao chamados pelo crt.s. 
Devem existir, mas ainda nao estao sendo usadas */
void UNDEF_Routine(){}
void SWI_Routine(){}

/* A rotina de atendimento da FIQ 
  Pisca LEDs em sequencia
  Liga o LED quando a contagem atinge MR0
  Desliga o LED quando a contagem atinge MR1
*/
void FIQ_Routine()
{
int k;
static int m = 0x80;
k=T0IR;		/* T0IR identifica quem causou a int	*/
if(k & 1) { /* Interrompeu no T0MR0	*/
	FIO4CLR = m; /* T0MR0: Liga LED	*/
	}
if(k & 2) { /* Interrompeu no T0MR1	*/
	FIO4SET = m; /* Desliga LED */
	m >>= 1;
	if(!m) m=0x80;
	}
T0IR = k;
}
