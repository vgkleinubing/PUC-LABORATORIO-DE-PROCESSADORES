/************************************************
Monitor LPC2378
Marcos Augusto Stemmer
*************************************************/
#ifndef STRING_H
#define STRING_H
/* Mascaras do U0LSR */
#define THREMPTY	0x20
#define RXDREADY	0x01
/* Configuracoes especificas */
#define CRYSTALFREQ 12000000
//#define BAUDRATE 9600

/* Maneiras de interpretar umendereco: */
union address
	{
	unsigned long w;	/* Como numero */
	unsigned char *p;	/* Como apontador para byte */
	unsigned long *pw;	/* Como apontador para word */
	void (*f)(void);	/* Como apontador para funcao (execute) */
	};

void UART0_Init(void);
void UART0puts(char *c);
char UART0getchar(void);
void UART0putchar(char c);
int GetStr(char *s, int n);
void PrByte(char c);
unsigned long getnum(char *str, int radix);
void escreve_num(int num);
unsigned long getaddress(void);
int lehex(unsigned long base);	/* Le arquivo Intel hex */
void dump(void);			/* Mostra bytes em hexadecimal */
void copy(void);			/* Copia blocos de memoria */
void execute(void);			/* Executa um programa */
void relogio(void);
void acerta_relogio(void);
void escreve_mem(void);
void escreve_word(void);
void prword(unsigned int w);
void pr2dec(int x);
void IAP_functions(void);

/* Modulo lcd.c */
void LCDinit(void);
void LCDcomando(int c);
void LCDputchar(int c);
void LCDputs(char *s);
void espera(long int k);
void ad0config(void);
int le_adc0(void);
void LCDprompt();
#endif
