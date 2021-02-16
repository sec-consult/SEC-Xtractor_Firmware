/**
 * @file gpio.c
 * 
 * Copyright (C): SEC Consult Unternehmensberatung GmbH, 2019         \n
 * Web:           https://sec-consult.com/                            \n
 *                                                                    \n
 * This Source Code Form is subject to the terms of the Mozilla Public\n
 * License, v. 2.0. If a copy of the MPL was not distributed with this\n
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.          \n
 * 
 * @authors Steffen Robertz
 */
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
	}else{
        setInputMode(pin_info, GPIO_HIGH_Z);
		pin_info.port->MODER &= ~(0x3<<(pin_info.pin*2));
	}
}

#pragma unroll_completely
__attribute__((optimize("unroll-loops")))
static inline void delay_ns(const uint32_t delay)
{
	const uint32_t cycles = ((delay+1)*F_CPU)/2000000000;
	for(uint32_t i = cycles; i>0; i--){
		__NOP();
	}
}

static inline void _delay_us(const uint32_t delay)
{
    delay_ns(delay*1000);
}