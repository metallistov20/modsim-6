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

#ifndef _PORT_D_H_
#define _PORT_D_H_

/* Port 'D', bit 0, pin 30 */
#define PD0 0x80
/* Port 'D', bit 1, pin 29 */
#define PD1 0x40
/* Port 'D', bit 2, pin 28 */
#define PD2 0x20
/* Port 'D', bit 3, pin 27 */
#define PD3 0x10

/* Port 'D', bit 4, pin 26 */
#define PD4 0x08
/* Port 'D', bit 5, pin 25 */
#define PD5 0x04
/* Port 'D', bit 6, pin 24 */
#define PD6 0x02
/* Port 'D', bit 7, pin 23 */
#define PD7 0x01


/* Not connected to AD5300. 'White' wire of CPE#0. (D-, White) */
#define NIX_W			PD0
/* Not connected to AD5300. 'Green' wire of CPE#0. (D+, Green) */
#define NIX_G			PD1

/* ~Synchronization. Leg 5 of 'white' AD5300. (Orange) */
#define SYNC_PIN_W	 	PD2
/* Clocking. Leg 6 of 'white' AD5300. (Yellow) */
#define SCLK_PIN_W	 	PD3
/* Data output. Leg 7 of 'white' AD5300. (Green) */
#define MOSI_PIN_W	 	PD4

/* ~Synchronization. Leg 5 of 'green' AD5300. (Orange) */
#define SYNC_PIN_G	 	PD5
/* Clocking. Leg 6 of 'green' AD5300. (Yellow) */
#define SCLK_PIN_G	 	PD6
/* Data output. Leg 7 of 'freen' AD5300. (Green) */
#define MOSI_PIN_G	 	PD7


/* Toggle down SCKL on 'white' converter */
#define SCLK_LO_W		PortD_Down(SCLK_PIN_W)
/* Toggle up SCKL on 'white' converter */
#define SCLK_HI_W		PortD_Up(SCLK_PIN_W)
/* Toggle down MOSI on 'white' converter */
#define MOSI_LO_W		PortD_Down(MOSI_PIN_W)
/* Toggle up MOSI on 'white' converter */
#define MOSI_HI_W		PortD_Up(MOSI_PIN_W)
/* Activate 'white' converter */
#define AD5300_ACT_W 		PortD_Down(SYNC_PIN_W)
/* Deactivate 'white' converter */
#define AD5300_DEACT_W 		PortD_Up(SYNC_PIN_W)

/* Toggle down SCKL on 'green' converter */
#define SCLK_LO_G		PortD_Down(SCLK_PIN_G)
/* Toggle up SCKL on 'green' converter */
#define SCLK_HI_G		PortD_Up(SCLK_PIN_G)
/* Toggle down MOSI on 'green' converter */
#define MOSI_LO_G		PortD_Down(MOSI_PIN_G)
/* Toggle up MOSI on 'green' converter */
#define MOSI_HI_G		PortD_Up(MOSI_PIN_G)
/* Activate 'green' converter */
#define AD5300_ACT_G 		PortD_Down(SYNC_PIN_G)
/* Deactivate 'green' converter */
#define AD5300_DEACT_G 		PortD_Up(SYNC_PIN_G)

/* Length of data word at AD53xx converter */
#define AD5300_DATA_LEN		16
/* Amount of bits (within data word) to be ignored while passing 8-bit data value to AD53xx converter */
#define AD5300_DONTCARE_LEN	4



/* RUNET: TTL levels "logical 0" 0.4V and "logical 1" 2.4V, more precise: less than 0.4 and more
than 2.4 correspondingly. Current on DIn, DOut shuld not exceet 3.6 V. */

/* WIKI: Low- and full-speed modes (USB 1.x) use a single data pair, labeled D+ and D−, in half-duplex.
Transmitted signal levels are 0.0–0.3 V for logical low, and 2.8–3.6 V for logical high level. The signal
lines are not terminated. */

// TODO: give a preference either to WIKI wither to RUNET. (see above.)

/* Anything under this voltage is regarded to be <logical 0> for USB 1.1 */
#define USB11_LOGIC_0_CURR		0.4	// TODO: not used, clean out
/* Integer part of <LOGIC_0_CURR> */
#define USB11_LOGIC_0_CURR_INTGR	0
/* Fraction  of <LOGIC_0_CURR> */
#define USB11_LOGIC_0_CURR_FRACT	40000

/* Anything above this voltage is regarded to be <logical 1> for USB 1.1 */
#define USB11_LOGIC_1_CURR		2.4	// TODO: not used, clean out
/* Integer part of <LOGIC_1_CURR> */
#define USB11_LOGIC_1_CURR_INTGR	2
/* Fraction  of <LOGIC_1_CURR> */
#define USB11_LOGIC_1_CURR_FRACT	40000


/* WIKI: high-speed mode (USB 2.0) uses the same wire pair, but with different electrical conventions. Lower
signal voltages of −10 – 10 mV for low and 360–440 mV for logical high level, and termination of 45 Ω to 
ground, or 90 Ω differential to match the data cable impedance. */


/* Anything from range −10 mV .. +10 mV is regarded to be <logical 0> for USB 2.0 */
#define USB20_LOGIC_0_LO_CURR	(-0.01)		// TODO: not used, clean out
#define USB20_LOGIC_0_UP_CURR	0.01 		// TODO: not used, clean out

/* Anything from range 360 mV .. 440 mV is regarded to be <logical 1> for USB 2.0 */
#define USB20_LOGIC_1_LO_CURR	0.36		// TODO: not used, clean out
#define USB20_LOGIC_1_UP_CURR	0.44		// TODO: not used, clean out

#define USB20_LOGIC_0_LO_CURR_INTGR	(-0)
#define USB20_LOGIC_0_LO_CURR_FRACT	1000
#define USB20_LOGIC_0_UP_CURR_INTGR	0
#define USB20_LOGIC_0_UP_CURR_FRACT	1000

#define USB20_LOGIC_1_LO_CURR_INTGR	0
#define USB20_LOGIC_1_LO_CURR_FRACT	36000
#define USB20_LOGIC_1_UP_CURR_INTGR	0
#define USB20_LOGIC_1_UP_CURR_FRACT	44000

#define UNPROC				"Not processed"

/* Set Port D into default state */
void PortD_Reset();

/* Prepared Port's D IOs defined by bitmask 'uchBit' as outputs. Exposed to main() [HW_PORTD_TEST] */
void PortD_Prepare(unsigned char uchBitMask);

/* Switch on bits defined by bitmask 'uchBit' in Port D. Exposed to main() [HW_PORTD_TEST] */
void PortD_Toggle(unsigned char uchBit);

/* Terminal line DOWN - switch off <dIN> wire on CPE#0, bzw CPE#1. Exposed to <ProcessPoint()> */
void Term_Up();

/* Terminal line DOWN - switch off <dIN> wire on CPE#0, bzw CPE#1. Exposed to <ProcessPoint()> */
void Term_Down();

/* Write 'data'::{0..255} to 'white'/'green' converter. Parameter '0' for 0V, '255' for VDD Volts. Exposed to main() [HW_AD53_TEST] */
void AD5300_Write_W(unsigned char data);
void AD5300_Write_G(unsigned char data);

/* Initialize 'white'/'green' converter . Exposed to main() [HW_AD53_TEST] */
void AD5300_Init_W(void);
void AD5300_Init_G(void);

/* Initialize Port 'D' and, once needed, converter arrached to it. Exposed to main() */
void PeriphInit(void);

#endif /* _PORT_D_H_ */
