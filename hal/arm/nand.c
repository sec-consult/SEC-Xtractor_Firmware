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
	if(pin_info.port == NULL) return;
	if(val){
		pin_info.port->BSRR = 1<< pin_info.pin;
	}else{
		pin_info.port->BSRR = 1<<(pin_info.pin+16);
	}
}

static inline uint8_t getPinValue(port_pin_t pin_info)
{
	return (pin_info.port->IDR & (1<<pin_info.pin))? 1:0;
}

static inline void setInputMode(port_pin_t pin_info, in_mode_t mode)
{
	if(pin_info.port == NULL) return;
	uint32_t pin_msk = 0x3<<(pin_info.pin*2);
	switch(mode){
		case GPIO_PU:
			pin_info.port->PUPDR = (pin_info.port->PUPDR & ~pin_msk) | (0x1<<(pin_info.pin*2));
			break;
		case GPIO_PD:
			pin_info.port->PUPDR = (pin_info.port->PUPDR & ~pin_msk) | (0x2<<(pin_info.pin*2));
			break;
		case GPIO_HIGH_Z:
			pin_info.port->PUPDR = (pin_info.port->PUPDR & ~pin_msk);
			break;
	}
}

static inline void setPinDir(port_pin_t pin_info, direction_t dir)
{
	if(pin_info.port == NULL) return;
	if(dir){
		pin_info.port->MODER = (pin_info.port->MODER & ~(0x3<<(pin_info.pin*2))) | (0x1<<(pin_info.pin*2));
		//setInputMode(pin_info, GPIO_HIGH_Z);
	}else{
		pin_info.port->MODER &= ~(0x3<<(pin_info.pin*2));
	}
}

#pragma unroll_completely
__attribute__((optimize("unroll-loops")))
static inline void  setNandIODir(direction_t dir)
{
	for(uint8_t i=0; i<8; i++){
		setPinDir(NAND_IO_PORT0[i], dir);
		setPinDir(NAND_IO_PORT1[i], dir);
	}
}
#pragma unroll_completely
__attribute__((optimize("unroll-loops")))
static inline void setNandIOValue(const uint16_t val)
{
	setPinValue(NAND_IO_PORT0[0], val & 0x1);
	setPinValue(NAND_IO_PORT0[1], (val & 0x2) ? HIGH:LOW);
	setPinValue(NAND_IO_PORT0[2], (val & 0x4) ? HIGH:LOW);
	setPinValue(NAND_IO_PORT0[3], (val & 0x8) ? HIGH:LOW);
	setPinValue(NAND_IO_PORT0[4], (val & 0x10) ? HIGH:LOW);
	setPinValue(NAND_IO_PORT0[5], (val & 0x20) ? HIGH:LOW);
	setPinValue(NAND_IO_PORT0[6], (val & 0x40) ? HIGH:LOW);
	setPinValue(NAND_IO_PORT0[7], (val & 0x80) ? HIGH:LOW);

	setPinValue(NAND_IO_PORT1[0], (val & 0x100) ? HIGH:LOW);
	setPinValue(NAND_IO_PORT1[1], (val & 0x200) ? HIGH:LOW);
	setPinValue(NAND_IO_PORT1[2], (val & 0x400) ? HIGH:LOW);
	setPinValue(NAND_IO_PORT1[3], (val & 0x800) ? HIGH:LOW);
	setPinValue(NAND_IO_PORT1[4], (val & 0x1000) ? HIGH:LOW);
	setPinValue(NAND_IO_PORT1[5], (val & 0x2000) ? HIGH:LOW);
	setPinValue(NAND_IO_PORT1[6], (val & 0x4000) ? HIGH:LOW);
	setPinValue(NAND_IO_PORT1[7], (val & 0x8000) ? HIGH:LOW);

	// for(uint8_t i=0; i<8; i++){
	// 	setPinValue(NAND_IO_PORT0[i], val & 0x1);
	// 	setPinValue(NAND_IO_PORT1[i], ((val & 0x100)? HIGH:LOW));
	// 	val >>= 1;
	// }
}
#pragma unroll_completely
__attribute__((optimize("unroll-loops")))
static inline void getNandIOValue(uint8_t *val_port0, uint8_t *val_port1)
{
	*val_port0 = ((getPinValue(NAND_IO_PORT0[7])<<7)|(getPinValue(NAND_IO_PORT0[6])<<6)|\
	(getPinValue(NAND_IO_PORT0[5])<<5)|(getPinValue(NAND_IO_PORT0[4])<<4)|\
	(getPinValue(NAND_IO_PORT0[3])<<3)|(getPinValue(NAND_IO_PORT0[2])<<2)|\
	(getPinValue(NAND_IO_PORT0[1])<<1)|(getPinValue(NAND_IO_PORT0[0])));
	*val_port1 = ((getPinValue(NAND_IO_PORT1[7])<<7)|(getPinValue(NAND_IO_PORT1[6])<<6)|\
	(getPinValue(NAND_IO_PORT1[5])<<5)|(getPinValue(NAND_IO_PORT1[4])<<4)|\
	(getPinValue(NAND_IO_PORT1[3])<<3)|(getPinValue(NAND_IO_PORT1[2])<<2)|\
	(getPinValue(NAND_IO_PORT1[1])<<1)|(getPinValue(NAND_IO_PORT1[0])));
	// for(int8_t i=7; i>=0; i--){
	// 	uint8_t in0 = getPinValue(NAND_IO_PORT0[i]);
	// 	uint8_t in1 = getPinValue(NAND_IO_PORT1[i]);
	// 	*val_port0 = (*val_port0 << 1) | in0;
	// 	*val_port1 = (*val_port1 << 1) | in1;
	// }
}
// #pragma unroll_completely
// __attribute__((optimize("unroll-loops")))
// static inline void _delay_32mhz_cycles(uint16_t cycles_at_32mhz)
// {
// 	for(uint32_t i = cycles_at_32mhz*5.25; i>0; i--){
// 		__NOP();
// 	}
// }
#pragma unroll_completely
__attribute__((optimize("unroll-loops")))
static inline void delay_ns(const uint32_t delay)
{
	const uint32_t cycles = ((delay+1)*F_CPU)/2000000000;
	for(uint32_t i = cycles; i>0; i--){
		__NOP();
	}
}

static inline uint8_t getNandRB()
{
    return getPinValue(NAND_CTRL_PORT.r_b);
}

static inline void initPins()
{
	port_pin_t* ctrl_port = (port_pin_t*) &NAND_CTRL_PORT.ale;
	for(uint8_t i=0; i<7; ++i){
		setInputMode(ctrl_port[i], GPIO_PD);
		setPinDir(ctrl_port[i], OUT);
		setPinValue(ctrl_port[i], LOW);
		ctrl_port[i].port->OSPEEDR |= 0x3<<(ctrl_port[i].pin*2);
	}
	for(uint8_t i=0; i<8; ++i){
		setInputMode(NAND_IO_PORT0[i], GPIO_PD);
		setPinDir(NAND_IO_PORT0[i], OUT);
		setPinValue(NAND_IO_PORT0[i], LOW);
		NAND_IO_PORT0[i].port->OSPEEDR |= 0x3<<(NAND_IO_PORT0[i].pin*2);
	}
	for(uint8_t i=0; i<8; ++i){
		setInputMode(NAND_IO_PORT1[i], GPIO_PD);
		setPinDir(NAND_IO_PORT1[i], OUT);
		setPinValue(NAND_IO_PORT1[i], LOW);
		NAND_IO_PORT1[i].port->OSPEEDR |= 0x3<<(NAND_IO_PORT1[i].pin*2);
	}
}