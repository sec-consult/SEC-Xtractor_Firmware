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
#include "../../secxtractor.h"
#include "../../hal.h"

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
