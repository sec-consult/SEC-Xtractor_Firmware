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

}

/**
*@brief Selects Word transfer mode
*/
static inline void norModeWord()
{

}

/**
*@brief Resets the NOR flash
*/
static void norResetFlash()
{

}

/**
*@brief Inits Pins for a read from NOR flash
*/
static inline void norStartReadData()
{

}

/**
*@brief Finishes the NOR Data read
*/
static inline void norEndReadData()
{
  
}

/**
*@brief Reads a word from the NOR
*@return    2 Byte of Data
*/
static inline uint16_t norRead16DataBits(uint32_t address)
{
    uint16_t databyte = 1;
    return databyte;
}

/**
*@brief Reads a byte from the NOR
*@return    Databyte
*@param address The address to read from
*/
static inline uint8_t norRead8DataBits(uint32_t address)
{
    uint8_t databyte=1;
    
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
