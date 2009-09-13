#include <arch/nxp/lpc23xx.h>
#include "uart.h"
#define PS2_CP 0x40000000
#define PS2_D 0x80000000

void espera(int t)
{
t += T0TC;
while(t != T0TC);
}

int lescan(void)
{
int k, scan;
do	{
	while(!(FIO4PIN3 & 0x40));	/* Espera enquanto KB_CP==0 */
	while(FIO4PIN3 & 0x40)		/* Espera enquanto KB_CP==1 */
		if(U0LSR & 1) return -1;
	} while(FIO4PIN3 & 0x80);	/* O start bit deve ser 0 */
scan=0; k=8;
do	{	/* 8 bits de dados */
	while(!(FIO4PIN3 & 0x40));	/* Espera subir CP */
	while(FIO4PIN3 & 0x40);		/* Espera descer CP */
	scan = (scan >> 1)+(FIO4PIN3 & 0x80);
	}while(--k);
while(!(FIO4PIN3 & 0x40));	/* Le o bit de paridade */
while(FIO4PIN3 & 0x40);
while(!(FIO4PIN3 & 0x40));	/* Le o stop bit  */
while(FIO4PIN3 & 0x40);
return scan;
}

void escreve_bit(int b)
{
while(!(FIO4PIN3 & 0x40));	/* Espera subir CP */
while(FIO4PIN3 & 0x40);		/* Espera descer CP */
FIO4DIR &= ~(PS2_CP | PS2_D);
if(!(b & 1)) {
	FIO4DIR |= PS2_D;
	FIO4CLR3 = 0x80;
	}
}

void escreve_ps2(int c)
{
int par, k;
while(!(FIO4PIN3 & 0x40));	/* Le o stop bit  */
FIO4DIR |= PS2_CP;	/* CP como saida */
FIO4CLR = PS2_CP;	/* CP=0 */
espera(2);
FIO4DIR |= PS2_D;
FIO4CLR = PS2_D;	/* DATA=0 */
FIO4DIR &= ~PS2_CP;
k=8; par=1;
do	{
	escreve_bit(c);
	par ^= c;
	c >>= 1;
	} while(--k);
escreve_bit(par);
escreve_bit(1);
while((FIO4PIN3 & 0xc0)!=0xc0);	/* Espera subir CP */
}

/**** Le o teclado traduzindo de scancode para ASCII   ***/
/**** e interpretando as teclas de prefixo shift e ctrl***/
int leascii()
{
static int shf;
int sc, asc;
static unsigned char scan[]={
'\x0e','\x16','\x1e','\x26','\x25','\x2e','\x36','\x3d','\x3e','\x46',
'\x45','\x4e','\x55','\x61','\x66','\x0d',
'\x15','\x1d','\x24','\x2d','\x2c','\x35','\x3c','\x43','\x44',
'\x4d','\x5b','\x5d',
'\x1c','\x1b','\x23','\x2b','\x34','\x33','\x3b','\x42','\x4b',
'\x4c','\x52',
'\x1a','\x22','\x21','\x2a','\x32','\x31','\x3a','\x41','\x49',
'\x51','\x29','\x5a',
'\x70','\x71','\x69','\x72','\x7a','\x6b','\x73','\x74',
'\x6c','\x75','\x7d','\xda','\xca','\x7c','\x7b','\x79',
'\xeb','\x76'};
static char ascii[]=
"'1234567890-=\\\b\tqwertyuiop[]asdfghjkl;'zxcvbnm,./ \n"
"0.123456789\n/*-+\b\x1b";
static char ascis[]=
"~!@#$%^&*()_+|\b\tQWERTYUIOP{}ASDFGHJKL:\"ZXCVBNM<>? \n"
"0.123456789\n/*-+\x1b";
#define CTRL_SCAN	'\x14'
#define SHF1_SCAN	'\x12'
#define SHF2_SCAN	'\x59'
#define CAPS_SCAN	'\x58'

do	{
	sc = lescan();	/* Le o make code */
	if(U0LSR & 1) return U0RBR;
	if(sc==0xe0)  sc=lescan() | 0x80;
	while(sc==CAPS_SCAN) sc=lescan();
	while(sc==SHF1_SCAN || sc==SHF2_SCAN) { sc=lescan(); shf|=1; }
	while(sc==CTRL_SCAN) { sc=lescan(); shf|=2; }
	if(sc !=0xf0) break;
	sc=lescan();	/* Le o break code (soltou a tecla) */
	switch(sc) {
	case SHF1_SCAN: case SHF2_SCAN:
		shf &= 0xfe; break;		/* Desliga shift */
	case CTRL_SCAN: shf &= 0xfd; break;	/* Desliga Ctrl */
	case CAPS_SCAN: shf ^= 0x24; 
		escreve_ps2(0xed); 
		escreve_ps2(shf & 4);
		lescan();
		break;
		}
  	} while(1);
for(asc=0; scan[asc]!='\x76' && scan[asc]!=sc; asc++);
asc=shf & 1 ? ascis[asc]:ascii[asc];
if((asc >= 'A' && asc <= 'Z') || (asc >= 'a' && asc <= 'z')) {
	asc ^= shf & 0x20;
	if(shf & 2) asc &= '\x1f';
	}
return asc;
}
