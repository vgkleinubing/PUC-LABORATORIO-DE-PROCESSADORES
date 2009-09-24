/******************************************************************************
/
/	Breve descrição e autor, demais informações no README
/ 
******************************************************************************/

#include "../lib/lib.h"
#include "gps.h"
//#define SW1	0x100
//#define SW2	0x200
//#define SW3	0x400
//#define SW4	0x800

/* Se CLOCKRTC for definido usa-se oscilsdor espencial de 32768Hz */
/* Se nao for definido usa o clock de 12MHz -*/
//#define CLOCKRTC

void init()
{
	LCDinit();	/* Inicializa o display LCD */
	U0init();	/* Inicializa a serial 0 */
	GPSinit(NMEA);
}

int main (void)
{
	do {
	} while(1);

	return 0;
}

/* Estas rotinas sao chamados pelo crt.s. 
Devem existir, mas ainda nao estao sendo usadas */
void UNDEF_Routine(){}
void SWI_Routine(){}
void FIQ_Routine(){}

