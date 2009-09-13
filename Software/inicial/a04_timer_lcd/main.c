/************************************************
Experiencia 4: Configura o Timer T0 e display LCD
LabProc 2009-2
Marcos Augusto Stemmer
*************************************************/
#include <arch/nxp/lpc23xx.h>
#define CRYSTALFREQ 12000000
#define LCD_E	(1<<24)
#define LCD_RS	(1<<25)

/* Espera t milisegundos */
void espera(unsigned int t)
{
unsigned int tf;
tf = T0TC + t;          /* tf = Valor futuro do T0TC */
while(tf != T0TC);      /* espera ate que x==T0TC */
}

/* Escreve um comando para o LCD */
void LCDcomando(int c)
{
FIO3PIN0 = c;
FIO4CLR = LCD_RS;
FIO4SET = LCD_E;
c++; c--;
FIO4CLR = LCD_E;
espera(20);
}

/* Escreve um caractere no LCD */
void LCDputchar(int c)
{
FIO3PIN0 = c;
FIO4SET = LCD_RS;
FIO4SET = LCD_E;
c++; c--;
FIO4CLR = LCD_E;
espera(8);
}

/* Configura Timer e LCD */
void LCDinit(void)
{
/* Configura portas I/O do LCD */
FIO3DIR |= 0xff;
FIO4DIR  |= LCD_E | LCD_RS;
espera(20);
LCDcomando(0x38);	/* Configura LCD para 2 linhas */
LCDcomando(1);		/* Limpa display */
LCDcomando(0x0c);	/* Apaga cursor */
}

/* Envia uma mensagem no LCD */
void LCDputs(char *txt)
{
while(*txt) LCDputchar(*txt++);
}

void escreve2digs(int x)
{
LCDputchar((x/10) + '0');	/* Escreve dezena  */
LCDputchar((x%10) + '0');	/* Escreve unidade */
}

int main (void)
{
int c=0, tempo;
T0TCR = 0;              /* Desabilita T0 */
T0PR = CRYSTALFREQ/1000 - 1; /* Incrementa o contador T0TC 1000 vezes por segundo */
T0TCR = 2;      /* Inicializa T0 */
T0TCR = 1;      /* Habilita T0  */
FIO4DIR = 1;	/* LED1 como saida */
LCDinit();
LCDputs("Teste do Timer");
LCDcomando(0xc0);
LCDputs("e LCD:");
tempo=T0TC;
do	{
	if(T0TC == tempo+600) FIO4CLR = 1;	/* Liga LED em 600ms */
	if(T0TC == tempo+1000){
		FIO4SET = 1;		/* Desliga em 1000ms */
		tempo += 1000;
		LCDcomando(0xc7);
		escreve2digs(c++);	/* Escreve contagem a cada segundo */
		if(c==100) c=0;
		}
	} while(1);
return 0;
}

/* Estas rotinas sao chamados pelo crt.S. 
Devem existir, mas ainda nao estao sendo usadas */
void UNDEF_Routine(){}
void SWI_Routine(){}
void FIQ_Routine(){}
