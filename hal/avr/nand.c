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

typedef enum {
	GPIO_PU = 0,
	GPIO_PD = 1,
	GPIO_HIGH_Z =2
} in_mode_t;

static inline void setPinValue(port_pin_t pin_info, state_t val)
{
	if(val){
		pin_info.port->OUTSET = (1<<pin_info.pin);
	}else{
		pin_info.port->OUTCLR = (1<<pin_info.pin);
	}
}

static inline void setInputMode(port_pin_t pin_info, in_mode_t mode)
{
	switch(mode){
		case GPIO_PU:
			pin_info.port->OUTSET = (1<<pin_info.pin);
			break;
		case GPIO_PD:
			break;
		case GPIO_HIGH_Z:
			pin_info.port->OUTCLR = (1<<pin_info.pin);
			break;
	}
}

static inline void setPinDir(port_pin_t pin_info, direction_t dir)
{
	if(dir){
		pin_info.port->DIRSET = (1<<pin_info.pin);
	}else{
		pin_info.port->DIRCLR = (1<<pin_info.pin);
	}
}

static inline void  setNandIODir(direction_t dir)
{
	if(dir){
		NAND_IO_PORT0[0].port->DIR = 0xff;
		NAND_IO_PORT1[0].port->DIR = 0xff;
	}else{
		NAND_IO_PORT0[0].port->DIR = 0x00;
		NAND_IO_PORT1[0].port->DIR = 0x00;
	}
}
static inline void setNandIOValue(uint16_t val)
{
	NAND_IO_PORT0[0].port->OUT = val;
	NAND_IO_PORT1[0].port->OUT = (val >> 8);
}
static inline void getNandIOValue(uint8_t *val_port0, uint8_t *val_port1)
{
	*val_port0 = NAND_IO_PORT0[0].port->IN;
	*val_port1 = NAND_IO_PORT1[0].port->IN;
}
// static inline void _delay_32mhz_cycles(uint16_t cycles_at_32mhz)
// {
// 	uint8_t cycles = cycles_at_32mhz *F_CPU/32000000;
// 	for(uint8_t i = 0; i<cycles; ++i){
// 		NOP;
// 	}
// }

static inline void delay_ns(const uint16_t delay)
{
	//loop too slow, NOP=31ns
	for(uint16_t i=31; i<delay; i +=31){
		NOP;
	}


	// const uint32_t cycles = (delay*F_CPU)/1000000000;
	// for(uint32_t i = cycles; i>0; i--){
	// 	NOP;
	// }
}

static inline uint8_t getNandRB()
{
	return ((NAND_CTRL_PORT.r_b.port->IN & (1<<NAND_CTRL_PORT.r_b.pin)) ? 1 : 0);
}

static inline void initPins()
{
	port_pin_t* ctrl_port = (port_pin_t*) &NAND_CTRL_PORT.ale;
	for(uint8_t i=0; i<7; ++i){
		setInputMode(ctrl_port[i], GPIO_HIGH_Z);
		setPinDir(ctrl_port[i], IN);
	}
	for(uint8_t i=0; i<8; ++i){
		setInputMode(NAND_IO_PORT0[i], GPIO_HIGH_Z);
		setPinDir(NAND_IO_PORT0[i], IN);
	}
	for(uint8_t i=0; i<8; ++i){
		setInputMode(NAND_IO_PORT1[i], GPIO_HIGH_Z);
		setPinDir(NAND_IO_PORT1[i], IN);
	}
}