/*******************************************************
Modulo para usar o canal 2 de I2C
********************************************************/
#include <arch/nxp/lpc23xx.h>
#include "vic_cpsr.h"
#include "i2c2.h"
/* Definindo DEBUG escerve o estado da leitura do I2C */
#define DEBUG

#ifdef DEBUG
#include "lcd.h"
char fila[80];
int nfila;
#endif

/* Variaveis globais da Interrupcao i2c */
char *i2cdados;	/* Buffer de dados lidos ou escritos */
int slvaddr;		/* Endereco do dispositivo i2c	*/
volatile int i2cn;		/* Numero de bytes a ler/escrever */

void IRQ_i2c2(void) __attribute__ ((interrupt("IRQ")));
/*
A rotina de interrupcao do I2C implementa uma maquina de estados.
Dependendo do valor do I2cSTAT sabe o que foi feito e qual e' a
proxima coisa a fazer. */
void IRQ_i2c2(void)
{
static int k;
I2cCONSET = 4;		/* Normalmente responde ACK */
switch(I2cSTAT){
	case 0:	case 0x20:	/* Erro */
#ifdef DEBUG
fila[nfila++]='?';
#endif

	case 0x30:	/* Dado foi escrito recebendo NACK */
#ifdef DEBUG
fila[nfila++]='#';
#endif
	case 0x48:	/* Endereco de leitura recebeu NACK */
#ifdef DEBUG
fila[nfila++]='&';
#endif
		I2cCONSET = 0x10;	/* Gera STOP */
		i2cn=-1;		/* i2cn=-1 indica erro */
		break;
	case 8:		/* Terminou de enviar o START bit */
#ifdef DEBUG
fila[nfila++]='S';
#endif
		I2cCONCLR=0x20;
		I2cDAT = slvaddr;	/* Escreve endereco	*/
		k=0;	/* Inicializa indice dos dados */
		break;
	case 0x18:	/* Enviou endereco de escrita recebendo ACK */
#ifdef DEBUG
fila[nfila++]='A';
#endif
	case 0x28:	/* Escreveu dado recebendo ACK */
		/* Escreve mais dados */
		if(i2cn) {
#ifdef DEBUG
fila[nfila++]='W';
fila[nfila++]=i2cdados[k];
#endif
			 I2cDAT = i2cdados[k++]; i2cn--; 
			}
		else I2cCONSET=0x14;	/* Fim dos dados: gera STOP */
		break;
	case 0x40:	/* Enviou endereco de leitura e recebeu ACK */
	/* Prepara para iniciar a leitura de dados */
#ifdef DEBUG
fila[nfila++]='a';
#endif
		i2cdados[0]=I2cDAT; k=0; /* Le lixo so para iniciar */
		if(i2cn==1) I2cCONCLR=4; /* So 1 dado responde NACK */
		break;
	case 0x50:	/* Dado recebido enviando ACK */
#ifdef DEBUG
fila[nfila++]='r';
#endif
		i2cdados[k++]=I2cDAT;	/* Le o proximo dado */
		/* Se for o ultimo envia NACK */
		if(!(--i2cn)) I2cCONCLR=4;
		break;
	case 0x58:	/* Leu enviando NACK */
#ifdef DEBUG
fila[nfila++]='n';
#endif
		i2cdados[k++]=I2cDAT;	/* Le o ultimo dado */
		i2cn=0;		/* Leu todos os dados */
		I2cCONSET=0x10;	/* Gera STOP */
		break;
#ifdef DEBUG
	default:
fila[nfila++]='@';
#endif
	}
#ifdef DEBUG
if(nfila>70) nfila=0;
#endif

I2cCONCLR = 8;	/* Limpa o indicador de interrupcao do I2C */
VICVectAddr = 0;	/* Limpa o vetor do VIC */
}

/* Espera ate terminar uma transacao i2c */
void espera_i2c(int t)
{
int k;
t += T0TC;
while((t!=T0TC) && (i2cn > 0));	/* Espera ate que i2cn==0 */

t=T0TC+40;
while(t != T0TC);
#ifdef DEBUG
fila[nfila++]=(i2cn? '}':']');
U0putchar('[');
for(k=0; k<nfila;k++) U0putchar(fila[k]);
nfila=0;
#endif
}

/* Inicializa o canal 2 de I2C com interrupcao */
void ini_i2c2(void)
{
T0TCR = 0;              /* Desabilita T0 */
T0PR = CRYSTALFREQ/1000 - 1; /* Incrementa o contador T0TC 1000 vezes por segundo */
T0TCR = 2;      /* Inicializa T0 */
T0TCR = 1;      /* Habilita T0  */
#ifdef DEBUG
nfila=0;
#endif

PCONP |= 0x04000080;	/* Liga energia do i2c0 e i2c2 */
I2cCONCLR = 0xff;
I2cCONSET = 0x40;	/* Habilita o i2c2 um modo mestre */
I2cSCLH   = 100;	/* Tempo alto do SCL	*/
I2cSCLL   = 100;	/* Tempo baixo do SCL	*/

/* Habilita a interrupcao do i2c2 como IRQ no VIC	*/
desabilitaIRQ();	/* Definida no #include "vic_cpsr.h"	*/
#ifdef USA_I2C2
PINSEL0 &= 0xffafffff;
PINSEL0 |= 0x00a00000;
VICIntSelect &= ~0x40000000;	/* i2c2=bit 30 como IRQ	*/
VICIntEnable |= 0x40000000;	/* Habilita int do i2c2 no VIC*/
VICVectAddr30 = (int)IRQ_i2c2;	/* Vetor para atendimento do I2C2 */
U0puts("I2C2\n");
#endif
#ifdef USA_I2C0
PINSEL1 &= 0xfd7fffff;
PINSEL1 |= 0x01400000;
VICIntSelect &= ~0x200;	/* i2c0=bit 9 como IRQ	*/
VICIntEnable |= 0x200;	/* Habilita int do i2c0 no VIC*/
VICVectAddr9 = (int)IRQ_i2c2;	/* Vetor 9 para atendimento do I2C0 */
U0puts("I2C0\n");
#endif
i2cn=0;
habilitaIRQ();	/* Definida no #include "vic_cpsr.h"	*/
I2cCONSET=0x10;
}

/* Escreve um byte na interface i2c2. Retorna 0 se conseguiu escrever. */
int escreve_byte_i2c2(int i2caddr, int val)
{
slvaddr=i2caddr;
i2cn = 1;
i2cdados = (char *)&val;
I2cCONCLR = 0xbf;
I2cCONSET=0x20;
espera_i2c(800);
return i2cn;
}

/* Retorna um byte lido da interface i2c2. Retorna -1 em caso de erro */
int le_byte_i2c2(int i2caddr)
{
char resposta[4];
slvaddr=i2caddr | 1;
i2cn = 1;
i2cdados = resposta;
I2cCONCLR = 0xbf;
I2cCONSET=0x20;
espera_i2c(800);
if(i2cn) return -1;
return (*((int *)resposta) & 0xff);
}

/* Le dados de um dispositivo i2c endercavel.
buf	Local onde deve armazenar os dados lidos
n	Numero de bytes que devem ser lidos
Retorna ZERO se conseguiu ler */
int le_i2c2(int i2caddr, char *buf, int n)
{
i2cdados = buf;
slvaddr = i2caddr | 1;	/* Endereco de leitura	*/
i2cn = n;		/* Vai ler n bytes	*/
I2cCONCLR = 0xbf;	/* Limpa comando */
I2cCONSET = 0x20;	/* Inicia a leitura	*/
espera_i2c(800);	/* Espera terminar de ler */
return i2cn;
}

/* Escreve dados em um dispositovo i2c enderecavel
addri2c	Endereco i2c do dispositivo
buf	Local onde os dados estao armazenados
n	Numero de bytes que devem ser escritos
Retorna ZERO em caso de sucesso */
int escreve_i2c2(int addri2c, char *buf, int n)
{
i2cdados=buf;
slvaddr = addri2c & 0xfe;
i2cn = n;
I2cCONCLR = 0xbf;
I2cCONSET=0x20;
espera_i2c(2000);
return i2cn;
}
