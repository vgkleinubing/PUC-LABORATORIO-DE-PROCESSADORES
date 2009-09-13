/*******************************************************
Programa que implementa um relogio e usa o canal 2 de I2C para ler 
a temperatura  em um termometro tipo TMP100 usando interrupcao.
Tutorial LPC2378-gcc parte 8
Marcos A. Stemmer
********************************************************/

#include <arch/nxp/lpc23xx.h>
#include "lcd.h"

void ini_i2c(void);
int initmp100(void);
/* Variaveis globais da interrupcao de I2C */
extern char i2cdados[];	/* Buffer de dados */
extern int slvaddr;	/* Endereco i2c do dispositivo */
extern int i2cn;	/* Numero de bytes	*/

/* Se CLOCKRTC for definido usa-se oscilsdor espencial de 32768Hz */
/* Se nao for definido usa o clock de 12MHz -*/
//#define CLOCKRTC

void acerta(void)
{
CCR=0;
CCR=2;
if(HOUR>23) HOUR=12;
if(MIN>59) MIN=0;
if(SEC>59) SEC=0;
#ifdef CLOCKRTC
PREINT = 0;	/* Usa o clock de 32768Hz do RTC */
PREFRAC = 0;
UART0putchar('%');
CCR=0x11;
#else
PREINT =  (CRYSTALFREQ >> 17)-1;	/* Usa CPUCLK de 12MHz */
PREFRAC = ((CRYSTALFREQ >> 2) & 0x7fff);
UART0putchar('&');
CCR=1;
#endif
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

/* Escreve um numero como 8 digitos hexadecimais */
void prword(unsigned int w)
{
PrByte(w >> 24); PrByte(w >> 16);
PrByte(w >> 8); PrByte(w);
}

void escreve2dec(int val)
{
LCDputchar((val / 10)  + '0');
LCDputchar((val % 10) + '0');
}

/* Escreve a temperatura como um numero decimal */
void escrevenum(int num,int f)
{
char digs[16];
int nd=0;
num *=625;
if(num<0) { LCDputchar('-'); num=-num; }
do	{
	digs[nd++]= (num % 10) + '0';
	num /= 10;
	} while(num);
while(nd < f) digs[nd++]='0';
while(nd >= f) LCDputchar(digs[--nd]);
LCDputchar(','); /* Parte fracionaria com resolucao de 1/16 */
LCDputchar(digs[--nd]);
LCDputchar(digs[--nd]);
UART0putchar('\n');
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
void ad0config(void)
{
AD0CR = 1;	/* ADC0 desligado */
PCONP |= 0x00001000;	/* Liga energia do ADC	*/
PINSEL1 |= 0x4000;	/* Seleciona pino do ADC0 */
AD0CR = 0x00200201;	/* Configura ADC0	*/
}

int le_adc0(void)
{
int k;
k=40000;
AD0CR |= 0x01000000;	/* Inicia a conversao (Liga bit 24:START)*/
while(!( AD0GDR & 0x80000000) & k) k--;	/* Espera terminar a conversao */
k = ((AD0DR0 >> 6) & 0x3ff);
return k;
}

int main (void)
{
unsigned int k;
short h, m;
UART0_Init();	/* Configura a porta serial UART0 */
LCDinit();
ad0config();
LCDputs("PUCRS\n");
UART0puts("\r\nLab. Processdores PUCRS 2008/2 - ");
#ifdef RAM_RUN		/* Dedinir externamente RAM_RUN para executar na RAM */
UART0puts("RAM\r\n");
MEMMAP = 2;	/* 2 Mapeia para RAM */
#else
UART0puts("FLASH\r\n");
MEMMAP = 1;	/* 1 Mapeia vetores para a FLASH */
#endif

ini_i2c();
acerta();
k=initmp100();
if(k) initmp100();
do	{
/* Faz piscar os LEDs 1 e 2 sincronizados com o timer T0 */
	do {
		k = T0TC % 500;
		switch(k){
		case 400:
			slvaddr = ADDRTMP;	/* TMP100 para escrita */
			i2cn=1;		/* vai escrever 1 byte */
			i2cdados[0]=0;	/* Escreve 0: numero do reg */
			I22CONCLR = 0xbf;
			I22CONSET = 0x20;	/* Inicia ligando o START */
			break;
		case 440:
			slvaddr = ADDRTMP+1;	/* TMP100 para leitura */
			i2cn=2;		/* Vai ler 2 bytes	*/
			I22CONCLR = 0xbf;
			I22CONSET = 0x20;	/* START */
			break;
			}
		}while(k);
/* Acerta o relogio usando os botoes */
	k=((~FIO4PIN) & (SW1 | SW2| SW3 | SW4));
	if(k)	{
		 h=HOUR; m=MIN;
		switch(k){
			case SW1: h++; if(h>23) h=0; break;	/* Adianta hora */
			case SW2: h--; if(h<0) h=23; break;	/* Atrasa hora */
			case SW3: m++; if(m>59) m=0; break;	/* Adianta minuto */
			case SW4: m--; if(m<0) m=59; break;	/* Atrasa minuto */
			}
		HOUR=h; MIN=m;
		}
	LCDcomando(0x86);
	LCDputs("ADC0=");
	escrevenum((le_adc0()*33)>>6,6);
	LCDputchar('V');
	LCDcomando(0xc0);
	UART0putchar('\r');
	escreve2dec(HOUR);
	LCDputchar(':');
	escreve2dec(MIN);
	LCDputchar(':');
	escreve2dec(SEC);
	LCDputs(" T=");
/* Mostra a temperatura: Lida em 2 bytes do array global i2cdados */
	escrevenum(((((int)i2cdados[0] << 8) | (i2cdados[1] & 0xff)) >> 4),5);
	LCDputchar(' ');
	} while(1);
return 0;
}

/* Estas rotinas sao chamados pelo crt.s. 
Devem existir, mas ainda nao estao sendo usadas */
void UNDEF_Routine(){}
void SWI_Routine(){}
void FIQ_Routine(){}
