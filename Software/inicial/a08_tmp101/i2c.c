/************************************
Modulo para usar o canal 2 de I2C para ler a 
temperatura em um termometro tipo TMP101 sem interrupcao
Tutorial LPC2378-gcc parte 8
Marcos A. Stemmer
*************************************/
#include <arch/nxp/lpc23xx.h>
#include "lcd.h"
void escreve_i2c(int dado, int val);

/* Inicializa o canal 2 de I2C */
void ini_i2c(void)
{
PCONP |=0x04000080;	/* Liga energia do I2C0 e I2C2 */
PINSEL0 &= 0xffafffff;
PINSEL0 |= 0x00a00000;	/* Seleciona pinos do SDA2 e SCL2 */
I22CONCLR = 0xff;
I22CONSET = 0x40;	/* Habilita o I2C-2 um modo mestre */
I22SCLH   = 100;	/* Tempo alto do SCL	*/
I22SCLL   = 100;	/* Tempo baixo do SCL	*/
escreve_i2c(1,0x60);
}

/* Escreve a temperatura como um numero decimal */
void escrevenum(int num)
{
char digs[16];
int nd=0;
num *=625;
if(num<0) { LCDputchar('-'); num=-num; }
do	{
	digs[nd++]= (num % 10) + '0';
	num /= 10;
	} while(num);
while(nd < 5) digs[nd++]='0';
while(nd > 4) LCDputchar(digs[--nd]);
LCDputchar(','); /* Parte fracionaria com resolucao de 1/16 grau */
LCDputchar(digs[--nd]);
LCDputchar(digs[--nd]);
UART0putchar('\n');
}

/* Escreve 2 bytes na interface i2c. Se val<0 escreve so o dado. */
void escreve_i2c(int dado, int val)
{
int k, i;
/* Inicia a transacao: Envia o endereco 0x96 para escrita no TMP100 */
I22CONCLR = 0xFF;
I22CONSET = 0x60;
k=1; i=1000;
do	{
	switch(I22STAT){
		case 0x08: /* Terminou START; envia endereco */
			I22CONCLR=0x20;
		case 0x20: /* NACK do endreco: reenvia */
			I22DAT = ADDRTMP; I22CONCLR=8; i=1000; break;
		case 0x18: /* ACK do endereco: envia dado */
			I22DAT = dado; I22CONCLR=8; i=1000; break;
		case 0x28: /* ACK do dado: envia STOP */
			if(val<0) I22CONSET = 0x10;
			else { I22DAT=val; val-=0x100; }
			I22CONCLR=8; k=0; i=1000; break;
		}
	} while(k && --i);
i=8000;
while(!(I22STAT & 8) && --i);
}

/*No I2C numero 0x96 tem um termometro tipo TMP100 da TI */
short letemp()
{
short Temp;
int k, i;
escreve_i2c(0,-1);
/* Fase 2: Le a temperatura (2 bytes) */
I22CONCLR = 0xff;
I22CONSET = 0x60;
k=2; Temp=0; i=1000;
do	{
	switch(I22STAT){	/* Terminou START; envia endereco */
		case 0x08: I22CONCLR=0x20;
		case 0x20: /* NACK do endreco: reenvia */
			I22DAT = ADDRTMP+1; /* Endereco i2c */
			I22CONCLR=8; i=1000; break;
		case 0x40:
			I22CONSET=4; I22CONCLR=8; i=1000; break;
/* Recebeu dado: Le dado */
		case 0x50: Temp = (Temp << 8) | I22DAT;
/* Ultimo dado: Envia STOP */
			if(!(--k)) I22CONSET=0x10;
			I22CONCLR=8; i=1000; break;
		}
	} while(k && --i);
return Temp;
}
