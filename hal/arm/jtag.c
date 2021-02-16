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

static inline void setPinMode(uint8_t pin, uint8_t inout) //inout = 1 -> output enabled
{
	port_pin_t pin_info = JTAG_port[pin];
	setPinDir(pin_info, inout);
}

static inline void jtagWritePin(uint8_t pin, uint8_t level)
{
	port_pin_t pin_info = JTAG_port[pin];
	setPinValue(pin_info, level);
}

static inline uint8_t jtagReadPin(uint8_t pin)
{
	port_pin_t pin_info = JTAG_port[pin];
	return getPinValue(pin_info);
}
