
/* Controle do LCD na porta P4 */
#define LCD_E  (1<<24)
#define LCD_RS (1<<25)
/* Dados do LCD nos bits 0 a 7 da P3 */
#define CRYSTALFREQ 12000000

void espera(int t);
void LCDcomando(int c);
void LCDputchar(int c);
/* Escreve texto no LCD */
void LCDputs(char *txt);
void LCDinit(void);

/* Prototipos da UART */
void U0init(void);
int U0getchar(void);
void U0putchar(int c);
void U0puts(char *txt);
/* suporte.c */
void letexto(char *txt, int nmax);
void PrByte(int c);
void LCDprbyte(int c);
void escrevenum(int num);
int lenumero(char *str);
int lesaldo(int offset);
