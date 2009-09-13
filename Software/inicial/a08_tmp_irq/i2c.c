/*******************************************************
Modulo para usar o canal 2 de I2C para ler a temperatura 
em um termometro tipo TMP100 usando interrupcao.
Tutorial LPC2378-gcc parte 8
Marcos A. Stemmer
********************************************************/
#include <arch/nxp/lpc23xx.h>
#include "lcd.h"
#include "vic_cpsr.h"
/* Definindo DEBUG escerve o estado da leitura do I2C */
#define DEBUG

#ifdef DEBUG
char fila[80];
int nfila;
#endif
/* Variaveis globais da Interrupcao i2c */
char i2cdados[32];	/* Buffer de dados lidos ou escritos */
int slvaddr;		/* Endereco do dispositivo i2c	*/
int i2cn;		/* Numero de bytes a ler/escrever */

void IRQ_i2c2(void) __attribute__ ((interrupt("IRQ")));
/*
A rotina de interrupcao do I2C implementa uma maquina de estados.
Dependendo do valor do I22STAT sabe o que foi feito e qual e' a
proxima coisa a fazer. */
void IRQ_i2c2(void)
{
static int k;
I22CONSET = 4;		/* Normalmente responde ACK */
switch(I22STAT){
	case 0:	case 0x20:	/* Erro */
#ifdef DEBUG
fila[nfila++]='&';
#endif
		I22CONSET = 0x10;	/* Gera STOP */
		i2cn=-1;		/* i2cn=-1 indica erro */
		break;
	case 8:		/* Terminou de enviar o START bit */
		I22CONCLR=0x20;

#ifdef DEBUG
fila[nfila++]='S';
#endif
		I22DAT = slvaddr;	/* Escreve endereco	*/
		I22CONSET=4;		/* Com ACK	*/
		k=0;
		break;
	case 0x18:	/* Enviou endereco de escrita recebendo ACK */
#ifdef DEBUG
fila[nfila++]='A';
#endif
	case 0x28:	/* Escreveu dado recebendo ACK */
#ifdef DEBUG
fila[nfila++]='W';
#endif
		/* Escreve mais dados */
		if(i2cn) { I22DAT = i2cdados[k++]; i2cn--; }
		else I22CONSET=0x14;	/* Fim dos dados: gera STOP */
		break;
	case 0x58:	/* Leu enviando NACK */
#ifdef DEBUG
fila[nfila++]=']';
#endif
		i2cdados[k++]=I22DAT;	/* Le o ultimo dado */
		i2cn=0;
		I22CONSET=0x10;		/* Gera STOP */
	case 0x30:	/* Dado foi escrito recebendo NACK */
#ifdef DEBUG
fila[nfila++]='n';
#endif
	case 0x48:	/* Endereco de leitura recebeu NACK */
#ifdef DEBUG
fila[nfila++]='}';
#endif
		I22CONSET=0x10;	/* Gera STOP */
		break;
	case 0x40:	/* Enviou endereco de leitura e recebeu ACK */
#ifdef DEBUG
fila[nfila++]='a';
#endif
	/* Prepara para iniciar a leitura de dados */
		i2cdados[0]=I22DAT; k=0; /* Le lixo so para iniciar */
		if(i2cn==1) I22CONCLR=4; /* So 1 dado responde NACK */
		break;
	case 0x50:	/* Dado recebido enviando ACK */
#ifdef DEBUG
fila[nfila++]='R';
#endif
		i2cdados[k++]=I22DAT;	/* Le o proximo dado */
		if(!i2cn) I22CONSET=0x10;
		/* Se for o ultimo envia NACK */
		else if(!(--i2cn)) I22CONCLR=4;
		break;
	}
#ifdef DEBUG
if(nfila > 78) nfila=78;
#endif
I22CONCLR = 8;	/* Limpa o indicador de interrupcao do I2C */
VICVectAddr = 0;	/* Limpa o vetor do VIC */
}

/* Inicializa o canal 2 de I2C com interrupcao */
void ini_i2c(void)
{
PCONP |= 0x04000080;	/* Liga energia do I2C0 e I2C2 */
PINSEL0 &= 0xffafffff;
PINSEL0 |= 0x00a00000;	/* Seleciona pinos do SDA2 e SCL2 */
I22CONCLR = 0xff;
I22CONSET = 0x40;	/* Habilita o I2C-2 um modo mestre */
I22SCLH   = 100;	/* Tempo alto do SCL	*/
I22SCLL   = 100;	/* Tempo baixo do SCL	*/

/* Habilita a interrupcao do i2c2 como IRQ no VIC	*/
desabilitaIRQ();	/* Definida no #include "vic_cpsr.h"	*/
VICIntSelect &= ~0x40000000;	/* i2c2=bit 30 como IRQ	*/
VICIntEnable |= 0x40000000;	/* Habilita int do i2c2 no VIC*/
VICVectAddr30 = (int)IRQ_i2c2;	/* Vetor para atendimento do I2C2 */
i2cn=0;
habilitaIRQ();	/* Definida no #include "vic_cpsr.h"	*/
I22CONSET=0x10;
}

/* Escreve o byte de configuracao do TMP100 */
int initmp100(void)
{
int i;
#ifdef DEBUG
nfila=0;
#endif
slvaddr = ADDRTMP;		/* Endereco do TMP100 para escrita */
i2cn=2;			/* Vai escrever 2 bytes */
i2cdados[0]=1;		/* Registrador 1 do TMP100 */
i2cdados[1]=0x60;	/* Valor 0x60 configura para resolucao de 1/16 grau */
I22CONCLR = 0xbf;	/* Limpa comando */
I22CONSET = 0x20;	/* Gera START para iniciar a transmissao */
espera(8);		/* Da um tempo para a interrupcao transmitir */
#ifdef DEBUG
UART0putchar(i2cn + '0');
for(i=0; i<nfila; i++) UART0putchar(fila[i]);
nfila=0;
UART0putchar('\n');
#endif
return i2cn;
}
