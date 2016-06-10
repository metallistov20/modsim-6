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

/* printf(), stdout */
#include <stdio.h>

/* realloc() */
#include <stdlib.h>

/* struct timeval, gettimeofday() */
#include <sys/time.h>

/* Basic project definitions  */
#include "modsim.h"

/* Type 'pTimepointType' definition */
#include "datastruct.h"

int ProcessPoint( pTimepointType pTimepoint )
{
// TODO: workaround. normally the data stream should distributed across all of the attached CPEs
int iCPE=0;

// TODO: remove this workaround
#define max(x,y)	((x>y)?x:y)
// TODO: remove this workaround
#define min(x,y)	((x<y)?x:y)
// TODO: remove this workaround
#define CONV_MAX_SCALE 255
// TODO: remove this workaround
#define VDD_VOLTAGE 	5



		/* Parsing-out redundant data from CVS-datafile yet to be implemented */
		none
//	usleep(1000*19/7); //  60 s
//	usleep((25*4*2*100*19/7)/105); // 10 s
	usleep((120*2*100*19/7)/105); // 11 s


		/* Put current value on 'green' wire */
// 07.06.2016 : this line is indifferent for HW->PC
#if 1
#if 0
		/* Logical zero, anyway */
		pTimepoint->ushQuadAvgYval = max(0.012*CONV_MAX_SCALE / VDD_VOLTAGE, pTimepoint->ushQuadAvgYval);
		pTimepoint->ushQuadAvgYval = max(0.012*CONV_MAX_SCALE / VDD_VOLTAGE, pTimepoint->ushQuadAvgYval);
		/* Logical one, anyway */
		pTimepoint->ushQuadAvgXval = min(0.036*CONV_MAX_SCALE / VDD_VOLTAGE, pTimepoint->ushQuadAvgXval);
		pTimepoint->ushQuadAvgYval = min(0.036*CONV_MAX_SCALE / VDD_VOLTAGE, pTimepoint->ushQuadAvgYval);

#endif /* (0) */
		if (pTimepoint->ushQuadAvgYval > 255)pTimepoint->ushQuadAvgYval = 255;  
		if (pTimepoint->ushQuadAvgXval > 255)pTimepoint->ushQuadAvgXval = 255;


		_i_AD5300_Write_W(pTimepoint->ushQuadAvgXval, iCPE);

		/* Put current value on 'white' wire */
		_i_AD5300_Write_G(pTimepoint->ushQuadAvgYval, iCPE);
#else

		if (pTimepoint->ushRawYval > 255)pTimepoint->ushRawYval = 255;  
		if (pTimepoint->ushRawXval > 255)pTimepoint->ushRawXval = 255;


		_i_AD5300_Write_W(pTimepoint->ushRawXval, iCPE);

		/* Put current value on 'white' wire */
		_i_AD5300_Write_G(pTimepoint->ushRawYval, iCPE);
#endif /* (0) */

} /* int ProcessPoint( pTimepointType pTimepoint ) */
