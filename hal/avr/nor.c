/**
 * @file nor.c
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

/**
*@brief Selects Byte transfer mode
*/
static inline void norModeByte()
{
    NOP2;    
    /* clear BYTE# */
    NOR_CTRL_PORT.OUTCLR = NOR_BYTE;
    NOR_DATA_PORT1.DIRSET = 0xF0; // Configure Q15-Q8 as output
    NOP2;
}

/**
*@brief Selects Word transfer mode
*/
static inline void norModeWord()
{
    NOP2;
    
    /* set BYTE# */
    NOR_CTRL_PORT.OUTSET = NOR_BYTE;
    
    NOP2;
}

/**
*@brief Resets the NOR flash
*/
static void norResetFlash()
{
    /* set CE# OE#, clear RESET */
	NOR_CTRL_PORT.OUTSET = NOR_CE;
	NOR_CTRL_PORT.OUTSET = NOR_OE;
	NOR_CTRL_PORT.OUTCLR = NOR_RESET;

	_delay_ms(5);
	NOP8;
	
    /* set RESET# */
    NOR_CTRL_PORT.OUTSET = NOR_RESET;

	NOP8;
	
    /* clear CE#, OE# */
    NOR_CTRL_PORT.OUTCLR = NOR_CE;
	NOR_CTRL_PORT.OUTCLR = NOR_OE;
	
    NOP8;
	_delay_ms(5);
	
    /* set CE#, OE# */
    NOR_CTRL_PORT.OUTSET = NOR_CE;
	NOR_CTRL_PORT.OUTSET = NOR_OE;
	
    /* wait for RY/BY# intput to go low */
    while (NOR_CTRL_PORT.IN & NOR_RYBY != NOR_RYBY);
}

/**
*@brief Inits Pins for a read from NOR flash
*/
static inline void norStartReadData()
{
    DELAY_US(2);
    
    /* set REST#, WE# anc clear CE#, OE# */
    NOR_CTRL_PORT.OUTSET = NOR_RESET;
    
    _delay_us(1);

    NOR_CTRL_PORT.OUTSET = NOR_WE;
    NOR_CTRL_PORT.OUTCLR = NOR_CE;
    NOR_CTRL_PORT.OUTCLR = NOR_OE;
    
    NOP5;
}

/**
*@brief Finishes the NOR Data read
*/
static inline void norEndReadData()
{
    DELAY_US(5);
    NOP5;
    
    NOR_CTRL_PORT.OUTSET = NOR_OE | NOR_CE;
}

/**
*@brief Reads a word from the NOR
*@return    2 Byte of Data
*/
static inline uint16_t norRead16DataBits(uint32_t address)
{
    uint16_t databyte;

    /* apply address */
    NOR_ADDR_PORT0.OUT = (uint8_t)(address & 0x000000FF);
    NOR_ADDR_PORT1.OUT = (uint8_t)((address & 0x0000FF00) >> 8);
    NOR_ADDR_PORT2.OUT = (uint8_t)((address & 0x00FF0000) >> 16);
    NOR_ADDR_PORT3.OUT = (uint8_t)((address & 0xFF000000) >> 24);
    NOP5;
    databyte = NOR_DATA_PORT0.IN;
    databyte |= (uint16_t)(NOR_DATA_PORT1.IN << 8);

    return databyte;
}

/**
*@brief Reads a byte from the NOR
*@return    Databyte
*@param address The address to read from
*/
static inline uint8_t norRead8DataBits(uint32_t address)
{
    uint8_t databyte;

    NOR_CTRL_PORT.OUTSET = NOR_WE;
    
    NOR_DATA_PORT1.OUT = (uint8_t)((address & 0x00000001) << 7); // A-1/Q15
    
    /* apply address */
    NOR_ADDR_PORT0.OUT = (uint8_t)((address & 0x000001FE) >> 1);
    NOR_ADDR_PORT1.OUT = (uint8_t)((address & 0x0001FE00) >> 9);
    NOR_ADDR_PORT2.OUT = (uint8_t)((address & 0x01FE0000) >> 17);
    NOR_ADDR_PORT3.OUT = (uint8_t)((address & 0xFE000000) >> 25);
    
    /* clear CE#, OE# */
    NOR_CTRL_PORT.OUTCLR = NOR_CE;
    NOR_CTRL_PORT.OUTCLR = NOR_OE;

    NOP5;
    databyte = NOR_DATA_PORT0.IN;
    NOP;

    NOR_CTRL_PORT.OUTSET = NOR_OE | NOR_CE;
    
    return databyte;
}

///TODO: Fix the Write and Erase Method
// static inline void norWrite8DataBits(uint32_t address, uint8_t data)
// {
//     NOR_CTRL_PORT.OUTSET = NOR_RESET;
//     NOR_CTRL_PORT.OUTSET = NOR_WP;    
//     _delay_us(1);

//     NOR_CTRL_PORT.OUTSET = NOR_WE;
//     NOR_CTRL_PORT.OUTSET = NOR_OE;

//     /* apply address */
//     NOR_DATA_PORT1.OUT = (uint8_t)((address & 0x00000001) << 7); // A-1/Q15
//     NOR_ADDR_PORT0.OUT = (uint8_t)((address & 0x000001FE) >> 1);
//     NOR_ADDR_PORT1.OUT = (uint8_t)((address & 0x0001FE00) >> 9);
//     NOR_ADDR_PORT2.OUT = (uint8_t)((address & 0x01FE0000) >> 17);
//     NOR_ADDR_PORT3.OUT = (uint8_t)((address & 0xFE000000) >> 25);

//     /* address fetched at falling WE flank */
//     NOR_CTRL_PORT.OUTCLR = NOR_WE;
//     NOP2;
//     /* apply data */
//     NOR_DATA_PORT0.OUT = data;
//     /*data fetched at rising WE flank */
//     NOR_CTRL_PORT.OUTSET = NOR_WE;
//     NOP;
//     NOR_CTRL_PORT.OUTSET = NOR_CE;
//     NOR_CTRL_PORT.OUTCLR = NOR_WE;
// }
//
// static inline void norEraseChip()
// {
//     norWrite8DataBits(0x555, 0xaa);
//     norWrite8DataBits(0x2aa, 0x55);
//     norWrite8DataBits(0x555, 0x80);
//     norWrite8DataBits(0x555, 0xaa);
//     norWrite8DataBits(0x2aa, 0x55);
//     norWrite8DataBits(0x555, 0x10);



//     // norWrite8DataBits(0xaaa, 0xaa);
//     // norWrite8DataBits(0x555, 0x55);
//     // norWrite8DataBits(0xaaa, 0x80);
//     // norWrite8DataBits(0xaaa, 0xaa);
//     // norWrite8DataBits(0x555, 0x55);
//     // norWrite8DataBits(0xaaa, 0x10);

//     NOR_CTRL_PORT.DIRSET = NOR_RYBY;
//     NOR_CTRL_PORT.OUTCLR = NOR_RYBY; 

//     while (!(NOR_CTRL_PORT.IN & NOR_RYBY));
//     uartprintf("Chip Erased!" NL);
// }
