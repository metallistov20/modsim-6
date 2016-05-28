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

#include <stdio.h>

#if defined(UCSIMM)

/* Platform definitions */
#include <asm/MC68EZ328.h>

#include <sys/bitypes.h>

#endif /* (UCSIMM) */

/* Own interface, definitions */
#include "port_d.h"

/* iOperatoin */
#include "modsim.h"


/* Set Port D into default state */
void PortD_Reset()
{
	PDSEL &= ~(NIX_W | NIX_G | SYNC_PIN_G | SCLK_PIN_G | MOSI_PIN_G | SYNC_PIN_W | SCLK_PIN_W | MOSI_PIN_W);
	PDDIR &= ~(NIX_W | NIX_G | SYNC_PIN_G | SCLK_PIN_G | MOSI_PIN_G | SYNC_PIN_W | SCLK_PIN_W | MOSI_PIN_W);  

} /* void PortD_Reset() */

/* Prepare Port's D IOs defined by bitmask 'uchBit' as outputs */
void PortD_Prepare(unsigned char uchBitMask)
{
	PDSEL = uchBitMask;

	PDDIR = uchBitMask;

	printf ("Bits <%08bb> of Port D initialized as OUT\n", uchBitMask );

} /* void PortD_Prepare(unsigned char uchBitMask) */

/* Toggle bits defined by bitmask 'uchBit' in Port D */
void PortD_Toggle(unsigned char uchBit)
{
	PDDATA ^= uchBit;

} /* void PortD_Toggle(unsigned char uchBit) */

/* Switch off bits defined by bitmask 'uchBit' in Port D */
void PortD_Down(unsigned char uchBit)
{
	PDDATA &= ~uchBit;

} /* void PortD_Down(unsigned char uchBit) */

/* Switch on bits defined by bitmask 'uchBit' in Port D */
void PortD_Up(unsigned char uchBit)
{
	PDDATA |= uchBit;

} /* void PortD_Up(unsigned char uchBit) */

void AD5300_Write_W(unsigned char data) 
{
unsigned short tmp;

unsigned char iCnt;

	tmp = data << AD5300_DONTCARE_LEN;

	AD5300_ACT_W;

	for (iCnt = 0; iCnt < AD5300_DATA_LEN; iCnt++)
	{
		SCLK_HI_W;

		(tmp & (unsigned short)( 1U << (15 - iCnt) ) ) ? (MOSI_HI_W) : (MOSI_LO_W);

		SCLK_LO_W;
	}

	AD5300_DEACT_W;

} /* void AD5300_Write_W(unsigned short data) */

void AD5300_Write_G(unsigned char data) 
{
unsigned short tmp;

unsigned char iCnt;

	tmp = data << AD5300_DONTCARE_LEN;

	AD5300_ACT_G;

	for (iCnt = 0; iCnt < AD5300_DATA_LEN; iCnt++)
	{
		SCLK_HI_G;

		(tmp & (unsigned short)( 1U << (15 - iCnt) ) ) ? (MOSI_HI_G) : (MOSI_LO_G);

		SCLK_LO_G;
	}

	AD5300_DEACT_G;

} /* void AD5300_Write_W(unsigned short data) */

/* Initialize 'white' converter */
void AD5300_Init_W(void)
{
	/* Prepare 'white'  bits on Port D leaving intact settings done earlier */
	PDSEL |= SYNC_PIN_W | SCLK_PIN_W | MOSI_PIN_W;
	PDDIR |= SYNC_PIN_W | SCLK_PIN_W | MOSI_PIN_W;

	/* Prepare 'white' controler AD53xx */
	AD5300_DEACT_W;

	SCLK_LO_W;
	MOSI_LO_W;

} /* void AD5300_Init_W(void) */

/* Initialize 'green' converter */
void AD5300_Init_G(void)
{
	/* Prepare 'green' bits on Port D leaving intact settings done earlier */
	PDSEL |= SYNC_PIN_G | SCLK_PIN_G | MOSI_PIN_G;
	PDDIR |= SYNC_PIN_G | SCLK_PIN_G | MOSI_PIN_G;

	/* Prepare 'white' controler AD53xx */
	AD5300_DEACT_G;

	SCLK_LO_G;
	MOSI_LO_G;

} /* void AD5300_Init_G(void) */


/* Initialize Port 'D' and, once needed, converter arrached to it */
void PeriphInit(void)
{
	switch (iOperation)
	{
		case DO_GATE0_OP:
			/* Set digital PIOs 1-4 as outputs */
			PortD_Prepare( NIX_W | NIX_G );
			break;

		case DO_GATE1_OP:
			/* Besides all, it prepares PIOs, so theres no need to do <PortD_Prepare()> */
			AD5300_Init_W();
			AD5300_Init_G();
			break;

		default:
			printf("__s: bad kind of operatoin (while PERIPH. INIT), restart the program\n");
			exit (-1);
	}
} /* void PeriphInit(void) */
