/************************************************
Monitor LPC2378
*************************************************/
#include <arch/nxp/lpc23xx.h>
#include "monarm.h"
unsigned regs[5];

/* Escreve String */
void UART0puts(char *s)
{
while(*s) UART0putchar(*s++);
}

/* Le uma linha de texto. n=numero maximo de letras */
int GetStr(char *s, int n)
{
int i;
char c;
i=0;
do	{
	c=UART0getchar();
	if(c==0x7f) c=8;
	if(c==8){
		if(i)	{
			UART0putchar(c); i--;
			UART0putchar(' '); UART0putchar(8);
			}
		}
	else	{
		s[i++]=c; 
		UART0putchar(c);
		}
	if(c=='\x1b'){
		*s='\0';
		return 1;
		}
	} while(c!=10 && c!=13 && i < n);
if(c==13) UART0putchar('\n');
s[i]=0;
return 0;
}

void PrDig(char c)
{
c = '0' + (c & 0xf);
if(c > '9') c+='A'-'9'-1;
UART0putchar(c);
}

void PrByte(char c)
{
PrDig(c >> 4); PrDig(c);
}

/* Pega um numero na base radix */
unsigned long getnum(char *str, int radix)
{
int x;
int d;
x=0;
do	{
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

/* Le um digito hexadecimal */
int ledig()
{
int d;
d=UART0getchar();
if(d>'9') {
	d &= 0xdf;
	d -= ('A'-'9' -1);
	}
d-='0';
if(d<0 || d>15) d=-8000;
return d;
}

int lebyte()
{
return (ledig() << 4)+ledig();
}
/*
Le dados em formato hexadecimal da Intel
Envia um caractere de resposta no final de cada linha
'.' (Ponto) significa linha OK
'E' Erro de ckecksum
'*' Erro de sintaxe (não entendi nada) 
	O transmissor deve parar de transmitir quando recebe '*'
0x40000000	RAM Interna
0x80000000	Flash externa
0x81000000	RAM Externa
*/
int lehex(unsigned long base)
{
unsigned char buf[32];
unsigned int addr;
unsigned char *pc;
int chksum, k, n, c;
int flag;
do	{
	n=lebyte();
	chksum=n;
	if(n<0) { UART0putchar('*'); continue; }
	c=lebyte();
	if(c<0) { UART0putchar('*'); continue; }
	chksum+=c;
	addr = (c << 8);
	c=lebyte();
	chksum+=c;
	addr |= c;
	flag=lebyte();
	if(flag==4) {
		c=lebyte();
		chksum+=c;
		base= c << 24;
		c=lebyte();
		chksum+=c;
		base |= c << 16;
		n-=2;
		}
	pc = (unsigned char *)(base+addr);
	if(n > 32 || n < 0) { UART0putchar('*'); break; }
	for(k=0; k<n; k++) {
		c = lebyte();
		chksum+=c;
		if(!flag) buf[k] = c;
		}
	chksum+=lebyte()+flag;
	if(chksum < 0) { UART0putchar('*'); break; }
	for(k=0; k<n; k++) pc[k]=buf[k];
	UART0putchar(chksum & 0xff? 'E':'.');
	if(flag==1) break;
	do n=UART0getchar(); while(n=='\n' || n=='\r' || n==' ' || n=='\t');
	} while(n==':');
return chksum;
}

unsigned long getaddress()
{
char buf[32];
UART0puts(" Address=0x");
GetStr(buf,32);
if(*buf < ' ') return 0xbfffffff;
return getnum(buf,16);
}

/* Escreve um numero como 8 digitos hexadecimais */
void prword(unsigned int w)
{
PrByte(w >> 24); PrByte(w >> 16);
PrByte(w >> 8); PrByte(w);
UART0putchar(':');
}

/* Escreve os bytes da memoria em hexadecimal */
void dump()
{
union address ad;
int i, c;
UART0puts("Dump:");
ad.w=getaddress();
if(ad.w==0xbfffffff) return;
do	{
	prword(ad.w);
	for(i=0; i<16; i++) {
		UART0putchar(' ');
		PrByte(ad.p[i]);
		}
	UART0puts("   ");
	for(i=0; i<16; i++) {
		c=ad.p[i];
		UART0putchar(c < ' ' || (c > 0x7f && c<0xa0)? '.':c);
		}
	ad.w += 16;
	UART0puts("\r\n");
	} while(UART0getchar()!=0x1b);
}

/* Escreve bytes na memoria */
void escreve_mem()
{
union address ad;
char buf[32];
UART0puts("Memory Set:");
ad.w=getaddress();
if(ad.w==0xbfffffff) return;
do	{
	prword(ad.w);
	PrByte(*(ad.p)); UART0putchar(' ');
	if(GetStr(buf,32)) break;
	if(!(*buf)) return;
	if(*buf >= '0') *ad.p = getnum(buf,16);
	UART0puts("\x1b[A\x1b[16C");
	PrByte(*(ad.p));
	UART0puts("\r\n");
	ad.w++;
	} while(1);
}

/* Escreve palavras de 32 bits na memoria */
void escreve_word()
{
union address ad;
char buf[32];
UART0puts("Word Set:");
ad.w=getaddress();
if(ad.w==0xbfffffff) return;
ad.w &= -4;
do	{
	prword(ad.w); UART0putchar(' ');
	prword(*(ad.pw)); UART0putchar(' ');
	if(GetStr(buf,32)) break;
	if(!(*buf)) return;
	if(*buf >= '0') *ad.pw = getnum(buf,16);
	ad.pw++;
	} while(1);
}

/* Copia bytes da memoria */
void copy()
{
union address af, ad;
int n;
char buf[32];
UART0puts("Copia de");
af.w=getaddress();
if(ad.w==0xbfffffff) return;
UART0puts("para");
ad.w=getaddress();
if(ad.w==0xbfffffff) return;
UART0puts("Numero de bytes (hex)=");
if(GetStr(buf,16)) return;
n=getnum(buf,16);
while(n--) { *ad.p++ = *af.p++; };
}

/* Ececuta um programa no endereco especificado */
void execute()
{
union address ad;
UART0puts("Execute:");
ad.w=getaddress();
if(ad.w==0xbfffffff) return;
(ad.f)();
}

void acerta_relogio()
{
char buf[20];
int s, t;
CCR=0; CCR=2; CCR=0;

//PREINT =  (CRYSTALFREQ >> 17)-1;
//PREFRAC = ((CRYSTALFREQ >> 2) & 0x7fff);
//PREINT=2; PREFRAC=0;
UART0puts("Hora=");
GetStr(buf,20);
if(*buf < ' ') { CCR=1; return; }
HOUR=getnum(buf,10);
UART0puts("Minuto=");
if(GetStr(buf,20)) return;
MIN=getnum(buf,10);
UART0puts("Segundo=");
if(GetStr(buf,20)) return;
SEC=getnum(buf,10);
CCR=0x11;
s= SEC;
t=T0TC+48;
while(T0TC!=t);
s = (SEC-s);
if(s<0) s+=60;
if(!(s==1 || s==2)) {
	PREINT =  (CRYSTALFREQ >> 17)-1;
	PREFRAC = ((CRYSTALFREQ >> 2) & 0x7fff);
	PREINT=2; PREFRAC=0;
	CCR=1;
	}
}

/* Escreve 2 digitos decimais */
void pr2dec(int x)
{
LCDputchar((x / 10) + '0');
LCDputchar((x % 10) + '0');
}

/* Escreve um numero decimal */
void escreve_num(int num)
{
char digs[16];
int nd=0;
if(num<0) { LCDputchar('-'); num=-num; }
do	{
	digs[nd++]= (num % 10) + '0';
	num /= 10;
	} while(num);
while(nd--) LCDputchar(digs[nd]);
}

/* Configura o conversor AD:
Bits do AD0CR:
31-28	Reservado:(sempre 0)
27	Borda	Valor usado: 0=subida)
26-24	START: Inicio da conversao: (valor usado: 0)
	0	Esperando
	1	Inicia agora
21	1=AD funcionando; 0=Desligado
19-17	Numero de bits: 0=10 bits
16	0 Iniciar conversao por comando explicito; 1 Inicia repetidamente
15-8	Divisor de clock. Divide PCLK por este numero+1. Valor usado:2 (divide por 3)
	A freq resultante deve ser menor ou igual a 4,5MHz
7-0	Seleciona os conversores ativos. 1 Seleciona o ADC0
*/
void ad0config()
{
AD0CR = 1;	/* ADC0 desligado */
PCONP |= 0x00001000;	/* Liga energia do ADC	*/
PINSEL1 |= 0x4000;	/* Seleciona pino do ADC0 */
AD0CR = 0x00200201;	/* Configura ADC0	*/
}

int le_adc0()
{
int k;
k=40000;
AD0CR |= 0x01000000;	/* Inicia a conversao (Liga bit 24:START)*/
while(!( AD0GDR & 0x80000000) & k) k--;	/* Espera terminar a conversao */
k = ((AD0DR0 >> 6) & 0x3ff);
return k;
}
