/*
 (C) Copyright 2016, TP-Link Inc, konstantin.mauch@tp-link.com

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License as
 published by the Free Software Foundation; either version 2 of
 the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT any WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 MA 02111-1307 USA
*/

/* stdout, NULL */
#include <stdio.h>

/* sprintf() */
#include <string.h>

/* <_1_SCLK_HI_W> et al */
#include "beagle.h"


/* Length of data word at AD53xx converter. TODO: move away */
#define AD5300_DATA_LEN		16

/* Amount of bits (within data word) to be ignored while passing 8-bit data value to AD53xx converter. TODO: move away */
#define AD5300_DONTCARE_LEN	4


/* Longest command line ever required by this app. */
#define MEDIUM_SIZE 1024

/* Command line buffer */
char pcCmdBuffer[MEDIUM_SIZE];


char * GPIOs[] = {
	 /* P9, left side */
	 "30", "31", "48" , "60", "50", "51", "5", "5", 

	 /* P9, right side */
	 "5", "5", "5" , "5", "5", "5", "5",  "5",

	 /* P8, left side */ 
	 "5", "5", "5", "5", "5", "5", "5",

	 /* P8, right side */ 
	 "5", "5", "5", "5", "5", "5", "5" 
}; /* char * GPIOs[] */

/* Array of pointers to GPIO files */
FILE * GPIO_VALUE_FILES[30];

/* Make GPIO port <pcPortStr>: a) to appear in the system; b) to become output port; */
static void OpenGPIO(char * pcPortStr)
{
	if  (NULL != pcPortStr)
	{
		/* Command prepare */
		sprintf (pcCmdBuffer, "echo %s > /sys/class/gpio/export", pcPortStr);

		/* Run command in <sh> shell */
		system (pcCmdBuffer);

		/* Wait for driver create filesystem components </sys/class/gpio/gpio%s/> */
		usleep(1000);

		/* Command prepare */
		sprintf (pcCmdBuffer, "echo out > /sys/class/gpio/gpio%s/direction", pcPortStr);

		/* Run command in <sh> shell */
		system (pcCmdBuffer);

		/* Wait for driver stabilize tartget GPIO port */
		usleep(1000);
	}
	else
		printf("[%s] [%s] empty Port ID \n",__FILE__, __func__ );
}

/* Toggle GPIO port <pcPortStr> ON */
#if defined(SH_FOPS)
void OnGPIO(char * pcPortStr)
#else
void OnGPIO(FILE * fcPortFile)
#endif /* (0) */
{
#if defined(SH_FOPS)
	if  (NULL != pcPortStr)
#else
	if  (NULL != fcPortFile)
#endif /* (0) */
	{
#if defined(SH_FOPS)
		/* Command prepare */		
		sprintf (pcCmdBuffer, "echo 1 > /sys/class/gpio/gpio%s/value", pcPortStr);

		/* Run command in <sh> shell */
		system (pcCmdBuffer);
#else
		fwrite("1", 1,1, fcPortFile);
		fflush(fcPortFile);
#endif /* (0) */

		/* Time ballast */
//.		usleep(1);
	}
	else
		printf("[%s] [%s] empty Port ID \n",__FILE__, __func__ );
}

/* Toggle GPIO port <pcPortStr> OFF */
#if defined(SH_FOPS)
void OffGPIO(char * pcPortStr)
#else
void OffGPIO(FILE * fcPortFile)
#endif /* (0) */
{
#if defined(SH_FOPS)
	if  (NULL != pcPortStr)
#else
	if  (NULL != fcPortFile)
#endif /* (0) */
	{
#if defined(SH_FOPS)
		/* Command prepare */
		sprintf (pcCmdBuffer, "echo 0 > /sys/class/gpio/gpio%s/value", pcPortStr);

		/* Run command in <sh> shell */
		system (pcCmdBuffer);
#else
		fwrite("0", 1,1, fcPortFile);
		fflush(fcPortFile);
#endif /* (0) */

		/* Time ballast */
//.		usleep(2);
	}
	else
		printf("[%s] [%s] empty Port ID \n",__FILE__, __func__ );
}

void _i_AD5300_Write_W(unsigned char data, int iIdx) 
{
unsigned short tmp;

unsigned char iCnt;

	tmp = data << AD5300_DONTCARE_LEN;

	_i_AD5300_ACT_W ( SYNC_i_W [iIdx] );

	for (iCnt = 0; iCnt < AD5300_DATA_LEN; iCnt++)
	{
		_i_SCLK_HI_W( SCLK_i_W[iIdx] );

		(tmp & (unsigned short)( 1U << (15 - iCnt) ) ) ? (_i_MOSI_HI_W( MOSI_i_W[iIdx] )) : (_i_MOSI_LO_W( MOSI_i_W[iIdx] ));

		_i_SCLK_LO_W( SCLK_i_W[iIdx] );
	}

	_i_AD5300_DEACT_W ( SYNC_i_W [iIdx] );

} /* void _i_AD5300_Write_W(unsigned short data, int iIdx) */

void _i_AD5300_Write_G(unsigned char data, int iIdx) 
{
unsigned short tmp;

unsigned char iCnt;

	tmp = data << AD5300_DONTCARE_LEN;

	_i_AD5300_ACT_G ( SYNC_i_G [iIdx] );

	for (iCnt = 0; iCnt < AD5300_DATA_LEN; iCnt++)
	{
		_i_SCLK_HI_G( SCLK_i_G[iIdx] );

		(tmp & (unsigned short)( 1U << (15 - iCnt) ) ) ? (_i_MOSI_HI_G( MOSI_i_G[iIdx] )) : (_i_MOSI_LO_G( MOSI_i_G[iIdx] ));

		_i_SCLK_LO_G( SCLK_i_G[iIdx] );
	}

	_i_AD5300_DEACT_G ( SYNC_i_G [iIdx] );

} /* void _i_AD5300_Write_G(unsigned short data, int iIdx) */


void AD5300_Init() 
{
int iIdx, iPdx;

	memset( (void*) GPIO_VALUE_FILES, 0, sizeof (GPIO_VALUE_FILES) );

	/* Initialize GPIO ports */
	for (iIdx = 0; iIdx < sizeof(GPIOs)/sizeof(GPIOs[0]);iIdx++ )
	{
//.		printf("[%s] [%s] opening GPIO %s \n",__FILE__, __func__ , GPIOs[iIdx] );

		OpenGPIO( GPIOs[iIdx] );	
	}
	printf("[%s] [%s] opened all GPIO ports \n",__FILE__, __func__ );

#if !defined(SH_FOPS)

	/* Open GPIO value files and store file poniters in array <GPIO_VALUE_FILES> */
	for (iIdx = 0; iIdx < sizeof(GPIO_VALUE_FILES)/sizeof(GPIO_VALUE_FILES[0]);iIdx++ )
	{
		printf("[%s] [%s] opening GPIO value file for GPIO %s \n",__FILE__, __func__ , GPIOs[iIdx] );

		/* Filename prepare */
		sprintf (pcCmdBuffer, "/sys/class/gpio/gpio%s/value", GPIOs[iIdx]);

		/* Try to open value file <pcCmdBuffer> */
		if ( NULL == (GPIO_VALUE_FILES[iIdx] = fopen (pcCmdBuffer, "wb+") ) )
		{
			printf("[%s] [%s] can't open GPIO value file <%s> \n", __FILE__, __func__ , pcCmdBuffer);

			continue;
		}

	} /* for (iIdx = 0; ... */

	/* GPIOs connected to SCKL with 'white' wire */
	for (iPdx = 0; iPdx < (NUM_PORTS-1); iPdx++)
		SCLK_i_W[iPdx] = GPIO_VALUE_FILES[NUM_PORTS*iPdx];

	/* GPIOs connected to MOSI with 'white' wire */
	for (iPdx = 0; iPdx < (NUM_PORTS-1); iPdx++)
		MOSI_i_W[iPdx] = GPIO_VALUE_FILES[NUM_PORTS*iPdx + 1];

	/* GPIOs connected to SYNC with 'white' wire */
	for (iPdx = 0; iPdx < (NUM_PORTS-1); iPdx++)
		SYNC_i_W[iPdx] = GPIO_VALUE_FILES[NUM_PORTS*iPdx + 2];

	/* GPIOs connected to SCKL with 'green' wire */
	for (iPdx = 0; iPdx < (NUM_PORTS-1); iPdx++)
		SCLK_i_G[iPdx] = GPIO_VALUE_FILES[NUM_PORTS*iPdx + 3];

	/* GPIOs connected to MOSI with 'green' wire */
	for (iPdx = 0; iPdx < (NUM_PORTS-1); iPdx++)
		MOSI_i_G[iPdx] = GPIO_VALUE_FILES[NUM_PORTS*iPdx + 4];

	/* GPIOs connected to SYNC with 'green' wire */
	for (iPdx = 0; iPdx < (NUM_PORTS-1); iPdx++)
		SYNC_i_G[iPdx] = GPIO_VALUE_FILES[NUM_PORTS*iPdx + 5];

#endif /* !defined(SH_FOPS) */

} /* void AD5300_Init()  */


void AD5300_Test() 
{
int iIdx;

	while (1) 
	{
#if defined(SH_FOPS)
		/* Cyclically toggle GPIO ports ON */
		for (iIdx = 0; iIdx < sizeof(GPIOs)/sizeof(GPIOs[0]);iIdx++ )
		{
//.			printf("[%s] [%s] toggling GPIO %s ON\n",__FILE__, __func__ , GPIOs[iIdx] );
			OnGPIO( GPIOs[iIdx] );
		}

		/* Cyclically toggle GPIO ports OFF */
		for (iIdx = 0; iIdx < sizeof(GPIOs)/sizeof(GPIOs[0]);iIdx++ )
		{
//.			printf("[%s] [%s] toggling GPIO %s OFF\n",__FILE__, __func__ , GPIOs[iIdx] );
			OffGPIO( GPIOs[iIdx] );
		}
#else		

#if 0
		/* For each CPE port except last one (which is going to be USB 3.0, and conseq. requires special handling) */
		for (iIdx = 0; iIdx < NUM_PORTS - 1 /* skip USB 3.0 Port */;iIdx++ )

		{
			/* Test */
			_i_AD5300_Write_W(0xAA, iIdx); /* CH1: blue oscilloscope beam */
			_i_AD5300_Write_G(0x88, iIdx); /* CH2: yellow oscilloscope beam */
		}
#else

		_i_AD5300_Write_W( 0 , 0);
			_i_AD5300_Write_G(0xFF, 0);
		_i_AD5300_Write_W(0x11, 0);
			_i_AD5300_Write_G( 0, 0);
		_i_AD5300_Write_W(0xAA, 0);
			_i_AD5300_Write_G( 0x11, 0);
		_i_AD5300_Write_W(0xFF, 0);
			_i_AD5300_Write_G( 0xAA, 0);

#endif /* (0) */


#endif /* defined(SH_FOPS) */

	} /* while (1) */

} /* void AD5300_Test()  */

/* Closing opened port */
void AD5300_Deinit() 
{
int iIdx;

#if !defined(SH_FOPS)

	/* Open GPIO value files and store file poniters in array <GPIO_VALUE_FILES> */
	for (iIdx = 0; iIdx < sizeof(GPIO_VALUE_FILES)/sizeof(GPIO_VALUE_FILES[0]);iIdx++ )
	{
		printf("[%s] [%s] closing GPIO value file for GPIO %s \n",__FILE__, __func__ , GPIOs[iIdx] );

		/* Try to close value file <pcCmdBuffer> */
		if ( 0 != fclose ( GPIO_VALUE_FILES[iIdx] ) )
		{
			printf("[%s] [%s] can't close GPIO value file  <%s>. Nevertheless proceeding with next one\n", __FILE__, __func__ , pcCmdBuffer);
		}

	}/* for (iIdx = 0; ... */

#endif
}/* void AD5300_Deinit() */
