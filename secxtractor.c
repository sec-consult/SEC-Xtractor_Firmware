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
	uartWriteString(DUMP_BEGIN_MARKER);
}

const char* hexChars = "0123456789ABCDEF";

static inline void dumpByte(uint8_t b)
{
	if(dumpFast){
		if(dumpPosition % 1024 == 0){ // write 1K bytes per line
			uartWriteString(NL);
		}
	}
	else
	{
		if (dumpPosition % 4 == 0)
		{
			uartWriteChar(' ');
		}
		if(dumpPosition != 0){
			if (dumpPosition % 16 == 0)
			{
				uartWriteString(NL " ");
			}
		}
	}
	uartWriteChar(hexChars[b >> 4]);
	uartWriteChar(hexChars[b & 0xF]);
	dumpPosition++;
}

static inline void dumpEnd()
{
	uartWriteString(NL DUMP_END_MARKER);
}

static inline int isOperationCanceled()
{
	if(cancelCurrentOperation)
	{
		return 1;
	}

	if(uartHasInput() && uartGetInput() == CHAR_BREAK){
		cancelCurrentOperation = 1;
		uartWriteString(NL OUTPUT_BOUNDARY_THICK "Cancelling..." NL);
		return 1;
	}
	return 0;
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
