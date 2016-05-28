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

#ifndef _DATASTUCT_H_
#define _DATASTUCT_H_


#if defined(UCSIMM)

/* Old cross compiler does not operate such ANSI macro as __func__ */
#define __func__ "_func_"

#endif /* defined(UCSIMM) */


#if defined(QUASIFLOAT) 
typedef struct _QuasiFloatType
{
	/* Integer part of quasi-float */
	int integer;

	/* Fractal part of quasi-float */
	int fraction;

}  QuasiFloatType, *pQuasiFloatType;
#endif /* defined(QUASIFLOAT) */


/* List of D+(d.IN) and D-(d.OUT) values with relative time points */
typedef struct _TimepointType
{
	/* String to descibe this tm. point */
	char * pcMarquee;

	/* 0 - 256 for ADxx for 'white' */
	unsigned short ushRawXval;

	/* 0 - 256 for ADxx for 'green' */
	unsigned short ushRawYval;

	/* Average qudratic for 'white' */
	unsigned short ushQuadAvgXval;

	/* Average qudratic for 'green' */
	unsigned short ushQuadAvgYval;	

#if !defined(QUASIFLOAT) 
	/* D- , yellow pin */
	float fltXval;

	/* D+ , blue pin */
	float fltYval;

	/* CVS's time stamp */
	float fltAbsTime;
#else
	/* D- , green wire of USB wire quartet */
	QuasiFloatType qfltXval;

	/* D+ , white wire of USB wire quartet */
	QuasiFloatType qfltYval;

	/* CSV's time stamp */
	QuasiFloatType qfltAbsTime;
#endif /* !defined(QUASIFLOAT) */

	/* Next time point in the chain */
	struct _TimepointType * pNext;

	/* Previous time point */
	struct _TimepointType * pPrev;

} TimepointType, *pTimepointType;

int _EnrollPoint(const char * caller, pTimepointType * ppThisPointChain, 
#if !defined(QUASIFLOAT) 
	float * pfltTm, float * pfltX, float * pfltY, 
#else
	pQuasiFloatType pqfltTm, pQuasiFloatType pqfltX, pQuasiFloatType pqfltY, 
#endif /* !defined(QUASIFLOAT) */
	char * pcMrq);

/* Exposed to main() via macro */
int _EnrollPoint(const char * caller, pTimepointType * ppThisPointChain, 
#if !defined(QUASIFLOAT) 
	float * pfltTm, float * pfltX, float * pfltY, 
#else
	pQuasiFloatType pqfltTm, pQuasiFloatType pqfltX, pQuasiFloatType pqfltY, 
#endif /* !defined(QUASIFLOAT) */
char * pcMrq);

/* Exposed to main() via macro  */
int _ProcessPoints(const char * caller, pTimepointType pPointChainPar);

/* Exposed to main() via macro */
void _DeletePoints(const char * caller, pTimepointType * ppThisPointChain);

#define EnrollPoint(x, y, s, t, u) _EnrollPoint(__func__, (x), (y), (s), (t), (u))

#define ProcessPoints(x) _ProcessPoints(__func__, (x))

#define DeletePoints(x) _DeletePoints(__func__, (x))

/* First time, exposed to ProcessPoint() */
#if !defined(QUASIFLOAT) 
	float fFIRST;
#else
	int iFIRST;
#endif /* defined(QUASIFLOAT)  */


#endif /* _DATASTUCT_H_ */
