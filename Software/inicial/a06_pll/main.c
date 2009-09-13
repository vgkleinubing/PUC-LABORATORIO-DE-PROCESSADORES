/************************************************
Experiencia 6: 
Configura o clock e pll, permitindo fazer o
microcontrolador operar com diferentes frequencias de clock
Marcos Augusto Stemmer
*************************************************/

#include <arch/nxp/lpc23xx.h>
#include "uart.h"

/* Le uma linha de texto. n=numero maximo de letras */
int GetStr(char *s, int n)
{
int i;
char c;
i=0;
do {
   c=U0getchar();
   if(c==0x7f) c=8;
   if(c==8) { 
	if(i)	{
		U0putchar(c); i--;
		U0putchar(' '); U0putchar(8);
		} 
	}
   else {
	if(c>=' ') s[i++]=c; U0putchar(c);
	}
   if(c=='\x1b') { *s='\0'; return 1; }
   } while(c!=10 && c!=13 && i < n);
if(c==13) U0putchar('\n');
s[i]=0;
return i;
}

/* Pega um numero na base radix */
unsigned long getnum(char *str, int radix)
{
unsigned long x;
int d;
x=0;
do{
  d = *str++;
  if(d < '0') break;
  if(d > '9') {
    d &= 0xdf;
    if(d < 'A') break;
    d -= ('A'-'9' -1);
    }
  d-='0';
  if(d >= radix) break;
  x = x*radix + d;
  } while(1);
return x;
}

void PrDig(char c)
{
c = '0' + (c & 0xf);
if(c > '9') c+='A'-'9'-1;
U0putchar(c);
}

void PrByte(char c)
{
PrDig(c >> 4); PrDig(c);
}

/* Escreve um numero como 8 digitos hexadecimais */
void prword(unsigned int w)
{
PrByte(w >> 24); PrByte(w >> 16);
PrByte(w >> 8); PrByte(w);
}

/* Escreve um numero decimal */
void escrevenum(int num)
{
char digs[16];
int nd=0;
if(num<0) { U0putchar('-'); num=-num; }
do	{
	digs[nd++]= (num % 10) + '0';
	num /= 10;
	} while(num);
while(nd) U0putchar(digs[--nd]);
}

int main (void)
{
int cpuclk;
char buf[32];
int pllm=16,cpud=16;
T0TCR =	0;		 /* Desabilita T0 */
 /* Prescaler para incrementar o T0TC 1000 vezes por segundo */
T0PR = CRYSTALFREQ/8	- 1;
T0TCR =	2;		/* Inicializa T0 */
T0TCR =	1;		/* Habilita T0 */
FIO4DIR=0xff;	/* LEDs como saidas */
FIO4SET=0xff;
do	{
	cpuclk=PLL_Init(pllm,cpud);	/* Configura o sistema de clock */
	U0init(BAUDRATE);	/* Configura a porta serial U0 */
	cpud=T0TC; while(cpud==T0TC);	/* Espera estabilizar */
	U0puts("\r\nLab. Processdores PUCRS 2009/2\r\n");
	U0puts("Teste da configuracao do PLL\r\n");
	U0puts("PLLSTAT = ");prword(PLLSTAT);
	pllm = ((PLLSTAT & 0x1ff)+1)*2;
	U0puts("  PLLMULTI="); escrevenum(pllm);
	if(PLLSTAT & (1<<26)) {
		U0puts("\r\nCCOFREQ = "); 
		escrevenum(24*pllm);
		U0puts("MHz");
		}
	else U0puts("\r\nO PLL nao conseguiu estabilizar");
	U0puts("\r\nCPUCLK = "); escrevenum(cpuclk/1000);
	U0puts("kHz = (12MHz)*PLLMUTI/CPUDIV\r\n");
	do	{
		U0puts("\nPLLMULTI (par de 12 a 24)=");
		GetStr(buf,30);
		pllm = getnum(buf,10);
		} while((pllm & 1) || pllm < 12 || pllm > 24);
	do	{
		U0puts("CPUDIV(deve ser par 2 a 64)=");
		GetStr(buf,30);
		cpud = getnum(buf,10);
		} while((cpud & 1) || cpud < 2 || cpud > 64);
	} while(1);
return 0;
}

/* Estas rotinas sao chamados pelo crt.s
Devem existir, mas ainda nao estao sendo usadas */
void UNDEF_Routine(){}
void SWI_Routine(){}
void FIQ_Routine(){}
