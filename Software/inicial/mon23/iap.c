/************************************************
Monitor LPC2378
Marcos Augusto Stemmer
*************************************************/
#include <arch/nxp/lpc23xx.h>
#include "monarm.h"

typedef void (*IAP)(unsigned *rin, unsigned *rout);
#define IAP_ENTRY ((IAP)0x7ffffff1)
unsigned regs[5];

void show_status(void)
{
static const char *sstatus[]={
"Done","Command error", "Invalid src addr","Invalid dest addr","Src not mapped",
"Dest not mapped","Count_error","Invalid sector",
"Sector not blank","Sector not prepared", "Compare error", "Busy"};
LCDcomando(1);
LCDputs("Status=");
escreve_num(regs[0]);
UART0puts(": ");
UART0puts((char *)sstatus[regs[0]]);
UART0puts("!\n");
}

int do_it()
{
char yes[8];
UART0puts(" Do it (yes)? ");
GetStr(yes,4);
return ((*((int *)yes) & 0xffffff)==0x736579);
}

void partID()
{
UART0puts("Read part ID: ");
regs[0]=54;
IAP_ENTRY(regs,regs);
show_status();
LCDcomando(1);
LCDputs(" ID=");
escreve_num(regs[1]);
UART0puts(" ID=0x");
prword(regs[1]);
}

unsigned sect2addr(int sector)
{
unsigned addr;
addr= (sector << 12);
if(sector > 8) addr=(sector << 15)-0x38000;
if(sector > 0x16) addr=(sector << 12)+0x62000;
return addr;
}

unsigned addr2sect(unsigned addr)
{
unsigned sect;
sect = (addr >> 12);
if(addr > 0x8000) sect = (addr >> 15) + 7;
if(addr > 0x78000) sect = (addr >> 12) - 0x56;
return sect;
}

void apaga(int setori, int setorf)
{
UART0puts("Erase sectors 0x");
PrByte(setori);
UART0puts(" to 0x");
PrByte(setorf);
UART0puts("\nAddress = 0x");
prword(sect2addr(setori));
prword(sect2addr(setorf+1)-1);
if(!do_it()) return;
regs[0]=50;	/* Prepare sectors for writing */
regs[1]=setori;
regs[2]=setorf;
IAP_ENTRY(regs,regs);
regs[0]=52;	/* Erase sectors */
regs[1]=setori;
regs[2]=setorf;
regs[3]=CRYSTALFREQ;
IAP_ENTRY(regs,regs);
show_status();
}

void grava(unsigned addr, unsigned fonte, int n)
{
unsigned sector;
int setorf, kn;
sector=addr2sect(addr);
setorf=addr2sect(addr+n);
PrByte(setorf);
UART0puts(" Recording 0x");
prword(addr); prword(addr+n-1);
if(!do_it()) return;
regs[0]=53;	/* Blanck check */
regs[1]=sector;
regs[2]=setorf;
IAP_ENTRY(regs,regs);
if(regs[0]) show_status();
kn=0x1000;
while(n >= 0x100){
	if(n < 0x1000) kn=0x400;
	while(n < kn) kn >>=1;
	regs[0]=50;	/* Prepare sectors for writing */
	regs[1]=sector;
	regs[2]=setorf;
	IAP_ENTRY(regs,regs);
	regs[0]=51;	/* Copy RAM to FLASH */
	regs[1]=addr;
	regs[2]=fonte;
	regs[3]=kn;
	regs[4]=CRYSTALFREQ;
	IAP_ENTRY(regs,regs);
	show_status();
	n-=kn; addr+=kn; fonte+=kn;
	}
}

void IAP_functions(void)
{
int c, sect, nbytes;
char input[40];
UART0puts("\nFlash programming: IAP\n"
	"b Boot code version\n"
	"i Read part ID\n"
	"w Write: Copy RAM to FLASH\n"
	"e Erase sector\n");
do	{
	UART0puts("\n=>");
	c=UART0getchar();
	switch(c){
		case 'b': regs[0]=55; IAP_ENTRY(regs,regs);
			UART0puts("Boot code version=");
			PrByte(regs[1]);
			UART0putchar('.');
			PrByte(regs[2]);
			break;
		case 'i': partID(); break;
		case 'w':
			UART0puts("Write to address: 0x");
			GetStr(input,40);
			sect=getnum(input,16);
			UART0puts("How may bytes? 0x");
			GetStr(input,40);
			nbytes=getnum(input,16);
			grava(sect,0x40002300,nbytes); break;
		case 'e':
			UART0puts("Erase sector: 0x");
			GetStr(input,40);
			sect=getnum(input,16);
			apaga(sect,sect);
			break;
		}
	} while(c!=0x1b);

}
