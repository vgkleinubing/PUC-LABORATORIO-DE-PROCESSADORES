/*
Rotinas de suporte de e/s
Programador: Marcos Stemmer
*/
#include "lcd.h"


int dig2ascii(int c)
{
c = '0' + (c & 0xf);
if(c > '9') c+='A'-'9'-1;
return (c);
}

void PrByte(int c)
{
U0putchar(dig2ascii(c >> 4));
U0putchar(dig2ascii(c));
}

void LCDprbyte(int c)
{
LCDputchar(dig2ascii(c >> 4));
LCDputchar(dig2ascii(c));
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

/* Entrada de texto */
void letexto(char *txt, int nmax)
{
int k;	/* Numero de caracteres armazenados */
int c;	/* caractere lido */
k=0;
do	{
	c=U0getchar();
	if(c==0x7f) c=8;
	U0putchar(c);
	/* se for backspace retira um caractere do buffer */
	if(c==8 && (k!=0)) k--;
	else if(k<nmax) txt[k++]=c;
	} while(c!='\n' && c!='\r');
txt[k-1]='\0';
}

/* Pega um numero; Usa-se assim:
char buf[20];	// Para armazenar o texto lido
int i;		// Vai ler o numero i
letexto(buf,20);	// Le como texto
i = lenum(buf);	// Converte para numero
*/
int lenumero(char *str)
{
int x, d, s;
x=s=0;
while(*str == ' ') str++;
if(*str == '-') { s=1; str++; }
do	{
	d = *str++;
	if(d < '0' || d > '9') break;
	x = x*10 + d - '0';
	} while(1);
return s? -x: x;
}
