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
// #define NAND_IO_PORT0 	PORTH
// #define NAND_IO_PORT1 	PORTJ
// #define NAND_CTRL_PORT 	PORTK

/*! \brief NOR interface pin definitions. */
#define NOR_ADDR_PORT0	PORTB
#define NOR_ADDR_PORT1	PORTC
#define NOR_ADDR_PORT2	PORTD
#define NOR_ADDR_PORT3	PORTF
#define NOR_DATA_PORT0	PORTH
#define NOR_DATA_PORT1	PORTJ
#define NOR_CTRL_PORT PORTK

typedef struct {
    PORT_t *port;
    uint8_t pin;
} port_pin_t;

typedef struct {
    port_pin_t ale;
    port_pin_t ce;
    port_pin_t cle;
    port_pin_t we;
    port_pin_t wp;
    port_pin_t r_b;
    port_pin_t re;
} Nand_Ctrl_Port_t;

port_pin_t NAND_IO_PORT0[8] = { \
{&PORTH,0},\
{&PORTH,1},\
{&PORTH,2},\
{&PORTH,3},\
{&PORTH,4},\
{&PORTH,5},\
{&PORTH,6},\
{&PORTH,7}}; 

port_pin_t NAND_IO_PORT1[8] = { \
{&PORTJ,0},\
{&PORTJ,1},\
{&PORTJ,2},\
{&PORTJ,3},\
{&PORTJ,4},\
{&PORTJ,5},\
{&PORTJ,6},\
{&PORTJ,7}}; 

Nand_Ctrl_Port_t NAND_CTRL_PORT = {
    .r_b = {&PORTK,6},
    .re = {&PORTK,1},
    .ce = {&PORTK,0},
    .cle = {&PORTK,5},
    .ale = {&PORTK,4},
    .we = {&PORTK,2},
    .wp = {&PORTK, 3},
};

#define DEBUG(x, ...) {}

#define NL "\r\n"

#endif
