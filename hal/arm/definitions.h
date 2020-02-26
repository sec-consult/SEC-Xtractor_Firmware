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

#include "stm32f446x.h"

/*! @brief No operation inline assembly definition */
//#define NOP do { __asm__ __volatile__ ("nop"); } while (0)

//#define DELAY_MS(ms) _delay_ms(ms)
//#define DELAY_US(us) _delay_us(us)

/* port for voltage ADC and 7seg display */
#define DEVICE_FUNCTION_PORT PORTA

/*! @brief NAND interface pin definitions. */

typedef struct __packed{
    GPIO_TypeDef *port;
    uint8_t pin;
} port_pin_t;

typedef struct __packed{
    port_pin_t ale;
    port_pin_t ce;
    port_pin_t cle;
    port_pin_t we;
    port_pin_t wp;
    port_pin_t r_b;
    port_pin_t re;
} Nand_Ctrl_Port_t;

port_pin_t NAND_IO_PORT0[8] = {\
{GPIOD,13},\
{GPIOB,2},\
{GPIOB,6},\
{GPIOF,4},\
{GPIOC,2},\
{GPIOB,1},\
{GPIOB,4},\
{GPIOA,4}}; 

port_pin_t NAND_IO_PORT1[8] = {
{GPIOB,3},\
{GPIOB,5},\
{GPIOC,7},\
{GPIOA,15},\
{GPIOB,12},\
{GPIOB,13},\
{GPIOB,15},\
{GPIOC,6}}; 

Nand_Ctrl_Port_t NAND_CTRL_PORT = {
    .r_b = {GPIOD,7},
    .re = {GPIOD,6},
    .ce = {GPIOD,5},
    .cle = {GPIOD,4},
    .ale = {GPIOD,3},
    .we = {GPIOE, 4},
    .wp = {GPIOE, 2},
};

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
