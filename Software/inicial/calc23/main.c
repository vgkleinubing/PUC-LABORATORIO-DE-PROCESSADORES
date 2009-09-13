/************************************************
Teste de ponto flutuante no lpc2378 com gcc
Calculadora RPN
Marcos Augusto Stemmer
*************************************************/
#include <arch/nxp/lpc23xx.h>
#include "uart.h"
/* Some useful constants.  */
# define M_E		2.7182818284590452354	/* e */
# define M_LOG2E	1.4426950408889634074	/* log_2 e */
# define M_LOG10E	0.43429448190325182765	/* log_10 e */
# define M_LN2		0.69314718055994530942	/* log_e 2 */
# define M_LN10		2.30258509299404568402	/* log_e 10 */
# define M_PI		3.14159265358979323846	/* pi */
# define M_PI_2		1.57079632679489661923	/* pi/2 */
# define M_PI_4		0.78539816339744830962	/* pi/4 */
# define M_1_PI		0.31830988618379067154	/* 1/pi */
# define M_2_PI		0.63661977236758134308	/* 2/pi */
# define M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
# define M_SQRT2	1.41421356237309504880	/* sqrt(2) */
# define M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */

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
k=((pc[3]<<4) + (pc[2] >> 4) - 1023)>>1;
k += 1023;
pc[3]=(k >> 4);
pc[2]=((k & 0xf) << 4);
k=5;
do	{
	r = (x/r + r)/2;
	} while(k--);
return r;
}

double seno(double x)
{
int qua,k;
double sen, termo, x2;
qua = x/M_PI_4;
x -= qua*M_PI_4;
if(qua & 2) x = M_PI - x;
if(qua & 4) x = -x;
k=0;
if(qua & 1) { sen=termo=1.0; x=M_PI_4 - x; }
else	{ sen=termo=x; k=1; }
x2=x*x;
do	{
	k++;
	termo = -termo*x2/((k+1) * k);
	k++;
	sen +=termo;
	} while(k<10);
return sen;
}

double coseno(double x)
{
return seno(M_PI_2-x);
}

double loga(double x)
{
double r,p, p2,m;
char *pc;
int k;
/* Se x<=0 retorna um valor errado, mas serve para teste */
if(x<=0) return x;
r=x;
pc=(char *)&r;
k=(pc[3]<<4) + (pc[2] >> 4) - 1023;
m= k*M_LN2;
pc[3]=0x3f;
pc[2]=(pc[2] & 0x0f) | 0xf0;
p = (r-1)/(r+1);
p2 = p*p;
p += p;
for(k=1; k < 18; k+=2){
	m+=(p/k);
	p *= p2;
	}
return m;
}

double exp(double x)
{
double r,p,t;
char *pc;
int k;
r=1.0;
pc=(char *)&r;
k=(x*M_LOG2E);
x -= k * M_LN2;
k+=1023;
pc[3]=(k >> 4);
pc[2]=(k & 0xf) << 4;
p=t=1.0;
for(k=1; k<15; k++){
	t=(t*x)/k;
	p+=t;
	}
return p*r;
}

int main (void)
{
double x, y, z, t, p;
int sobe=1;
int c;
char buf[80];
char formato[20];
x=y=z=t=0.0;
T0TCR = 0;
T0PR = 12000000/10000 - 1;
T0TCR = 2;
T0TCR = 1;
LCDinit();
U0init();
sprintf(formato,"%%10.4g    ");
do	{
	LCDcomando(0x80);
	mprintf(LCDputchar,formato,y);
	LCDcomando(0xc0);
	mprintf(LCDputchar,formato,x);
	c=leascii();
	if((c>='0' && c<='9') || c=='.'){
		LCDcomando(1);
		mprintf(LCDputchar,formato,x);
		U0putchar(c);
		LCDcomando(0xc0);
		LCDputchar(c);
		buf[0]=c;
		if(c=='.') { buf[0]='0'; buf[1]=c; }
		c=U0gets(buf,78,1+(c=='.'));
		if(sobe) { t=z;z=y;y=x;}
		U0puts(buf);
		x=atod(buf);
		}
	sobe=1;
	switch(c){
		case '\n': case '\r': case ' ':
			t=z; z=y; y=x; sobe=0; break;
		case '+': x=y+x; y=z; z=t; break;
		case '-': x=y-x; y=z; z=t; break;
		case '*': x=y*x; y=z; z=t; break;
		case '/': x=y/x; y=z; z=t; break;
		case 'x': p=x; x=y; y=p; break;
		case 'f': LCDcomando(1);
		LCDputs("FIX ");
		U0gets(buf,78,0);
		c=atoi(buf);
		if(c>13) c=14;
		sprintf(formato,"%%10.%dg     ",c);
		break;
		case '=': x=-x; break;
		case 'q': x=sqrt(x); break;
		case 's': x=seno(x); break;
		case 'p': t=z; z=y; y=x; x=M_PI; break;
		case 'c': x=coseno(x); break;
		case 'l': x=loga(x); break;
		case 'e': x=exp(x); break;
		}

	} while(1);
return 0;
}

/* Estas rotinas sao chamados pelo crt.S. 
Devem existir, mas ainda nao estao sendo usadas */
void UNDEF_Routine(){}
void SWI_Routine(){}
void FIQ_Routine(){}
