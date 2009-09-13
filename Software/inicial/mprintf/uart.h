
#define CRYSTALFREQ 12000000
/* Prototipos da UART0 */
void U0init(void);
char U0getchar(void);
void U0putchar(int c);
void U0puts(char *s);
/* Prototipos do LCD */
void LCDinit();
void LCDputchar(int c);
void LCDcomando(int c);
void LCDputs(char *s);
char *U0gets(char *txt, int nmax);
/* Prototipos do mprintf */
int atoi(char *str);
double atod(char *str);
int dprint(double x, int campo, int frac, void (*putc)(int));
int mprintf(void (*putc)(int), const char *format, ... );
int sprintf(char *, const char *format, ... );
