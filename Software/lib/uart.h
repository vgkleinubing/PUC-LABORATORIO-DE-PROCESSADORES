#include "uart.c"
#define CRYSTALFREQ 12000000
/* Prototipos da UART0 */
void U0init(void);
char U0getchar(void);
void U0putchar(int c);
void U0puts(char *s);
char *U0gets(char *txt, int nmax);

/* Prototipos da UART1 */
void U1init(void);
char U1getchar(void);
void U1putchar(int c);
void U1puts(char *s);
char *U1gets(char *txt, int nmax);
