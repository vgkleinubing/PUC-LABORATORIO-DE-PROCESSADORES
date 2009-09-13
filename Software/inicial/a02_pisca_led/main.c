/************************************************
Experiencia 1: Faz piscar os LEDs
LabProc 2009-1
Marcos Augusto Stemmer
*************************************************/
#include <arch/nxp/lpc23xx.h>
#define SW1 0x100
#define SW2 0x200

/* A rotina espera tem um loop para gastar tempo */
/* Esta maneira de esperar nao e' recomendada porque o tempo nao pode ser determinado */
void espera(long int k)
{
volatile int x;
while(k--){
	x++; x--;
	}
}

int main (void)
{
int mask, dir;
FIO4DIR = 0xff;	/* Bits 0 a 7 ligados nos LEDs: devem ser saidas */
FIO2DIR = 1;
dir=mask=0;
do	{
	espera(30000);
	FIO2CLR = 1;
	espera(30000);
	FIO2SET = 1;
	if(!(FIO4PIN & SW1)) dir=1; /* Botao SW1: Faz deslocar para a direita */
	if(!(FIO4PIN & SW2)) dir=0; /* Botao SW2: Faz deslocar para a esquerda */
	FIO4SET = 0xff;	/* Apaga todos os LEDs */
	FIO4CLR = mask;	/* Liga um LED da sequencia */
	if(dir)	{
		mask >>= 1;	/* Desloca LED ligado para a direita */
		if(!mask) mask = 0x80;	/* Volta para o inicio */
		}
	else	{
		mask <<= 1;	/* Desloca para a esquerda */
		if(!(mask & 0xff)) mask=1;
		}
	} while(1);
return 0;
}

/* Estas rotinas sao chamados pelo crt.S. 
Devem existir, mas ainda nao estao sendo usadas */
void UNDEF_Routine(){}
void SWI_Routine(){}
void FIQ_Routine(){}
