/******************************************
Cabecalho da configuracao do PLL e da UART0
M. Stemmer 2009-2
*******************************************/
/* Mascaras do PLLSTAT */
#define PLOCK  (1<<10)
/* Mascaras do U0LSR */
#define THREMPTY	0x20
#define RXDREADY	0x01
/* Configuracoes especificas 
Com cristal de 12 MHz, PLLMUTI=16 fazemos o cco oscilar a 384 MHz.
A frequencia do CCO (Current Controlled Oscillator) deve ficar entre 275 MHz e 550 MHz
Com CPUDIVi=4 fazemos a CPU andar a 48MHz (CPUDIVi deve ser par) */
#define CRYSTALFREQ 12000000

int PLL_Init(int pllmulti, int cpudiv);
void U0init(int baudrate);
char U0getchar(void);
void U0putchar(char c);
void U0puts(char *s);
