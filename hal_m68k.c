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

/* Type 'pTimepointType' definition, <iFIRST> integer */
#include "datastruct.h"

/* PortD_Up(), PortD_Down(), PD0, PD1 */
#include "port_d.h"

/* Time measurement variable to define current position on time scale */
struct timeval endtimePROC;

/* Var. defined in 'datastruct.c' */
extern struct timeval starttimePROC;

/* Var. to check if current second already was 'displayed' */
int iOldSecPRC;

int ProcessPoint( pTimepointType pTimepoint )
{

QuasiFloatType qfltRelTime;
int _left, _right;
QuasiFloatType qfltJiffy; 
qfltJiffy.fraction = 1;

#if 1

	if (iOldSecPRC!= pTimepoint->qfltAbsTime.integer)
		{iOldSecPRC=pTimepoint->qfltAbsTime.integer; printf("secPRC: %d; ", iOldSecPRC); fflush(stdout); }

	pTimepoint->qfltAbsTime.integer = (pTimepoint->qfltAbsTime.integer < 0)?
		(pTimepoint->qfltAbsTime.integer * 1000000) - (pTimepoint->qfltAbsTime.fraction):
		(pTimepoint->qfltAbsTime.integer * 1000000) + (pTimepoint->qfltAbsTime.fraction);

	/* Take current time */
	gettimeofday(&endtimePROC,0);

	/* Compute time elapsed since head of list processing till now */
	qfltRelTime.integer = 1000000*(endtimePROC.tv_sec - starttimePROC.tv_sec) 
		+ endtimePROC.tv_usec - starttimePROC.tv_usec - iFIRST;

#if defined(FAST_UCSIMM)
	printf("[%s] : <BEFORE TIME SHIFTING> real tm.: %d, shiftable tm.: %d \n", __FILE__,
		pTimepoint->qfltAbsTime.integer,	qfltRelTime.integer );
#else
#endif /* defined(FAST_UCSIMM) */

	/* If relative time stays on the left from 0 */
	if (qfltRelTime.integer < 0)

		/* then '_right' is relative time, '_left' is real time */
		_right = qfltRelTime.integer, _left = pTimepoint->qfltAbsTime.integer;

	/* If relative time stays on the right from 0, or is exctly 0 */
	else
		/* then '_right' is real time, '_left' is relative time */
		_left = qfltRelTime.integer, _right = pTimepoint->qfltAbsTime.integer;

	if (0 == pTimepoint->qfltAbsTime.integer) return;

	/* TODO: make <do-while> instead of <while-do>, thus avoid a code duplication */
	while (_right < _left )
	{
		/* Wait for relative <fltRelTime> to catch up with absolute <fltRealTime>  */
		usleep (qfltJiffy.fraction);

		/* Take current time */
		gettimeofday(&endtimePROC,0);

		/* Compute how much time elapsed since head of list processing till now */
		qfltRelTime.integer = 1000000*(endtimePROC.tv_sec - starttimePROC.tv_sec) 

			+ endtimePROC.tv_usec - starttimePROC.tv_usec  - iFIRST;

		/* If relative time stays on the left from 0 */
		if (qfltRelTime.integer < 0)

			/* then '_right' is relative time, '_left' is real time */
			_right = qfltRelTime.integer, _left = pTimepoint->qfltAbsTime.integer;

		/* If relative time stays on the right from 0, or is exctly 0 */
		else

			/* then '_right' is real time, '_left' is relative time */
			_left = qfltRelTime.integer, _right = pTimepoint->qfltAbsTime.integer;

#if defined(FAST_UCSIMM)
		printf("[%s] : <TIME SHIFTING> real tm.: %d, shiftable tm.: %d \n", __FILE__,
		pTimepoint->qfltAbsTime.integer,	qfltRelTime.integer );
#else
#endif /* defined(FAST_UCSIMM) */
	}

#if defined(FAST_UCSIMM)
	/* Now they're equal or least 'relative tm' is not less than 'real tm' */
	printf("[%s] : <AFTER TIME SHIFTING> will pretend like <%d>, is same as <%d> \n", __FILE__,
		pTimepoint->qfltAbsTime.integer,
		qfltRelTime.integer );
#else
#endif /* defined(FAST_UCSIMM) */

#endif /* (0) */


		/* Put current value on 'green' wire */
		AD5300_Write_W(pTimepoint->ushQuadAvgXval);

		/* Put current value on 'white' wire */
		AD5300_Write_G(pTimepoint->ushQuadAvgYval);


} /* int ProcessPoint( pTimepointType pTimepoint ) */
