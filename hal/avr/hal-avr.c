/**
 * @file hal-avr.c
 * 
 * Copyright (C): SEC Consult Unternehmensberatung GmbH, 2019         \n
 * Web:           https://sec-consult.com/                            \n
 *                                                                    \n
 * This Source Code Form is subject to the terms of the Mozilla Public\n
 * License, v. 2.0. If a copy of the MPL was not distributed with this\n
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.          \n
 * 
 * @authors Thomas Weber, Wolfgang Ettlinger, Steffen Robertz
 */
#include "../../secxtractor.h"
#include "../../hal.h"
#include <avr/io.h>
/* 4 Megabit UART */
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stddef.h>

#include "adc.c"
#include "jtag.c"
#include "nand.c"
#include "nor.c"
#include "sevenseg.c"
//#include "spi.c"
#include "uart.c"
//#include "uart_scanner.c"
//#include "timer.c"

static inline void portOut(xtractor_port_t *port, uint8_t b)
{
	port->OUT = b;
}

static inline void portSetDirection(xtractor_port_t *port, uint8_t mask)
{
	port->DIR = mask;
}

static inline void activateInterrupts()
{
	/* activate interrupt level "high" only */
	PMIC.CTRL |= PMIC_HILVLEN_bm;
	sei();
}

/**
 * @brief Function to disable the JTAG port. This prevents the JTAG brute forcer from detecting itself.
 */
static inline void disableJTAG()
{
	CCP = CCP_IOREG_gc;
	MCU.MCUCR = MCU_JTAGD_bm;
}

/**
 * @brief Clock initialization for 32MHz internal oscillator.
 */
static inline void initSystemClock(void)
{
	OSC.CTRL |= OSC_RC32MEN_bm;
	while (!(OSC.STATUS & OSC_RC32MRDY_bm));
	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;


	// OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc ;
	// OSC.CTRL |= OSC_XOSCEN_bm ; // enable it
	// while((OSC.STATUS & OSC_XOSCRDY_bm) == 0){} // wait until it's stable
	// OSC.PLLCTRL = OSC_PLLSRC_XOSC_gc | 2 ;
	// OSC.CTRL |= OSC_PLLEN_bm ; // enable the PLL...
	// while( (OSC.STATUS & OSC_PLLRDY_bm) == 0 ){} // wait until it's stable
	// CCP = CCP_IOREG_gc;	// protected write follows   
	// CLK.CTRL = CLK_SCLKSEL_PLL_gc;	// The System clock is now  PLL (16Mhz * 2)

	// OSC.XOSCCTRL = OSC_FRQRANGE_12TO16_gc | OSC_XOSCSEL_XTAL_16KCLK_gc;
	// OSC.CTRL |= OSC_XOSCEN_bm;
	// while (!(OSC.STATUS & OSC_XOSCRDY_bm));
	// CCP = CCP_IOREG_gc;
	// CLK.CTRL = CLK_SCLKSEL_XOSC_gc;
	// OSC.PLLCTRL = OSC_PLLSRC_XOSC_gc | 2;
	// OSC.CTRL |= OSC_PLLEN_bm;
	// while (!(OSC.STATUS & OSC_PLLRDY_bm));
	// CCP = CCP_IOREG_gc;
	// CLK.CTRL = CLK_SCLKSEL_PLL_gc;
	// OSC.CTRL &= ~OSC_RC32MEN_bm;

}

static inline void resetDevice(void)
{
	cli();

	CCP = CCP_IOREG_gc;
	RST.CTRL = RST_SWRST_bm;
}
