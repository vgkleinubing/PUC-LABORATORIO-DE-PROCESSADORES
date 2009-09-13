/******************************************
Cabecalho da configuracao do PLL e da UART0
M. Stemmer 2008
*******************************************/
/* Mascaras do U0LSR */
#define THREMPTY	0x20
#define RXDREADY	0x01
/* Configuracoes especificas */
#define CRYSTALFREQ 12000000
//#define BAUDRATE 9600

void UART0_Init(void);
char UART0getchar(void);
void UART0putchar(char c);
void UART0puts(char *s);
