/**
 * @file nand.c
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
#include "definitions.h"
#include "../../secxtractor.h"

typedef enum {
	HIGH = 1,
	LOW	 = 0
} state_t;

typedef enum {
	IN = 0,
	OUT = 1
} direction_t;

static inline void setPinValue(PORT_t *port,  uint8_t pin_mask, state_t val)
{
	if(val){
		port->OUTSET = pin_mask;
	}else{
		port->OUTCLR = pin_mask;
	}
}

static inline void setPinDir(PORT_t *port,  uint8_t pin_mask, direction_t dir)
{
	if(dir){
		port->DIRSET = pin_mask;
	}else{
		port->DIRCLR = pin_mask;
	}
}

static inline void  setNandIODir(direction_t dir)
{
	if(dir){
		NAND_IO_PORT0.DIR = 0xff;
		NAND_IO_PORT1.DIR = 0xff;
	}else{
		NAND_IO_PORT0.DIR = 0x00;
		NAND_IO_PORT1.DIR = 0x00;
	}
}
static inline void setNandIOValue(uint16_t val)
{
	NAND_IO_PORT0.OUT = val;
	NAND_IO_PORT1.OUT = (val >> 8);
}
static inline void getNandIOValue(uint8_t *val_port0, uint8_t *val_port1)
{
	*val_port0 = NAND_IO_PORT0.IN;
	*val_port1 = NAND_IO_PORT1.IN;
}
static inline void _delay_32mhz_cycles(uint8_t cycles_at_32mhz)
{
	uint8_t cycles = cycles_at_32mhz *F_CPU/32000000;
	for(uint8_t i = 0; i<cycles; ++i){
		NOP;
	}
}
static inline uint8_t getNandRB()
{
	return (((NAND_CTRL_PORT.IN & NAND_RB) == NAND_RB) ? 1 : 0);
}