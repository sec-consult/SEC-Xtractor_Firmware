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
 * @authors Thomas Weber, Wolfgang Ettlinger, Steffen Robertz
 */

//uint8_t pinslen = 5;
//uint8_t set_tck = 3;
//uint8_t set_tms = 0;
//uint8_t set_tdo = 2;
//uint8_t set_tdi = 1;
//uint8_t set_trst = 4;

//uint32_t flashsize = 0;

uint8_t set_ir_len = 8;

/*****  JTAG Functionality from JTAGenum *****/
/*
 * Set the JTAG TAP state machine
 */
void jtagTapState(char jtagTapState[], int tck, int tms)
{
	while (*jtagTapState)
	{ // exit when string \0 terminator encountered
		if (jtagDelay)
			_delay_us(50);
		jtagWritePin(tck, 0);
		jtagWritePin(tms, *jtagTapState - '0'); // conv from ascii pattern
		jtagWritePin(tck, 1);				 // rising edge shifts in TMS
		*jtagTapState++;
	}
}

void jtagSetIRLen(uint8_t ir_len)
{
	set_ir_len = ir_len;
}

void jtagInitPins(int tck, int tms, int tdi, int ntrst, uint8_t pinslen)
{
	// default all to INPUT state
	for (int i = 0; i < pinslen; i++)
	{
		setPinMode(i, 0);
		// internal pullups default to logic 1:
		//		if (PULLUP) jtagWritePin(i, 1);
	}
	// TCK = output
	if (tck != IGNOREPIN)
		setPinMode(tck, 1);
	// TMS = output
	if (tms != IGNOREPIN)
		setPinMode(tms, 1);
	// tdi = output
	if (tdi != IGNOREPIN)
		setPinMode(tdi, 1);
	// tdo = input
	if (tdi != IGNOREPIN)
		setPinMode(tdi, 0);
	// ntrst = output, fixed to 1
	if (ntrst != IGNOREPIN)
	{
		setPinMode(ntrst, 1);
		jtagWritePin(ntrst, 1);
	}
}

static void jtagPrintPins(int tck, int tms, int tdo, int tdi, int ntrst)
{
	uint8_t buffer[6];
	if (ntrst != IGNOREPIN)
	{
		uartWriteString("ntrst:\tA");
		snprintf(buffer, sizeof(buffer), "%d", ntrst);
		uartWriteString(buffer);
		uartWriteString("\n\r");
	}
	uartWriteString("tck:\tA");
	snprintf(buffer, sizeof(buffer), "%d", tck);
	uartWriteString(buffer);
	uartWriteString("\n\rtms:\tA");
	snprintf(buffer, sizeof(buffer), "%d", tms);
	uartWriteString(buffer);
	uartWriteString("\n\rtdo:\tA");
	snprintf(buffer, sizeof(buffer), "%d", tdo);
	uartWriteString(buffer);
	if (tdi != IGNOREPIN)
	{
		uartWriteString("\n\rtdi:\tA");
		snprintf(buffer, sizeof(buffer), "%d", tdi);
		uartWriteString(buffer);
		uartWriteString("\n\r");
	}
}

static void jtagPulseTms(int tck, int tms, int s_tms)
{
	jtagWritePin(tck, 0);
	jtagWritePin(tms, s_tms);
	jtagWritePin(tck, 1);
}

static void jtagPulseTdi(int tck, int tdi, int s_tdi)
{
	if (jtagDelay)
		_delay_us(50);
	if (tck != IGNOREPIN)
		jtagWritePin(tck, 0);
	jtagWritePin(tdi, s_tdi);
	if (tck != IGNOREPIN)
		jtagWritePin(tck, 1);
}

uint8_t jtagPulseTdo(int tck, int tdo)
{
	uint8_t tdo_read;
	if (jtagDelay)
		_delay_us(50);
	jtagWritePin(tck, 0); // read in TDO on falling edge
	tdo_read = jtagReadPin(tdo);
	jtagWritePin(tck, 1);
	return tdo_read;
}

static void bruteforce(int variables[], int count, int index, int num_variables, void(*action)(int[], uint8_t))
{
	int j;
	int *i = &variables[index];
	int found;

	for(*i=0; *i<count; (*i)++)
	{
		found = 0;
		for(j=0; j<index; j++)
		{
			// check if the value of *i is already used
			if(*i == variables[j]){
				found = 1;
				break;
			}
		}

		if(!found)
		{
			if(index == num_variables - 1)
			{
				action(variables, count);
			}
			else
			{
				bruteforce(variables, count, index + 1, num_variables, action);
			}
		}
	}
}

/*
 * send pattern[] to TDI and check for output on TDO
 * This is used for both loopback, and Shift-IR testing, i.e.
 * the pattern may show up with some delay.
 * return: 0 = no match
 *		   1 = match 
 *		   2 or greater = no pattern found but line appears active
 *
 * if retval == 1, *reglen returns the length of the register
 */
static uint8_t check_data(char pattern[], int iterations, int tck, int tdi, int tdo, uint8_t *reg_len)
{
	int i;
	int w = 0;
	int plen = strlen(pattern);
	char tdo_read;
	char tdo_prev;
	uint8_t nr_toggle = 0; // count how often tdo toggled
	/* we store the last plen (<=PATTERN_LEN) bits,
	 *  rcv[0] contains the oldest bit */
	char rcv[PATTERN_LEN];
	tdo_prev = '0' + (jtagReadPin(tdo) == 1);
	for (i = 0; i < iterations; i++)
	{
		/* output pattern and incr write index */
		jtagPulseTdi(tck, tdi, pattern[w++] - '0');
		if (!pattern[w])
		{
			w = 0;
		}
		/* read from TDO and put it into rcv[] */
		tdo_read = '0' + (jtagReadPin(tdo) == 1);

		nr_toggle += (tdo_read != tdo_prev);
		tdo_prev = tdo_read;

		if (i < plen)
		{
			rcv[i] = tdo_read;
		}
		else
		{
			memmove(rcv, rcv + 1, plen - 1);
			rcv[plen - 1] = tdo_read;
		}

		/* check if we got the pattern in rcv[] */
		if ((i >= (plen - 1)) && (!memcmp(pattern, rcv, plen)))
		{
			*reg_len = i + 1 - plen;
			return 1;
		}
	} /* for(i=0; ... ) */

	*reg_len = 0;
	return nr_toggle > 1 ? nr_toggle : 0;
}

static void patternScanTest(int pins[], uint8_t pinslen)
{
	uint8_t tck = pins[0];
	uint8_t tms = pins[1];
	uint8_t tdo = pins[2];
	uint8_t tdi = pins[3];
	uint8_t ntrst = pins[4];
	int checkdataret = 0;
	uint8_t reg_len;

	DEBUG("patternScanTest: checking pins tck:%d tms:%d tdo:%d tdi:%d ntrst:%d", tck, tms, tdo, tdi, ntrst);

	if (debug)
	{
		jtagPrintPins(tck, tms, tdo, tdi, ntrst);
		uartWriteString("	  ");
	}

	jtagInitPins(tck, tms, tdi, ntrst, pinslen);
	jtagTapState(TAP_SHIFTIR, tck, tms);
	checkdataret = check_data(pattern, (2 * PATTERN_LEN), tck, tdi, tdo, &reg_len);
	if (checkdataret == 1)
	{
		uartWriteString("JTAG Test Access Port Found!" NL);
		jtagPrintPins(tck, tms, tdo, tdi, ntrst);
		uartprintf(NL "IR length: %d", reg_len);
		uartWriteString(NL);
	}
	else if (checkdataret > 1)
	{
		//uartWriteString("active ");
		//jtagPrintPins(tck, tms, tdo, tdi, ntrst);
		//uartWriteString("	bits toggled:");
		//sprintf(buffer,"%d",checkdataret);
		//uartWriteString(buffer);
		//uartWriteChar(NL);
	}
	else if (debug)
	{
		uartWriteString(NL);
	}
}

static void loopbackTest(int pins[], uint8_t pinslen)
{
	int checkdataret = 0;
	uint8_t reg_len;
	uint8_t tdo = pins[0];
	uint8_t tdi = pins[1];

	if (debug)
	{
		uartprintf(" tdi:A%d tdo:A%d\t  " NL, tdi, tdo);
	}
	jtagInitPins(IGNOREPIN /*tck*/, IGNOREPIN /*tck*/, tdi, IGNOREPIN /*ntrst*/, pinslen);
	checkdataret = check_data(pattern, (2 * PATTERN_LEN), IGNOREPIN, tdi, tdo, &reg_len);
	if (checkdataret == 1)
	{
		uartprintf("FOUND!  tdo:A%d tdi:A" NL, tdo, tdi);
	}
	else if (checkdataret > 1)
	{
		uartprintf("active  tdo:A%d tdi:A\tbits toggled:%d" NL, tdo, tdi, checkdataret);
	}
	else if (debug)
	{
		uartWriteString(NL);
	}
}


static inline const char* getVendor(uint32_t idcode)
{	
	uint8_t temp_idcode = (uint8_t)(idcode >> 1);
	uartprintf("test");
	switch (temp_idcode)
	{
	case IDCODE_MASK_ATMEL: return "ATMEL";
	case IDCODE_MASK_AMD: 	return "AMD";
	case IDCODE_MASK_AMI: return "AMI";
	case IDCODE_MASK_FAIRCHILD: return "FAIRCHILD";
	case IDCODE_MASK_FUJITSU: return "Fujitsu";
	case IDCODE_MASK_GTE: return "GTE";
	case IDCODE_MASK_HARRIS: return "Harris";
	case IDCODE_MASK_HITACHI: return "Hitachi";
	case IDCODE_MASK_INMOS: return "Inmos";
	case IDCODE_MASK_INTEL: return "Intel";
	case IDCODE_MASK_ITT: return "I.T.T.";
	case IDCODE_MASK_INTERSIL: return "Intersil";
	case IDCODE_MASK_MONOLITHIC_MEMORIES: return "Monolithic Memories";
	case IDCODE_MASK_MOSTEK: return "Mostek";
	case IDCODE_MASK_FREESCALE: return "Freescale";
	case IDCODE_MASK_NATIONAL: return "National";
	case IDCODE_MASK_NEC: return "NEC";
	case IDCODE_MASK_RCA: return "RCA";
	case IDCODE_MASK_RAYTHEON: return "Raytheon";
	case IDCODE_MASK_CONEXANT: return "Conexant";
	case IDCODE_MASK_SEEQ: return "Seeq";
	case IDCODE_MASK_NXP: return "NXP";
	case IDCODE_MASK_SYNERTEK: return "Synertek";
	case IDCODE_MASK_TEXAS_INSTRUMENTS: return "Texas Instruments";
	case IDCODE_MASK_TOSHIBA: return "Toshiba";
	case IDCODE_MASK_XICOR: return "Xicor";
	case IDCODE_MASK_ZILOG: return "Zilog";
	case IDCODE_MASK_MARVELL_SEMICONDUCTORS: return "Marvell";
	default: return "UNKNOWN";
	}
}

static void idcodeTest(int pins[], uint8_t pinslen)
{
	int i, j;
	int tdo_read;
	uint32_t idcodes[MAX_DEV_NR];
	int tck = pins[0];
	int tms = pins[1];
	int tdo = pins[2];
	int tdi = pins[3];
	int ntrst = pins[4];

	DEBUG("idcodeTest: checking pins tck:%d tms:%d tdo:%d tdi:%d ntrst:%d", tck, tms, tdo, tdi, ntrst);

	if (debug)
	{
		jtagPrintPins(tck, tms, tdo, tdi, ntrst);
		uartWriteString("	  ");
	}

	jtagInitPins(tck, tms, tdi, ntrst, pinslen);
	/* we hope that IDCODE is the default DR after reset */
	jtagTapState(TAP_RESET, tck, tms);
	jtagTapState(TAP_SHIFTDR, tck, tms);
	/* j is the number of bits we pulse into TDI and read from TDO */
	for (i = 0; i < MAX_DEV_NR; i++)
	{
		idcodes[i] = 0;
		for (j = 0; j < IDCODE_LEN; j++)
		{
			/* we send '0' in */
			jtagPulseTdi(tck, tdi, 0);
			tdo_read = jtagReadPin(tdo);
			if (tdo_read)
			{
				idcodes[i] |= ((uint32_t)1) << j;
			}
			if (debug)
			{
				uartprintf("%d" NL, tdo_read);
			}
		} 
		if (debug)
		{
			uartprintf(" %d" NL, idcodes[i]);
		}
		/* abort when the last bit is not 1 (the last bit is always 1 on a valid ID) */
		if (!(idcodes[i] & 1) || idcodes[i] == 0xffffffff)
		{
			break;
		}
		uartprintf("IDCODE: %08X" NL, idcodes[i]);

	}

	if (i > 0)
	{
		jtagPrintPins(tck, tms, tdo, tdi, ntrst);
		uartprintf("	devices: %08X" NL, idcodes[i]);
		for (j = 0; j < i; j++)
		{
			uartprintf(" IDCode: 0x%08X" NL, idcodes[j]);
			uartprintf("%s controller detected" NL, getVendor(idcodes[j]));
			uartprintf("Part Number: 0x%04X" NL, ((idcodes[j] >> 11) & 0xFF));
			uartprintf("Revision: 0x%02X" NL, idcodes[j] >> 28);
		}
		uartWriteString(NL);
	}
}

static void bypassScanTest(int pins[], uint8_t pinslen)
{
	int checkdataret;
	uint8_t reg_len;
	uint8_t tdi = pins[0];
	uint8_t tdo = pins[1];
	uint8_t tck = pins[2];

	if (debug)
	{
		uartprintf(" tck:A%d tdi:A%d tdo:A%d\t  " NL, tck, tdi, tdo);
	}

	jtagInitPins(tck, IGNOREPIN /*tms*/, tdi, IGNOREPIN /*ntrst*/, pinslen);
	// if bypass is default on start, no need to init TAP state
	checkdataret = check_data(pattern, (2 * PATTERN_LEN), tck, tdi, tdo, &reg_len);
	if (checkdataret == 1)
	{
		uartprintf("FOUND!  tck:A%d tdo:A%d tdi:A" NL, tck, tdo, tdi);
	}
	else if (checkdataret > 1)
	{
		uartprintf("active  tck:A%d tdo:A%d tdi:A\tbits toggled:%d" NL, tck, tdo, tdi, checkdataret);

	}
	else if (debug)
	{
		uartWriteString(NL);
	}
}

/* ir_state()
 * Set TAP to Reset then ShiftIR. 
 * Shift in state[] as IR value.
 * Switch to ShiftDR state and end.
 */
static inline void ir_state(char state[], int tck, int tms, int tdi)
{
#ifdef DEBUGIR
	uartWriteString("ir_state: set TAP to ShiftIR:");
#endif
	jtagTapState(TAP_SHIFTIR, tck, tms);
#ifdef DEBUGIR
	uartWriteString("ir_state: jtagPulseTdi to: ");
#endif
	for (int i = 0; i < set_ir_len; i++)
	{
		if (jtagDelay)
			_delay_us(50);
		// TAP/TMS changes to Exit IR state (1) must be executed
		// at same time that the last TDI bit is sent:
		if (i == set_ir_len - 1)
		{
			jtagWritePin(tms, 1); // ExitIR
#ifdef DEBUGIR
			uartWriteString(" (will be in ExitIR after next bit) ");
#endif
		}
		jtagPulseTdi(tck, tdi, *state - '0');
#ifdef DEBUGIR
		uartprintf("%d", *state - '0');
#endif
		// TMS already set to 0 "shiftir" state to shift in bit to IR
		*state++;
	}
#ifdef DEBUGIR
	uartWriteString(NL "ir_state: Change TAP from ExitIR to ShiftDR:");
#endif
	// a reset would cause IDCODE instruction to be selected again
	jtagTapState("1100", tck, tms); // -1> UpdateIR -1> SelectDR -0> CaptureDR -0> ShiftDR
}

static inline void sample(int iterations, int tck, int tms, int tdi, int tdo, int ntrst)
{
	uartWriteString(OUTPUT_BOUNDARY_THICK "Starting sample (boundary scan)..." NL);
	jtagInitPins(tck, tms, tdi, ntrst, 0);

	// send instruction and go to ShiftDR
	ir_state(IR_SAMPLE, tck, tms, tdi);

	// Tell TAP to go to shiftout of selected data register (DR)
	// is determined by the instruction we sent, in our case
	// SAMPLE/boundary scan
	for (int i = 0; i < iterations; i++)
	{
		// no need to set TMS. It's set to the '0' state to
		// force a Shift DR by the TAP
		uartprintf("%d", jtagPulseTdo(tck, tdo));
		if (i % 32 == 31)
			uartWriteString(" ");
		if (i % 128 == 127)
			uartWriteString(NL);
	}
	uartWriteString(NL);
}

static inline void brute_ir(int iterations, int tck, int tms, int tdi, int tdo, int ntrst)
{
	char ir_buf[set_ir_len + 1];
	
	uartWriteString(OUTPUT_BOUNDARY_THICK
					"Starting brute force scan of IR instructions..." NL
					"NOTE: If Verbose mode is off output is only printed" NL
					"      after activity (bit changes) are noticed and" NL
					"      you might not see the first bit of output." NL
					"IR_LEN set to ");
	uartprintf("%d" NL, set_ir_len);
	
	jtagInitPins(tck, tms, tdi, ntrst, 0);
	int iractive;
	uint8_t tdo_read;
	uint8_t prevread;
	for (uint32_t ir = 0; ir < (1UL << set_ir_len); ir++)
	{
		iractive = 0;
		// send instruction and go to ShiftDR (ir_state() does this already)
		// convert ir to string.
		for (int i = 0; i < set_ir_len; i++)
			ir_buf[i] = ((ir >> i) & 0x01) + '0';
		ir_buf[set_ir_len] = 0; // terminate
		ir_state(ir_buf, tck, tms, tdi);
		// we are now in TAP_SHIFTDR state

		prevread = jtagPulseTdo(tck, tdo);
		for (int i = 0; i < iterations - 1; i++)
		{
			// no need to set TMS. It's set to the '0' state to force a Shift DR by the TAP
			tdo_read = jtagPulseTdo(tck, tdo);
			if (tdo_read != prevread)
				iractive++;

			if (iractive || debug)
			{
				uartprintf("%d", prevread);
				if (i % 16 == 15)
					uartWriteString(" ");
				if (i % 128 == 127)
					uartWriteString(NL);
			}
			prevread = tdo_read;
		}
		if (iractive || debug)
		{
			uartprintf("%d\tIr %p\tbits changed %d" NL, prevread, ir_buf, iractive);
		}
	}
}