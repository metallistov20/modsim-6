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

/* printf() */
#include <stdio.h>

/* struct timeval, gettimeofday() */
#include <sys/time.h>

/* Basic project definitions  */
#include "modsim.h"

/* Type 'pTimepointType' definition */
#include "datastruct.h"

/* Time measurement variable to define current position on time scale */
struct timeval endtimePROC;

/* Var. defined in 'datastruct.c' */
extern struct timeval starttimePROC;

/* Process Realtime and Relative-time values: certain data to be oputput onto Port D */
int ProcessPoint(pTimepointType pTimepoint)
{
#if !defined(QUASIFLOAT) 
float fltRelTime;
float _left, _right;
float fltJiffy = 1.0;
#else
QuasiFloatType fltRelTime;
QuasiFloatType _left, _right;
int fltJiffy = 1;
#endif /* defined(QUASIFLOAT)  */

	/* TODO: add plain description here  */

#if !defined(QUASIFLOAT) 
	/* Operate uSeconds multiplied by 10e6 because <usleep> accepts	integer parameters only */
	pTimepoint->fltAbsTime = pTimepoint->fltAbsTime*1000000;

	printf("[%s] : <BEFORE TIME SHIFTING> real tm.: %f\n", __FILE__, pTimepoint->fltAbsTime	);

	/* Don't proceed with this fuction once given an unappicable input data */
	if (0.0 == pTimepoint->fltAbsTime ) return;

	do 
	{	/* Take current time */
		gettimeofday(&endtimePROC,0);

		/* Compute how much time elapsed since head of list processing till now */
		fltRelTime = 1000000*(endtimePROC.tv_sec - starttimePROC.tv_sec - fFIRST) 

			+ endtimePROC.tv_usec - starttimePROC.tv_usec;

		/* If relative time stays on the left from 0 */
		if (fltRelTime < 0)

			/* then '_right' is relative time, '_left' is real time */
			_right = fltRelTime, _left = pTimepoint->fltAbsTime;

		/* If relative time stays on the right from 0, or is exctly 0 */
		else
			/* then '_right' is real time, '_left' is relative time */
			_left = fltRelTime, _right = pTimepoint->fltAbsTime;


		/* Wait for relative time <fltRelTime> to catch up with real time <pTimepoint->fltAbsTime>  */
		usleep (fltJiffy);

		printf("[%s] : <TIME SHIFTING> real tm.: %f, shiftable tm.: %f \n", __FILE__, pTimepoint->fltAbsTime,	fltRelTime ); 

	} while (_right < _left);
	
	/* Now they're equal or least 'relative tm' is not less than 'real tm' */
	printf("[%s] : <AFTER TIME SHIFTING> will pretend like <%f>, is same as <%f> \n", __FILE__,

		pTimepoint->fltAbsTime,	fltRelTime );

	/* PC-based simulation of hardware Port 'D' processing */
	none

#else

#endif /* defined(QUASIFLOAT)  */


} /* int ProcessPoint(pTimepointType pTimepoint) */
