/************************************************
Configura o PWM, modulando com uma funcao senoidal
A saida do PWM aparece no LED2
Marcos Augusto Stemmer
*************************************************/

#include <arch/nxp/lpc23xx.h>
#include "uart.h"

/* Espera t milisegundos */
void espera(int	t)
{
unsigned int tf;
tf = T0TC + t;		/* tf = Valor futuro do T0TC */
while(tf != T0TC)	/* espera ate que x==T0TC */
	if(U0LSR & RXDREADY) break;
}

/* Funcao senoidal com amplitude 510 e 256 amostras por periodo */
int seno(int k)
{
int x,y;
/* A tabela tem 1/4 de periodo, com 64 amostras */
static short tabela[]={  6, 19, 31, 44, 56, 69, 81, 93,106,118,130,142,154,166,178,189,201,212,224,235,246,257,
268,278,289,299,309,319,328,338,347,356,365,374,382,390,398,
406,413,421,427,434,441,447,453,458,464,469,474,478,482,486,
490,493,496,499,501,504,505,507,508,509,510,510};
x = k & 0x3f;	/* x=indice na tabela */
if(k & 0x40) x=0x3f-x;	/* No segundo e quarto quadrante complementa x */
y = tabela[x];
if(k & 0x80) y=-y;	/* No terceiro e quarto quadrante y e' negativo */
return y;
}

/* Escreve um numero decimal */
void escreve_num(int num)
{
char digs[16];
int nd=0;
if(num<0) { UART0putchar('-'); num=-num; }
do {
   digs[nd++]= (num % 10) + '0';
   num /= 10;
   } while(num);
while(nd--) UART0putchar(digs[nd]);
UART0putchar(' ');
}

int main (void)
{
int x,y;
UART0_Init();	/* Configura a porta serial UART0 */
UART0puts("\r\nLab. Processdores PUCRS 2008/2\r\nTeste do PWM\r\n");
T0TCR =	0;		 /* Desabilita T0 */
 /* Prescaler para incrementar o T0TC 200 vezes por segundo */
T0PR = CRYSTALFREQ/200	- 1;
T0TCR =	2;		/* Inicializa T0 */
T0TCR =	1;		/* Habilita T0 */

PINSEL7|= 0x30000;	/* Seleciona o pino P3.24 (LED2) como PWM */
/* Contadores do PWM a 100000 contagens por segundo */
PWM1PR = CRYSTALFREQ/100000-1;
PWM1PCR=0x0200;	/* Habilita PWM1 e configura como single-edge */
PWM1TCR=0x09;	/* Habilita para contagem em modo PWM */
PWM1CTCR=0x00;	/* Modo timer */
PWM1MR0=1023;	/* Periodo: 1024 contagens */
PWM1MR1=500;	/* Borda de descida do PWM1 */
PWM1LER=0x03;	/* Habilita Latch para alterar MCR0 e MCR1 */
x=0;	/* x = indice da senoidal modulante */
/* Varia o duty-cycle de acordo com a envoltoria senoidal */
do	{
	espera(1);
	x++;
/* Aprertando o botao SW1 duplica a frequencia */
	if(!(FIO1PIN & (1<<19))) x++;
	y= 512+seno(x);	/* Componente DC de 512 para y ser sempre positivo */
	PWM1MR1=y;	/* y varia de 2 a 1022 */
	PWM1MR0=1023;
	PWM1LER=3;
	} while(1);
return 0;
}

/* Estas rotinas sao chamados pelo crt.s. 
Devem existir, mas ainda nao estao sendo usadas */
void UNDEF_Routine(){}
void SWI_Routine(){}
void FIQ_Routine(){}
