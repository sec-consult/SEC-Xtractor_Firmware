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

static inline void setPinMode(uint8_t pin, uint8_t inout) //inout = 1 -> output enabled
{
	if (pin < 8)
	{
		if (inout)
		{
			PORTB.DIRSET = (1 << pin);
		}
		else
		{
			PORTB.DIRCLR = (1 << pin);
		}
	}
	else if (pin < 16)
	{
		if (inout)
		{
			PORTC.DIRSET = (1 << (pin - 8));
		}
		else
		{
			PORTC.DIRCLR = (1 << (pin - 8));
		}
	}
	else if (pin < 24)
	{
		if (inout)
		{
			PORTD.DIRSET = (1 << (pin - 16));
		}
		else
		{
			PORTD.DIRCLR = (1 << (pin - 16));
		}
	}
	else if (pin < 32)
	{
		if (inout)
		{
			PORTF.DIRSET = (1 << (pin - 24));
		}
		else
		{
			PORTF.DIRCLR = (1 << (pin - 24));
		}
	}
	return;
}

static inline void jtagWritePin(uint8_t pin, uint8_t level)
{
	if (pin < 8)
	{
		if (level)
		{
			PORTB.OUTSET = (1 << pin);
		}
		else
		{
			PORTB.OUTCLR = (1 << pin);
		}
	}
	else if (pin < 16)
	{
		if (level)
		{
			PORTC.OUTSET = (1 << (pin - 8));
		}
		else
		{
			PORTC.OUTCLR = (1 << (pin - 8));
		}
	}
	else if (pin < 24)
	{
		if (level)
		{
			PORTD.OUTSET = (1 << (pin - 16));
		}
		else
		{
			PORTD.OUTCLR = (1 << (pin - 16));
		}
	}
	else if (pin < 32)
	{
		if (level)
		{
			PORTF.OUTSET = (1 << (pin - 24));
		}
		else
		{
			PORTF.OUTCLR = (1 << (pin - 24));
		}
	}
	return;
}

static inline uint8_t jtagReadPin(uint8_t pin)
{
	uint8_t value = 0;
	if (pin < 8) //porta
	{
		value = (PORTB.IN & (1 << pin)) >> pin;
	}
	else if (pin < 16)
	{
		value = (PORTC.IN & (1 << (pin - 8))) >> (pin - 8);
	}
	else if (pin < 24)
	{
		value = (PORTD.IN & (1 << (pin - 16))) >> (pin - 16);
	}
	else if (pin < 32)
	{
		value = (PORTF.IN & (1 << (pin - 24))) >> (pin - 24);
	}
	return value;
}

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
	// ntrst = output, fixed to 1
	if (ntrst != IGNOREPIN)
	{
		setPinMode(ntrst, 1);
		jtagWritePin(ntrst, 1);
	}
}

static void jtagPrintPins(int tck, int tms, int tdo, int tdi, int ntrst)
{
	char *buffer = calloc(1, 6);
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
