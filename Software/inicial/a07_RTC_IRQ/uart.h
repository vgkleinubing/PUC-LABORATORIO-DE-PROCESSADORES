#define SYSCLK 12000000

void U0init(void);
int U0getchar(void);
void U0putchar(int c);
void U0puts(char *s);
/* Prototipos do LCD */
void LCDinit();
void LCDputchar(int c);
void LCDcomando(int c);
void LCDputs(char *s);
