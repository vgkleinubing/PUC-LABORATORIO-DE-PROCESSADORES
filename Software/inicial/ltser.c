/*
This program shows how to setup the console and serial interface
to implement a dumb terminal in LINUX

What you type on the keyboard is sent to the serial interface;
The characters received by the serial interface are written on the screen
by Marcos Augusto Stemmer
This program is Free Software
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/time.h>

int sd;    /* Serial port handle */
struct termios mytty, kbdios;   /* Console settings */
char *errmsg[]={"OK", "File read error","Checksum error", "No answer"};

/* This function is called whenever the program terminates */
void back_to_normal(void)
{
/* Set console back to normal */
fcntl(STDIN_FILENO, 0, FNDELAY);
tcsetattr(STDIN_FILENO, TCSANOW, &kbdios);
}

void kbd_rawmode()
{
/* Set keyboard to raw mode */
tcgetattr(STDIN_FILENO, &kbdios);
atexit(back_to_normal);         /* Restore tty mode when exiting */
memcpy(&mytty, &kbdios, sizeof(struct termios));
mytty.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
tcsetattr(STDIN_FILENO, TCSANOW, &mytty);
/* These calls make read to return imedately even when there is
   no data in the input stream */
fcntl(sd, F_SETFL, FNDELAY);
fcntl(STDIN_FILENO, F_SETFL, FNDELAY);
}

int OpenSerial(char *serial_dev, int baudrate)
{
char msg[80];
int baudflag;
sd=open(serial_dev, O_RDWR | O_NDELAY);
if(sd==-1)
  {
  sprintf(msg, "\nUnable to open \"%s\"", serial_dev);
  perror(msg);
  return -1;
  }
/* Set attributes of the serial interface to raw mode */
if(tcgetattr(sd, &mytty)==-1)
  {
  perror(NULL);
  close(sd);
  return -1;
  }
mytty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP
               |INLCR|IGNCR|ICRNL|IXON);
mytty.c_oflag &= ~OPOST;
mytty.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
mytty.c_cflag &= ~(CSIZE|PARENB);
mytty.c_cflag |= CS8;
/* Set Baud Rate to 9600 baud */
switch(baudrate)
	{
	case 115200: baudflag=B115200; break;
	case 57600: baudflag=B57600; break;
	case 38400: baudflag=B38400; break;
	case 19200: baudflag=B19200; break;
	case 4800: baudflag=B4800; break;
	default: baudrate=9600; baudflag=B9600; break;
	}
cfsetispeed(&mytty, baudflag);
cfsetospeed(&mytty, baudflag);
if(tcsetattr(sd, TCSANOW, &mytty)==-1)
  {
  perror(NULL);
  close(sd);
  return -1;
  }

fprintf(stderr, 
"Opened \"%s\" @ %d baud\n"
"Press Ctrl-X to exit\n", serial_dev, baudrate);
return 0;
}

/* Retorna o ponto que vem depois do ultimo ponto no nome do arquivo */
char *extencao(char *nome)
{
char *p;
for(p=nome; *nome; nome++) if(*nome=='.') p=nome;
return p;
}

/* Envia arquivo em formato HEX da intel */
void envia_hex(char *nomearq)
{
FILE *arq;
char bloco[256];
int n,ack, erro;
fd_set descritores;     /* Set of i/o handles */
struct timeval timeout;
arq=fopen(nomearq,"rb");
erro=0;
if(!arq) { perror(NULL); return; }
do{
  fgets(bloco,80,arq);
  write(sd, bloco, strlen(bloco));
  FD_ZERO(&descritores);  /* Inicializa a lista de handles */
  FD_SET(sd,  &descritores);
  timeout.tv_sec=5;
  timeout.tv_usec=0;
  select(FD_SETSIZE, &descritores, NULL, NULL, &timeout);
  ack=16*bloco[7]+bloco[8];
  if((n=read(sd,bloco, 80))<0) { erro=3; break; }
  write(STDIN_FILENO, bloco, n);
  if(*bloco!='.' && *bloco!='E') { erro=3; break; }
  } while(!feof(arq) && ack!=(16*'0'+'1'));
fprintf(stderr, "%s\n", errmsg[erro]);
fclose(arq);
}

/* Envia arquivo binario */
void envia_arquivo(char *nomearq, int endini)
{
FILE *arq;
char chksum, ack, ff=0xff, bloco[256];
int tamanho, k, nrep, erro;
fd_set descritores;     /* Set of i/o handles */
struct timeval timeout;
arq=fopen(nomearq,"rb");
erro=ack=0;
if(!arq) { perror(NULL); return; }
do
  {
  if(!ack) { tamanho = fread(bloco, 1, 256, arq); nrep=3; }
  if(!tamanho) { erro=1; break; }
  write(sd, &ff,1);
  write(sd, &endini,2);
  write(sd, &tamanho,2);
  write(sd, bloco, tamanho);
  for(k=0, chksum=0; k<tamanho; k++) chksum -= bloco[k];
  write(sd, &chksum, 1);
  FD_ZERO(&descritores);  /* Inicializa a lista de handles */
  FD_SET(sd,  &descritores);
  timeout.tv_sec=5;
  timeout.tv_usec=0;
  select(FD_SETSIZE, &descritores, NULL, NULL, &timeout);
  if(read(sd,&ack, 1)<0) { erro=3; break; }
  if(!ack) { endini += tamanho; write(STDIN_FILENO, ".",1); }
  else nrep--;
  if(!nrep) { erro=2; break; }
  }
while(!feof(arq));
fprintf(stderr, "%s\n", errmsg[erro]);
fclose(arq);
}

int main(int argc, char **argv)
{
char letra[4], *portname, *filename, *ext;
int endini, i, n;
int baudrate;
fd_set descritores;     /* Set of i/o handles */
/* Make shure that input is a keybord */
if(!isatty(STDIN_FILENO))
  {
  fputs("\nInput must be from a keyboard.\n", stderr);
  return -1;
  }
filename=NULL;
endini=0;
/* Open serial port */
portname="/dev/ttyUSB0";
baudrate=-1;
for(i=1; i<argc; i++)
  {
  if(*argv[i]=='b' && argv[i][1]=='=') 
	{ 
	baudrate=atoi(argv[i]+2); continue; 
	}
  if(memcmp("/dev/",argv[i],5)) filename=argv[i];
  else portname=argv[i];
  }
puts("\nSerial terminal/uploader for Linux: Command line:\n"
"\tltser [serial_device] [b=baudrate] [file.hex]\n");

if(OpenSerial(portname, baudrate)) return 1;
kbd_rawmode();
/* Now we may use the serial interface and keyboard for sending
  just one character at a time */

if(filename!=NULL)
  {
  ext=extencao(filename);
  if(!strcmp(ext,".ihx") || !strcmp(ext,".hex")) envia_hex(filename);
  }

do
  {
/* Espera receber algo na porta serial (sd) 
   ou no teclado (STDIN_FILENO) */
  FD_ZERO(&descritores);  /* Inicializa a lista de handles */
  FD_SET(sd,  &descritores);
  FD_SET(STDIN_FILENO, &descritores);
  select(FD_SETSIZE, &descritores, NULL, NULL, NULL);

  while((n=read(sd, letra, 4)) > 0) write(STDIN_FILENO, letra, n);
  *letra='\0';
  while((n=read(STDIN_FILENO, letra,4)) > 0) write(sd, letra, n);
  } while(*letra!='X'-64);
close(sd);
return 0;
}
