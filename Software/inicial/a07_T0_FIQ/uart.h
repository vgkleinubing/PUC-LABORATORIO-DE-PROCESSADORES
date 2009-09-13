/******************************************
Cabecalho da configuracao da UART0
M. Stemmer 2009-2
*******************************************/
#define SYSCLK 12000000

void PLL_Init(void);
void U0init(void);
int U0getchar(void);
void U0putchar(int c);
void U0puts(char *s);
