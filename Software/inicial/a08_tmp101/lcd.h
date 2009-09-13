/******************************************
Cabecalho da configuracao do LCD
M. Stemmer 2009-1
*******************************************/
/* Mascaras da P1 para os botoes */
#define SW1 0x100
#define SW2 0x200
#define SW3 0x400
#define SW4 0x800
#define SW5 0x1000
/* Display LCD */
#define LCD_E	0x01000000
#define LCD_RS	0x02000000
#define LCD_D	0x000000ff
/* Endereco i2c do TMP101 */
#define ADDRTMP 0x90
#define CRYSTALFREQ 12000000
/* Agora o BAUDRATE e'definido na linha de comando do gcc */
//#define BAUDRATE 9600

void UART0_Init(void);
char UART0getchar(void);
void UART0putchar(char c);
void UART0puts(char *s);

/* Modulo lcd.c */
void LCDinit(void);
void LCDcomando(int c);
void LCDputchar(int c);
void LCDputs(char *s);
void espera(int t);
