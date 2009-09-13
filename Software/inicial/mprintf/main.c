/************************************************
Teste de ponto flutuante no lpc2378 com gcc
Programa modificado para testar a funcao mprintf
Marcos Augusto Stemmer
*************************************************/
#include <arch/nxp/lpc23xx.h>
#include "uart.h"

/* Calcula raiz quadrada */
double sqrt(double x)
{
double r;
char *pc;
int k;
/* Se x<0 retorna um valor errado, mas serve para teste */
if(x<=0) return x;
r=x;
pc=(char *)&r;
k=((pc[3]<<4) + (pc[2] >> 4) + 1023);
pc[3]=(k >> 5);
pc[2]=((k << 3) & 0xf0);
k=5;
do	{
	r = (x/r + r)/2;
	} while(k--);
return r;
}

int main (void)
{
double x, y;
int i;
char buf[80];
LCDinit();
U0init();
LCDcomando(0x80);
LCDputs("Ponto Flutuante");
LCDcomando(0xc0);
/* escreve saiz de 3 na linha 2 do LCD */
dprint(sqrt(3.0),14,12,LCDputchar);
/* Cabecalho da tabela (formato %s alinhado a direita) */
mprintf(U0putchar,"\r\n%4s%16s\r\n","x","sqrt(x)");
for(x=0.5; x < 5.1; x +=0.5){
	y=sqrt(x);
	mprintf(U0putchar, "%4.1lf|%16.8lf|\r\n", x, y);
	}
U0puts("Formato '-' Alinhado a esquerda\r\n");
for(x=40.0; x < 45.1; x +=0.5){
	y=sqrt(x);
	mprintf(U0putchar, "%4.1lf|%-16.8lf|\r\n", x, y);
	}
do	{
	U0puts("\r\nNumero inteiro (-9999 sai): i=");
	i=atoi(U0gets(buf,80));
	mprintf(U0putchar,"i(dec)=%4d i(hex)=%04x  i(octal)=%06o\r\n",i,i,i);
	} while(i!=-9999);
sprintf(buf,"sqrt(3)=%14.12lf", sqrt(3.0));
mprintf(U0putchar,"Escrito com sprintf: \"%s\"\n", buf);
do	{
	U0puts("\r\nRaiz quadrada de x=");
	x = atod(U0gets(buf, 80));
	y=sqrt(x);
	mprintf(U0putchar,"Formato G: sqrt(%6.4lg) = %12.10lg  x²=%8.2lg\r\n", x, y, x*x);
	mprintf(U0putchar,"Formato E: x=%12.6e\r\n",x);
	} while(1);
return 0;
}

/* Estas rotinas sao chamados pelo crt.S. 
Devem existir, mas ainda nao estao sendo usadas */
void UNDEF_Routine(){}
void SWI_Routine(){}
void FIQ_Routine(){}
