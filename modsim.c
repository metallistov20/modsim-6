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

/* strcpy() */
#include <string.h>

/* getopt_long() */
#include <getopt.h>

/* abort(), SIGABT */
#include <stdlib.h>


/* Own interface, definitions */
#include "modsim.h"

/* Data structures, mocros */
#include "datastruct.h"

/* Port D definitions and prototypes */
#include "port_d.h"

/* Pointer to raw data file */
FILE *fp = NULL;

/* Bufer to store scanned raw data */
char cBuf [LARGE_BUF_SZ];

#if !defined(QUASIFLOAT) 
	float fltTM, fltDIn, fltDOut;
#else
	QuasiFloatType qfltTM, qfltDIn, qfltDOut;
#endif /* !defined(QUASIFLOAT) */

/* Pointer to a dynamic structure to store raw data to */
pTimepointType pTimeChain;

/* Aux. var to tell whether it's a first time we output a message for this 'second'  */
int iOldSec;


/* Single operation names */
static struct option long_options[] =
{
	/* USB 1.1 on CPE#0 Gate */
	{"CPE#0",  no_argument, 		0,'1'},

	/* USB 2.0 on CPE#1 Gate */
	{"CPE#1",  no_argument, 		0,'2'},

	/* End of array */
	{0, 0, 0, 0}
};

static int iBadUsage(char * cArg)
{
#if defined(UCSIMM)
		printf("%s: bad usage; must be either <%s -1> ether <%s -2>; exiting.\n", cArg, cArg, cArg );
#else
		printf("%s: bad usage; must be either one of these: <%s --CPE#0>, <%s -CPE#1>, <%s -1> ether <%s -2>; exiting.\n", cArg, cArg, cArg, cArg, cArg );
#endif /* defined(UCSIMM) */

		return -1;
} /* static int iBadUsage(char * cArg) */

/* Variable to tell whether we're to work on CPE#0 termical or on CPE#1 terminal */
int iOperation;

int main (int argc, char **argv)
{
int iOption;

/* Index of the option */
int iOptionIdx = 0;

char cArg0[LARGE_BUF_SZ];

#if defined(HW_PORTD_TEST)

	printf("[%s] %s: NOTIFICATION: doing test of Port 'D' of MC68EZ328 controller. Will hang in this test. \n", __FILE__, __func__);

	/* Put port D into initial state */
	PortD_Reset();

	/* Set digital IOs PD0, PD1  as outputs */
	PortD_Prepare( NIX_W | NIX_G);

	/* Set digital IOs PD0, PD1 up and set down, eternally */
	PortD_Toggle( NIX_W ); while (1) { PortD_Toggle( NIX_W | NIX_G  ); usleep(1);	}

#endif /* (defined(HW_DUMB_TEST) ) */


#if defined(HW_AD53_TEST)

	printf("[%s] %s: NOTIFICATION: doing test of AD53xx controllers. Will hang in this test. \n", __FILE__, __func__ );

#if defined(UCSIMM)
	/* Put port D into initial state */
	PortD_Reset();

	/* Besides all, it prepares PIOs, so theres no need to do <PortD_Prepare()> */
	AD5300_Init_W(); AD5300_Init_G();
{
int iA, iB = 0;
	/* Checking on values close to limit, on limits, in the middle, and at random valuoes from range, ..  */
	while (1)
	{
#if 0
		iB++;

		iA = (int)( iB % 31 );
		if (iA < 16 ) AD5300_Write_W ( iA*iA ); else AD5300_Write_W ( (15-(iA-15)) * (15-(iA-15)) ) ;

		iA = (int)( iB % 32 );
		if (iA < 16 ) AD5300_Write_G (225 - iA*iA ); else AD5300_Write_G (  256 - (32-iA) * (32-iA)  ) ;
#endif 

#if 1
		AD5300_Write_W( 0 );
			AD5300_Write_G(0xFF);
		AD5300_Write_W(0x11);
			AD5300_Write_G( 0 );
		AD5300_Write_W(0xAA);
			AD5300_Write_G( 0x11 );
		AD5300_Write_W(0xFF);
			AD5300_Write_G( 0xAA );
#endif 
	}
}
#endif /* defined(UCSIMM) */

#if defined(SITARA)
	/* Open GPIO ports for output */
	AD5300_Init();

	AD5300_Test();

	/* not reached */
	AD5300_Deinit();
#endif /* defined(SITARA) */

#endif /* (defined(HW_DUMB_TEST) ) */


	/* Avoid dafault 0 value */
	iOperation=DO_NO_OP;

	/* Assign program name, requirted for output*/
	strcpy (cArg0, argv[0]);

	/* Parsing command line arguments */
	while (1)
	{
		/* Get each paramter */
		iOption = getopt_long (argc, argv, "12", long_options, &iOptionIdx);

		/* TODO: check if assigning zero is necessary */
		iOptionIdx = 0;

		/* End of the options reached? */
		if (-1 == iOption)
		{
			/* Then break the parsing cycle */
			break;
		}

		/* Parce each parameter */
		switch (iOption)
		{
			case '1':
				printf("%s: option is  <%c>\n", cArg0, iOption);//TODO: remove
				iOperation = DO_GATE0_OP;
				break;

			case '2':
				printf("%s: option is  <%c>\n", cArg0, iOption);//TODO: remove
				iOperation = DO_GATE1_OP;
				break;

			default:
				return iBadUsage(cArg0);
		}
	} /* Command line arguments were parsed */

	if ( DO_GATE0_OP != iOperation && DO_GATE1_OP != iOperation )
		return iBadUsage(cArg0);

	printf("[%s] %s: NOTIFICATION: assuming that CPE is attached to [%s] gate, works according to [%s] protocol.\n", __FILE__, __func__, (DO_GATE0_OP==iOperation)?"CPE#0":"CPE#1", (DO_GATE0_OP==iOperation)?"USB1.1":"USB2.0"   );

	/* Try to open Raw Data file at place defined by 'FILE_NAME' */
	if ( NULL == (fp = fopen (FILE_NAME, "r") ) )
	{
		printf("[%s] %s: can't open file <%s> \n", __FILE__, __func__ , FILE_NAME);

		return P_ERROR;
	}

	printf("[%s] %s: loading USB-data via NFS from file <%s>\n", __FILE__, __func__, FILE_NAME);

	/* For each string of Raw Data file */
	while ( ! (feof (fp) ) ) 
	{
		/* Try to scan a whole string into temp. buffer */
		if (0 > fscanf (fp, "%s", cBuf ) )
		{
			/* EOF reached, or can't scan for some other reason (such as NFS conn. is down) */
		}
		else
		{
		/* Aux. buffer to keep results of parsing */
		char * cpTmp = cBuf;
#if defined(DEBUG_DATA_)
			printf("[%s] %s: scanned: < %s >\n", __FILE__, __func__, cBuf);
#endif /* (DEBUG_DATA) */

#if !defined(QUASIFLOAT) 
			/* Set default values. MISRA RULE #TODO */
			fltTM = fltDIn = fltDOut = 0.0f;
#else
			/* Set default values. MISRA RULE #TODO */
			memset (&qfltTM, 0, sizeof (QuasiFloatType) ) ;
			memset (&qfltDIn, 0, sizeof (QuasiFloatType) ) ;
			memset (&qfltDOut, 0, sizeof (QuasiFloatType) ) ;

#endif /* !defined(QUASIFLOAT) */

			/* For each character in aux. buffer */
			while (*cpTmp)

				/* replace all commas with spaces, to let the <scanf()> parse it */
				{ if (',' == *cpTmp) *cpTmp = ' '; cpTmp++; }

#if defined(DEBUG_DATA_)
			printf("[%s] %s: changed: < %s >\n", __FILE__, __func__, cBuf);
#endif /* (DEBUG_DATA) */

#if !defined(QUASIFLOAT) 
			/* Find 3 floats separated by spaces in aux. buffer */
			sscanf(cBuf, "%f %f %f,", &fltTM,     &fltDIn,   &fltDOut );
#else
sscanf(cBuf, "%d.%d %d.%d %d.%d",
					&(qfltTM.integer),&(qfltTM.fraction),
					&(qfltDIn.integer),&(qfltDIn.fraction),
					&(qfltDOut.integer),&(qfltDOut.fraction)    );
#endif /* !defined(QUASIFLOAT) */

#if defined(DEBUG_DATA_)
#if !defined(QUASIFLOAT) 
			printf("[%s] %s: parsed :  <%f> <%f> <%f>\n", __FILE__, __func__, fltTM, fltDIn, fltDOut );
#else
			printf("[%s] %s: parsed :  <%d.%d>  <%d.%d>  <%d.%d> \n",
					__FILE__, __func__, 
					qfltTM.integer,qfltTM.fraction,
					qfltDIn.integer,qfltDIn.fraction,
					qfltDOut.integer,qfltDOut.fraction   );
#endif /* !defined(QUASIFLOAT) */
#endif /* (DEBUG_DATA) */

#if !defined(QUASIFLOAT) 
			if (iOldSec!= (int)fltTM)
				{iOldSec=(int)fltTM; printf("sec: %d; ", iOldSec); fflush(stdout); }

			/* Attach just scanned data (three floats) to tail of dynamic structure */
			EnrollPoint(&pTimeChain, &fltTM, &fltDIn, &fltDOut, "N/A");
#else
			if (iOldSec!= qfltTM.integer)
				{iOldSec=qfltTM.integer; printf("sec: %d; ", iOldSec); fflush(stdout); }

			/* Attach just scanned data (three floats represented as <INT>.<INT>) to tail of dynamic structure */
			EnrollPoint(&pTimeChain, &qfltTM, &qfltDIn, &qfltDOut, "N/A");
#endif /* !defined(QUASIFLOAT) */
		}
	}

	/* Dispose pointer to Raw Data file */
	fclose(fp);

#if defined(UCSIMM)
	/* Initialize on-board peripherals */
	PeriphInit();
#endif /* (UCSIMM) */

#if defined(SITARA)
	/* Open GPIO ports for output */
	AD5300_Init();
#endif /* defined(SITARA) */

	printf("\n[%s] %s: issuing USB-data on Port 'D'\n", __FILE__, __func__);

	/* Process data stored in dynamic structure pointed by 'pTimeChain' */
	ProcessPoints(pTimeChain);

	printf("\n[%s] %s: disposing memory allocations\n", __FILE__, __func__);

	/* Free memory occupied by dynamically stored raw data */
	DeletePoints(&pTimeChain);

#if defined(SITARA)
	/* Close GPIO-port files, if needed  */
	AD5300_Deinit();
#endif /* defined(SITARA) */

	printf("[%s] %s: done (success) \n", __FILE__, __func__); fflush(stdout);

	return P_SUCCESS;

} /* int main(int argc, char **argv) */
