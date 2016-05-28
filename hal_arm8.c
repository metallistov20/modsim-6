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
int iCPE = 0;

		/* Parsing-out redundant data from CVS-datafile yet to be implemented */
		none

		/* Put current value on 'green' wire */
		_i_AD5300_Write_W(pTimepoint->ushQuadAvgYval /*pTimepoint->ushQuadAvgXval*/, iCPE);

		/* Put current value on 'white' wire */
		_i_AD5300_Write_G(pTimepoint->ushQuadAvgYval, iCPE);

} /* int ProcessPoint( pTimepointType pTimepoint ) */
