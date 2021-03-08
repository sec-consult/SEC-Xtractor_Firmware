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
 * @authors Thomas Weber, Wolfgang Ettlinger, Steffeb Robertz
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
port_pin_t nor_addr_port[4][8] = {
    {
        {GPIOD,13}, //A0
        {GPIOB,2},  //A1
        {GPIOB,6},  //A2
        {GPIOF,4},  //A3
        {GPIOC,2},  //A4
        {GPIOB,1},  //A5
        {GPIOB,4},  //A6
        {GPIOA,4},  //A7
    },
    {
        {GPIOB,3},  //A8
        {GPIOB,5},  //A9
        {GPIOC,7},  //A10
        {GPIOA,15}, //A11
        {GPIOB,12}, //A12
        {GPIOB,13}, //A13
        {GPIOB,15}, //A14
        {GPIOC,6},  //A15
    },
    {
        {NULL,0},   //A16
        {NULL,0},   //A17
        {NULL,0},   //A18
        {NULL,0},   //A19
        {NULL,0},   //A20
        {NULL,0},   //A21
        {NULL,0},   //A22
        {NULL,0},   //A23
    },
    {
        {NULL,0},   //A24
        {NULL,0},   //A25
        {NULL,0},   //A26
        {NULL,0},   //A27
        {NULL,0},   //A28
        {NULL,0},   //A29
        {NULL,0},   //A30
        {NULL,0},   //A31
    }
};

port_pin_t nor_data_port[2][8] = {
    {
        {NULL,0},   //D0
        {NULL,0},   //D1
        {NULL,0},   //D2
        {NULL,0},   //D3
        {NULL,0},   //D4
        {NULL,0},   //D5
        {NULL,0},   //D6
        {NULL,0},   //D7
    },
    {
        {NULL,0},   //D8
        {NULL,0},   //D9
        {NULL,0},   //D10
        {NULL,0},   //D11
        {NULL,0},   //D12
        {NULL,0},   //D13
        {NULL,0},   //D14
        {NULL,0},   //D15
    }
};

typedef struct __packed{
    port_pin_t ce;
    port_pin_t oe;
    port_pin_t we;
    port_pin_t wp;
    port_pin_t reset;
    port_pin_t byte;
    port_pin_t ryby;
} Nor_Ctrl_Port_t;

Nor_Ctrl_Port_t nor_ctrl_port = {
    .ryby = {GPIOD,7},
    .oe = {GPIOD,6},
    .ce = {GPIOD,5},
    .byte = {GPIOD,4},
    .reset = {GPIOD,3},
    .we = {GPIOE, 4},
    .wp = {GPIOE, 2},
};

/*! \brief JTAG Pin defintions */
port_pin_t JTAG_port[] = {
{GPIOD,13}, //A0
{GPIOB,2},  //A1
{GPIOB,6},  //A2
{GPIOF,4},  //A3
{GPIOC,2},  //A4
{GPIOB,1},  //A5
{GPIOB,4},  //A6
{GPIOA,4},  //A7
{GPIOB,3},  //A8
{GPIOB,5},  //A9
{GPIOC,7},  //A10
{GPIOA,15}, //A11
{GPIOB,12}, //A12
{GPIOB,13}, //A13
{GPIOB,15}, //A14
{GPIOC,6},  //A15
{NULL,0},   //A16
{NULL,0},   //A17
{NULL,0},   //A18
{NULL,0},   //A19
{NULL,0},   //A20
{NULL,0},   //A21
{NULL,0},   //A22
{NULL,0},   //A23
{NULL,0},   //A24
{NULL,0},   //A25
{NULL,0},   //A26
{NULL,0},   //A27
{NULL,0},   //A28
{NULL,0},   //A29
{NULL,0},   //A30
{NULL,0}   //A31
};

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

#define DEBUG(x, ...) {}

#define NL "\r\n"

#endif
