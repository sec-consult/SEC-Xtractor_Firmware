/**
 * @file definitions.h
 * 
 * Copyright (C): SEC Consult Unternehmensberatung GmbH, 2019         \n
 * Web:           https://sec-consult.com/                            \n
 *                                                                    \n
 * This Source Code Form is subject to the terms of the Mozilla Public\n
 * License, v. 2.0. If a copy of the MPL was not distributed with this\n
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.          \n
 * 
 * @authors Thomas Weber, Wolfgang Ettlinger
 */
#ifndef _DEFINITIONS_H
#define _DEFINITIONS_H 

#include <avr/io.h>

typedef PORT_t xtractor_port_t;

/*! @brief No operation inline assembly definition */
#define NOP do { __asm__ __volatile__ ("nop"); } while (0)

#define DELAY_MS(ms) _delay_ms(ms)
#define DELAY_US(us) _delay_us(us)

/* port for voltage ADC and 7seg display */
#define DEVICE_FUNCTION_PORT PORTA

/*! @brief NAND interface pin definitions. */
#define NAND_IO_PORT0 	PORTH
#define NAND_IO_PORT1 	PORTJ
#define NAND_CTRL_PORT 	PORTK

/*! \brief NOR interface pin definitions. */
#define NOR_ADDR_PORT0	PORTB
#define NOR_ADDR_PORT1	PORTC
#define NOR_ADDR_PORT2	PORTD
#define NOR_ADDR_PORT3	PORTF
#define NOR_DATA_PORT0	PORTH
#define NOR_DATA_PORT1	PORTJ
#define NOR_CTRL_PORT PORTK

#define DEBUG(x, ...) {}

#define NL "\r\n"

#endif
