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

/* stdout */
#include <stdio.h>

/* strlen() */
#include <string.h>

/* calloc() */
#include <stdlib.h>

/* struct timeval, gettimeofday() */
#include <sys/time.h>

/* sqrt() */
#include <math.h>

/* Data structure type definition */
#include "datastruct.h"

/* Errcode definitions */
#include "modsim.h"

#define max(x,y)	((x>y)?x:y)
#define min(x,y)	((x<y)?x:y)

/* Base of  ADxx converter */
#define CONV_BASE	256

/* Hundred multiplier to get decimal data for ADxx converter from volatge value */
#define CONV_CENTUM	100

/* Decimal power of fractal part */
#define FRACT_PWR	100000

/* Subraction of Decimal power and Centi */
#define FRACT_PWR_SB	1000

/* Time measurement variable to define begin of time scale */
struct timeval starttimePROC;

/* First time, will be subtracted in HAL */
float fFIRST;

/* Attach 3 floats to tail of dynamic structure 'pTimepointType' */
int _EnrollPoint(const char * caller, pTimepointType * ppThisPointChain, 
	float * pfltTm, float * pfltX, float * pfltY, 
	char * pcMrq)
{
pTimepointType pChild, pTempPointChain;

	if (NULL == *ppThisPointChain)
	{
		/* only one chain, for beginning */
		*ppThisPointChain = (pTimepointType) calloc ( 1, sizeof (TimepointType) );

		/* check if successful */
		if (NULL == *ppThisPointChain)
		{

			printf("[%s] %s:%s : ERROR: can't allocate memory for first element. %f: [X(%f),Y(%f)]  \n",
			__FILE__, caller, __func__,
			*pfltTm, *pfltX, *pfltY);

			return P_ERROR_MEM;
		}

		(*ppThisPointChain)->fltXval = *pfltX;
		(*ppThisPointChain)->fltYval = *pfltY;
		(*ppThisPointChain)->fltAbsTime = *pfltTm;

		/* Remove parasitic values (mostly they're same by modul, and different by sign) */
		if ( (*ppThisPointChain)->fltXval < 0.0) (*ppThisPointChain)->fltXval *= (-1);
		if ( (*ppThisPointChain)->fltYval < 0.0) (*ppThisPointChain)->fltYval *= (-1);

		/* Making unsigned short (needed fror ADxx conv'r) form float */
		(*ppThisPointChain)->ushRawXval = (*ppThisPointChain)->fltXval * CONV_CENTUM;
		(*ppThisPointChain)->ushRawYval = (*ppThisPointChain)->fltYval * CONV_CENTUM;


		/* Don't let the values to be higher than highest value allowed for converter */
		(*ppThisPointChain)->ushRawXval = min(CONV_BASE, (*ppThisPointChain)->ushRawXval);
		(*ppThisPointChain)->ushRawYval = min(CONV_BASE, (*ppThisPointChain)->ushRawYval);

		(*ppThisPointChain)->pcMarquee = calloc (1, strlen (pcMrq) +1 );
		strcpy( (*ppThisPointChain)->pcMarquee, pcMrq);

		// TODO: rework
		fFIRST = (*ppThisPointChain)->fltAbsTime;


#if defined(DEBUG_DATA)

		printf("[%s] %s:%s : FIRST <%f> <%f>[%d] <%f>[%d] <%s> \n", __FILE__, caller, __func__,
			(*ppThisPointChain)->fltAbsTime,
			(*ppThisPointChain)->fltXval, (*ppThisPointChain)->ushRawXval,
			(*ppThisPointChain)->fltYval, (*ppThisPointChain)->ushRawYval,
			(*ppThisPointChain)->pcMarquee
		);
#endif /* (DEBUG_DATA) */

		/* No first el't */
		(*ppThisPointChain)->pPrev = NULL;
	}
	else
	{
		/* point with first temporary element to head of chain */
		pChild = *ppThisPointChain;

		pTempPointChain = (pTimepointType) calloc (1, sizeof (TimepointType) );

		if (NULL == pTempPointChain)
		{

			printf("[%s] %s:%s : ERROR: can't allocate memory for next element. %f: [X(%f),Y(%f)]  \n", 
			__FILE__, caller, __func__,
			*pfltTm, *pfltX, *pfltY);

			return P_ERROR_MEM;
		}


		pTempPointChain->fltXval = *pfltX;
		pTempPointChain->fltYval = *pfltY;
		pTempPointChain->fltAbsTime = *pfltTm;

		/* Remove parasitic values (mostly they're same by modul, and different by sign) */
		if ( pTempPointChain->fltXval < 0.0) pTempPointChain->fltXval *= (-1);
		if ( pTempPointChain->fltYval < 0.0) pTempPointChain->fltYval *= (-1);

		/* Making unsigned short (needed fror ADxx conv'r) form float */
		pTempPointChain->ushRawXval = pTempPointChain->fltXval * CONV_CENTUM;
		pTempPointChain->ushRawYval = pTempPointChain->fltYval * CONV_CENTUM;

		/* Don't let the values to be higher than highest value allowed for converter */
		pTempPointChain->ushRawXval = min(CONV_BASE, pTempPointChain->ushRawXval);
		pTempPointChain->ushRawYval = min(CONV_BASE, pTempPointChain->ushRawYval);

		pTempPointChain->pcMarquee = calloc (1, strlen (pcMrq) +1 );
		strcpy( pTempPointChain->pcMarquee, pcMrq);


#if defined(DEBUG_DATA)
		printf("[%s] %s:%s : NEXT <%f> <%f>[%d] <%f>[%d] <%s> \n", __FILE__, caller, __func__,
			pTempPointChain->fltAbsTime,
			pTempPointChain->fltXval, pTempPointChain->ushRawXval,
			pTempPointChain->fltYval, pTempPointChain->ushRawYval,
			pTempPointChain->pcMarquee
		);
#endif /* (DEBUG_DATA) */

		/* Skip everything, except last entry */
		while ( (NULL != pChild) && (NULL != pChild->pNext ) )
		{
			/* . . */
			pChild = pChild->pNext;
		}

		/* Point onto previous el't */
		pTempPointChain->pPrev = pChild;

		/* Next chunk was created allright (we know it at this moment), so we attach a new chain entry to the end of existing chain */
		pChild->pNext = pTempPointChain;

	}

	return P_SUCCESS;

} /* int _EnrollPoint( ... ) */


/* Process data stored in dynamic structure pointed by 'pPointChainPar' */
int _ProcessPoints(const char * caller, pTimepointType pPointChainPar)
{
pTimepointType pPointChain = pPointChainPar;
pTimepointType pPointChainRESERV__2DEL = pPointChainPar;// TODO: _2_DEL . TESTING REALDATA CURVES on OSCILL. remove!
float fltAbsTime;

double timeusePROC;

	/* Take initial time. Current time values will be taken in 'ProcRealAndRel()' */
	gettimeofday(&starttimePROC, 0);

while(1)// TODO: _2_DEL . TESTING REALDATA CURVES on OSCILL. remove!
{pPointChain =  pPointChainRESERV__2DEL;// TODO: _2_DEL . TESTING REALDATA CURVES on OSCILL. remove!

	/* Process each entry of chain */
	while (NULL != pPointChain)
	{
#if DEBUG_DATA_
		printf("[%s] %s:%s : <%f> <%f> <%f> <%s> \n", __FILE__, caller, __func__,
			pPointChain->fltAbsTime,
			pPointChain->fltXval,
			pPointChain->fltYval,
			pPointChain->pcMarquee
		);
#endif /* (DEBUG_DATA) */

		unsigned short ushPrevXval, ushPrevYval, ushNextXval, ushNextYval;
		
		/* Pick up previous X,Y (e.g. 'white','green') value into <ushPrevXval>,<ushPrevYval> */  
		if (NULL != pPointChain->pPrev)
		{
			ushPrevXval = pPointChain->pPrev->ushRawXval;

			ushPrevYval = pPointChain->pPrev->ushRawYval;
		}
		/* use current one if impossible to get previous one */
		else
		{
			ushPrevXval = pPointChain->ushRawXval;

			ushPrevYval = pPointChain->ushRawYval;
		}

		/* Pick up next X,Y (e.g. 'white','green') value into <ushPrevXval>,<ushPrevYval>  */ 
		if (NULL != pPointChain->pNext)
		{
			ushNextXval = pPointChain->pNext->ushRawXval;

			ushNextYval = pPointChain->pNext->ushRawYval;
		}
		/* use current one if impossible to get next one */
		else
		{
			ushNextXval = pPointChain->ushRawXval;

			ushNextYval = pPointChain->ushRawYval;
		}

		/* Compute the average quadratic X as QUAD_AVG=SQRT(Prev*Prev + Curr*Curr + Next*Next )*/
		pPointChain->ushQuadAvgXval = sqrt(ushPrevXval*ushPrevXval +
			pPointChain->ushRawXval*pPointChain->ushRawXval +
			ushNextXval*ushNextXval);

		/* Compute the average quadratic Y as QUAD_AVG=SQRT(Prev*Prev + Curr*Curr + Next*Next )*/
		pPointChain->ushQuadAvgYval = sqrt(ushPrevYval*ushPrevYval +
			pPointChain->ushRawYval*pPointChain->ushRawYval +
			ushNextYval*ushNextYval);

		/* Realtime and relative-time values */
		ProcessPoint(pPointChain);

		/* Go to next record of chain */
		pPointChain = pPointChain->pNext;
	}

}// TODO: _2_DEL . TESTING REALDATA CURVES on OSCILL. remove!

	return P_SUCCESS;

} /* int _ProcessPoints(const char * caller, pTimepointType pPointChainPar) */

/* Free memory occupied by '*ppThisPointChain' */
void _DeletePoints(const char * caller, pTimepointType * ppThisPointChain)
{
pTimepointType pChild, pThisPointChain = *ppThisPointChain;

	/* Walk through entire list and delete each chain */
	while (NULL != pThisPointChain)
	{
		/* if space to keep item's name is allocated */
		if (pThisPointChain->pcMarquee)
		
		    /* then release this space */
		    free(pThisPointChain->pcMarquee);

		/* preserve a pointer to next record */		    
		pChild = pThisPointChain->pNext;
		
		/* free space occupied by current record */
		free (pThisPointChain);
		
		/* Go to next record */
		pThisPointChain = pChild;
	}

	/* Dispose first element of chain */
	*ppThisPointChain = NULL;

} /* void _DeletePoints(const char * caller, pTimepointType * ppThisPointChain) */
