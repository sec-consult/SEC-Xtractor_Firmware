/**
 * @file jtag.c
 * 
 * Copyright (C): SEC Consult Unternehmensberatung GmbH, 2019         \n
 * Web:           https://sec-consult.com/                            \n
 * Notice:        This code is based on the project JTAGenum from Nathan Andrew Fain\n
 *                It is available under https://github.com/cyphunk/JTAGenum\n
 * This Source Code Form is subject to the terms of the Mozilla Public\n
 * License, v. 2.0. If a copy of the MPL was not distributed with this\n
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.          \n
 * 
 * @authors Thomas Weber, Wolfgang Ettlinger
 */
#include "../function_modules/jtag.c"

uint8_t pinslen = 5;
uint8_t set_tck = 3;
uint8_t set_tms = 0;
uint8_t set_tdo = 2;
uint8_t set_tdi = 1;
uint8_t set_trst = 4;

uint32_t flashsize = 0;

static inline void cmdPatternSet(char input[64])
{
	for (int i = 0; i < 64; i++)
	{
		pattern[i] = input[i];
	}
	uartprintf("new pattern set [%s]" NL, pattern);
	return;
}


void cmdPatternScanSingle(char *arguments)
{
	uint8_t dummy;
	jtagInitPins(set_tck, set_tms, set_tdi, set_trst /*ntrst*/, pinslen);
	jtagTapState(TAP_SHIFTIR, set_tck, set_tms);
	if (check_data(pattern, (2 * PATTERN_LEN), set_tck, set_tdi, set_tdo, &dummy))
		uartWriteString(NL "found pattern or other" NL);
	else
		uartWriteString(NL "no pattern found" NL);
}

void cmdPinlenSet(char *arguments)
{
	pinslen = atoi(arguments);
	if (pinslen > 32)
	{
		uartWriteString(NL "Value too high. Max 32!" NL);
		uartWriteString(NL "PINSEL set to 32" NL);
		pinslen = 32;
	}
	else
	{
		uartWriteString(NL "Pinlen set!" NL);
	}
}

void cmdTckSet(char *arguments)
{
	set_tck = atoi(arguments);
	if (set_tck > 32)
	{
		uartWriteString("Value too high. Max 32!" NL);
		uartWriteString(NL "TCK set to 32" NL);
		set_tck = 32;
	}
}

void cmdTmsSet(char *arguments)
{
	set_tms = atoi(arguments);
	if (set_tms > 32)
	{
		uartWriteString(NL "Value too high. Max 32!" NL);
		uartWriteString(NL "TMS set to 32" NL);
		set_tms = 32;
	}
}

void cmdTdiSet(char *arguments)
{
	set_tdi = atoi(arguments);
	if (set_tdi > 32)
	{
		uartWriteString("Value too high. Max 32!" NL);
		uartWriteString("TDI set to 32" NL);
		set_tdi = 32;
	}
}

void cmdTdoSet(char *arguments)
{
	set_tdo = atoi(arguments);
	if (set_tdo > 32)
	{
		uartWriteString("Value too high. Max 32!" NL);
		uartWriteString("TDO set to 32" NL);
		set_tdo = 32;
	}
}

void cmdTrstSet(char *arguments)
{
	set_trst = atoi(arguments);
	if (set_trst > 32)
	{
		uartWriteString("Value too high. Max 32!" NL);
		uartWriteString("TRST set to 32" NL);
		set_trst = 32;
	}
}

void cmdBoundaryScan(char *arguments)
{
	uartWriteString("pins");
	jtagPrintPins(set_tck, set_tms, set_tdo, set_tdi, set_trst);
	uartWriteString(NL);
	sample(SCAN_LEN + 100, set_tck, set_tms, set_tdi, set_tdo, set_trst);
}

void cmdIrLenSet(char *arguments)
{
	uint8_t ir_len = atoi(arguments);
	if (ir_len > 40)
	{
		uartWriteString("Value too high. Max 40!" NL);
		uartWriteString("IR_LEN set to 40" NL);
		ir_len = 40;
	}
	jtagSetIRLen(ir_len);
}

void cmdIrEnum(char *arguments)
{
	brute_ir(SCAN_LEN, set_tck, set_tms, set_tdi, set_tdo, set_trst);
}


void cmdFlashsizeSet(char *arguments)
{
	flashsize = atoi(arguments);
	uartWriteString("Caution, size is unlimited!" NL);
}


void cmdDelayJtag(char *arguments)
{
	if (*arguments == '-')
	{
		uartWriteString("");
		if (jtagDelayTime != 0 && jtagDelayTime > 1000)
			jtagDelayTime -= 1000;
		else if (jtagDelayTime != 0 && jtagDelayTime <= 1000)
			jtagDelayTime -= 100;
		uartprintf("Delay microseconds: %ld" NL, jtagDelayTime);
	}
	else if (*arguments == '-')
	{
		if (jtagDelayTime < 1000)
			jtagDelayTime += 100;
		else
			jtagDelayTime += 1000;
		uartprintf("Delay microseconds: %ld" NL, jtagDelayTime);
	}
	else
	{
		jtagDelay = ~jtagDelay;
		if (jtagDelay)
		{
			uartWriteString("JTAG DELAY ON" NL);
		}
		else
		{
			uartWriteString("JTAG DELAY OFF" NL);
		}
	}
}


/*
 * Check for pins that pass pattern[] between tdi and tdo
 * regardless of JTAG TAP state (tms, tck ignored).
 *
 * TDO, TDI pairs that match indicate possible shorts between
 * pins. Pins that do not match but are active might indicate
 * that the patch cable used is not shielded well enough. Run
 * the test again without the cable connected between controller
 * and target. Run with the verbose flag to examine closely.
 */
static void cmdLoopbackCheck(char *arguments)
{
	int pins[2];

	uartWriteString(OUTPUT_BOUNDARY_THICK "Starting loopback check..." NL);

	bruteforce(pins, pinslen, 0, 2, bypassScanTest);

	uartWriteString(OUTPUT_BOUNDARY_THICK);
}


/*
 * Shift JTAG TAP to ShiftIR state. Send pattern to TDI and check
 * for output on TDO
 */
static void cmdPatternScan(char *arguments)
{
	int pins[5];
	
	uartprintf(NL OUTPUT_BOUNDARY "Starting scan for pattern: '%s'" NL, pattern);

	bruteforce(pins, pinslen, 0, 5, patternScanTest);

	uartWriteString(OUTPUT_BOUNDARY "End of JTAG brute force" NL);
}


/*
 * Scan TDO for IDCODE. Handle MAX_DEV_NR many devices.
 * We feed zeros into TDI and wait for the first 32 of them to come out at TDO (after n * 32 bit).
 * As IEEE 1149.1 requires bit 0 of an IDCODE to be a "1", we check this bit.
 * We record the first bit from the idcodes into bit0.
 * (oppposite to the old code).
 * If we get an IDCODE of all ones, we assume that the pins are wrong.
 */
static void cmdIdcodeScan(char *arguments)
{
	int pins[5];

	uartWriteString(NL OUTPUT_BOUNDARY_THICK "Starting scan for IDCODE..." NL);
	
	bruteforce(pins, pinslen, 0, 5, idcodeTest);

	uartWriteString(NL OUTPUT_BOUNDARY_THICK);
}

static void cmdBypassScan(char *arguments)
{
	int pins[3];

	uartWriteString(NL OUTPUT_BOUNDARY_THICK
					"Starting shift of pattern through bypass..." NL
					"Assumes bypass is the default DR on reset." NL
					"Hence, no need to check for TMS. Also, currently" NL
					"not checking for nTRST, which might not work" NL);
	
	bruteforce(pins, pinslen, 0, 3, bypassScanTest);

	uartWriteString(OUTPUT_BOUNDARY_THICK);
}

// static void jtagPrintPins(int tck, int tms, int tdo, int tdi, int ntrst)
// {
// 	uint8_t buffer[6];
// 	if (ntrst != IGNOREPIN)
// 	{
// 		uartWriteString("ntrst:\tA");
// 		snprintf(buffer, sizeof(buffer), "%d", ntrst);
// 		uartWriteString(buffer);
// 		uartWriteString("\n\r");
// 	}
// 	uartWriteString("tck:\tA");
// 	snprintf(buffer, sizeof(buffer), "%d", tck);
// 	uartWriteString(buffer);
// 	uartWriteString("\n\rtms:\tA");
// 	snprintf(buffer, sizeof(buffer), "%d", tms);
// 	uartWriteString(buffer);
// 	uartWriteString("\n\rtdo:\tA");
// 	snprintf(buffer, sizeof(buffer), "%d", tdo);
// 	uartWriteString(buffer);
// 	if (tdi != IGNOREPIN)
// 	{
// 		uartWriteString("\n\rtdi:\tA");
// 		snprintf(buffer, sizeof(buffer), "%d", tdi);
// 		uartWriteString(buffer);
// 		uartWriteString("\n\r");
// 	}
// }