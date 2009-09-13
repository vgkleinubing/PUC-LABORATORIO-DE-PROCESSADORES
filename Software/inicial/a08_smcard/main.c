/************************************************
Teste do SmartCard
modulo main.c
Marcos Augusto Stemmer
*************************************************/
#include <arch/nxp/lpc23xx.h>
#include "lcd.h"
#include "i2c2.h"

int main (void)
{
char buf[24];
int c, offset;
int x;
buf[16]='\0';
LCDinit();
U0init();
U0puts("Le/escreve no smartcard - ");
#ifdef RAM_RUN		/* Dedinir externamente RAM_RUN para executar na RAM */
U0puts("RAM\r\n");
MEMMAP = 2;	/* 2 Mapeia para RAM */
#else
U0puts("FLASH\r\n");
MEMMAP = 1;	/* 1 Mapeia vetores para a FLASH */
#endif
U0puts("+ Le 16 bytes adiante\r\n");
U0puts("- Le 16 bytes anteriores\r\n");
U0puts("n Escreve um nome no smcard\r\n");
LCDputs("Le SmartCard");
ini_i2c2();	/* Inicializa a interface i2c do smcard */
offset=0;
while(1){
	c=U0getchar();
	switch(c){
		case '+': offset += 16; break;
		case '-': offset -= 16; break;
		case 'n': case 'N':
			U0puts("\r\nNome=");
			letexto(buf+1,16);
			U0puts("\r\nNome=");
			U0puts(buf+1);
			buf[0]=offset;
			c=escreve_i2c2(0xa0,buf,9);
			buf[8]=offset+8;
			c|=escreve_i2c2(0xa0,buf+8,9);
			U0puts(c? "Erro!\r\n":"OK\r\n");
			break;
		}
	LCDcomando(1);
	escreve_byte_i2c2(0xa0,offset);
	c=le_i2c2(0xa0, buf, 16);
	buf[16]='\0';
	if(c)	{
		LCDputs("**Erro**");	/* Retorna !=0 se nao leu */
		U0puts("\r\nErro!\r\n");
		}
	else	{
		LCDcomando(1);
		LCDputs("Leu Ofs=");
		LCDprbyte(offset);
		LCDcomando(0xc0);
		LCDputs(buf);
		}
	U0puts("\r\n");
	PrByte(offset);
	U0putchar(':');	/* abre aspas " */
	for(x=0; x<16; x++){
		PrByte(buf[x]);
		U0putchar(' ');
		}
	U0puts(":\"");	/* abre aspas " */
	U0puts(buf);		/* Esceve texto lido do smcard */
	U0puts("\"\r\n");		/* fecha aspas */
	}
return 0;
}

/* Estas rotinas sao chamados pelo crt.s. 
Devem existir, mas ainda nao estao sendo usadas */
void UNDEF_Routine(){}
void SWI_Routine(){}
void FIQ_Routine(){}
