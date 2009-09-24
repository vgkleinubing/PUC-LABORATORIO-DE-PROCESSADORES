/******************************************************************************
/
/       GPS GRAMIN LIBRARY
/
******************************************************************************/

#define NMEA 0
#define GARMIN 1

/******************************************************************************
/	Function to set and init uart for GPS communication	
/
/	PARAM	protocol	GPS protocol, config in GPS
/
******************************************************************************/
void GPSinit(int protocol)
{
	if (protocol == NMEA)
		U1init(4800);
	else
		U1init(9600); //<- verificar na documentacao

}

/******************************************************************************
/	Function to set and init uart for GPS communication	
/
/	PARAM	protocol	GPS protocol, config in GPS
/
******************************************************************************/

