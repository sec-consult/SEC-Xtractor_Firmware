/**
 * @file secxtractor.c
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
#include "hal.h"
#include "shell.h"

uint64_t dumpPosition;

static inline void dumpStart()
{
	dumpPosition = 0;
	uartWriteString("Begin Dump:" NL " ");
	uartWriteString(OUTPUT_BOUNDARY_THICK);
}

static inline void dumpByte(uint8_t b)
{
	if(dumpPosition != 0){
		if (dumpPosition % 4 == 0)
		{
			uartWriteChar(' ');
		}
		
		if (dumpPosition % 16 == 0)
		{
			uartWriteString(NL " ");
		}
	}
	uartprintf("%02X", b);
	dumpPosition++;
}

static inline void dumpEnd()
{
	uartWriteString(NL OUTPUT_BOUNDARY_THICK NL);
}

/**
 * @brief Main microcontroller loop. This is the main entry point of the SEC Xtractor firmware.
 */
int main(void)
{
	DELAY_MS(100); //wait for possible voltage glitches
	//disable JTAG to make PORTB 4-7 usable
	disableJTAG();
	initSystemClock();
	uartInit();
	activateInterrupts();
	shellInit();
	sevensegInit();
	initADC();
	initOnfiParam();
	while (1)
	{
		if (uartHasInput())
		{
			shell(uartGetInput());
		}
		sevensegWriteNumber(readVoltage());
	}
}
