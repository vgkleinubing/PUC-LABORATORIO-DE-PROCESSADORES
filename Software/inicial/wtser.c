/*******************************************************************
Programa wtser: Terminal serial RS232 
para uma janela de Console do Windows.
Recomendo compilar com o Dev-C++ (www.bloodshed.net)
Executar na linha de comando:
	wtser [b=baudrate] [com1 ou com2] [arquivo.hex]
Os parâmetros são opcionais, sendo por omissão b=9600 com1, sem
enviar arquivo hex.
***********************************************************************
Escrito por Marcos A. Stemmer
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License.
***********************************************************************/
#include <windows.h>
#include <mem.h>
#include <io.h>
#include <conio.h>
#include <stdio.h>
/****************************************************************
Abre uma porta de comunicação RS232
Parâmetros:
	char nomecom = Nome da porta serial: "COM1" ou "COM2"
	int baudrate
Retorna:
	Handle do arquivo de comunicação se foi bem sucedido
	INVALID_HANDLE_VALUE de não conseguiu.
******************************************************************/
HANDLE AbreComm(char *nomecom, int baudrate)
{
HANDLE hcom;
DCB cdcb;
COMMTIMEOUTS comto;

hcom = CreateFile(nomecom, 
GENERIC_READ | GENERIC_WRITE,0,NULL,OPEN_EXISTING,
FILE_ATTRIBUTE_NORMAL,NULL);

if(hcom == INVALID_HANDLE_VALUE)
  {
  fprintf(stderr, "Não abriu a %s\n", nomecom);
  return hcom;
  }
GetCommState(hcom, &cdcb);
cdcb.BaudRate    = baudrate;
cdcb.ByteSize    = 8;
cdcb.StopBits    = ONESTOPBIT;
cdcb.Parity      = NOPARITY;
cdcb.fDtrControl = DTR_CONTROL_DISABLE;
cdcb.fOutX       = FALSE;
cdcb.fInX        = FALSE;
cdcb.fNull       = FALSE;
cdcb.fRtsControl = RTS_CONTROL_DISABLE;

if(!SetCommState(hcom,&cdcb)) {
  fputs("SetCommState", stderr);
  return INVALID_HANDLE_VALUE;
  }

SetCommMask(hcom,EV_RXCHAR | EV_TXEMPTY);
//GetCommTimeouts(hcom, &comto);
comto.ReadIntervalTimeout         = MAXDWORD;
comto.ReadTotalTimeoutMultiplier  =    0;
comto.ReadTotalTimeoutConstant    =    1;
comto.WriteTotalTimeoutMultiplier =    0;
comto.WriteTotalTimeoutConstant   =    0;
SetCommTimeouts(hcom, &comto);
return hcom;
}

/* Separa a extenção do nome do arquivo */
char *extencao(char *nome)
{
char *p;
for(p=nome; *nome; nome++) if(*nome=='.') p=nome;
return p;
}

/***** Envia um arquivo hex *******
 Envia uma linha de cada vez. 
 Após cada linha spera um caractere de resposta. */
int envia_hex(char *nomearq, HANDLE hcom)
{
char buf[256];
FILE *arq;
unsigned long n;
int k;
arq=fopen(nomearq,"rb");
if(arq==NULL)
  {
  perror(nomearq);
  return -1;
  }
do {
   fgets(buf,256,arq);
   if(feof(arq) || *buf!=':') break;
   WriteFile(hcom, buf, strlen(buf), &n, NULL);
   k=5000;
   do	{
	Sleep(1);
	ReadFile(hcom, buf, 1, &n, NULL);
	} while(!n && k--);
   if(!k) {
	fputs("Não responde...\n", stderr);
	return -1;
	}
   putch(*buf);
   } while(!(buf[7]=='0' && buf[8]=='1'));
fclose(arq);
return 0;
}

int main(int argc, char **argv)
{
HANDLE hcom;
int k;
unsigned long n;
char *ext, *pnome;
char *ncom="COM1";
int baud=9600;
char nomearq[80];
pnome=NULL;
for(k=1; k<argc; k++)
  {
  for(ext=argv[k]; *ext && *ext!='='; ext++);
  if(*ext && *argv[k]=='b') baud=atoi(ext+1);
  if((baud < 300) || (baud > 115200)) baud=9600;
  if((!memcmp(argv[k],"com",3) || !memcmp(argv[k],"COM",3)) 
		&& isdigit(argv[k][3]))
	{
	ncom=argv[k];
	continue;
	}
  ext=extencao(argv[k]);
  if(!strcmp(ext,".ihx") || !strcmp(ext,".hex")) pnome=argv[k];
  }
hcom=AbreComm(ncom,baud);
if(hcom==INVALID_HANDLE_VALUE) return 1;
printf("Terminal %s %dbaud\nCtrl-X para sair\n"
"Ctrl-U (Upload) Envia arquivo hex\n", ncom, baud);
if(pnome!=NULL) envia_hex(pnome,hcom);
do {
//   Sleep(1);
   ReadFile(hcom, nomearq, 1, &n, NULL);
   if(n) putchar(*nomearq);
   if(kbhit())
	{
	*nomearq=getch();
	WriteFile(hcom, nomearq, 1, &n, NULL);
	}
   if(*nomearq=='U'-64)
	{
	printf("Envia arquivo hex.\nNome do arquivo=");
	fgets(nomearq,80,stdin);
	for(k=0; nomearq[k]>' '; k++);
	nomearq[k]='\0';
	envia_hex(nomearq,hcom);
	}
   } while(*nomearq!='X'-64);
return 0;
}
