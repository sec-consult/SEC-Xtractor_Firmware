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

#pragma unroll_completely
__attribute__((optimize("unroll-loops")))
static inline void setNorPortValue(const port_pin_t *port, uint8_t val)
{
    for(uint8_t i=0; i<8; i++){
	 	setPinValue(port[i], val & 0x1);
        val >>=1;
    }
}
#pragma unroll_completely
__attribute__((optimize("unroll-loops")))
static inline void setNorPortMode(const port_pin_t *port, const uint8_t dir)
{
    for(uint8_t i=0; i<8; i++){
	 	setPinDir(port[i], dir);
    }
}
#pragma unroll_completely
__attribute__((optimize("unroll-loops")))
static inline void getNorPortValue(port_pin_t *port, uint8_t *value)
{
	for(int8_t i=7; i>=0; i--){
		uint8_t data = getPinValue(port[i]);
		*value = (*value << 1) | data;
	}
}
//TODO: Check if Tyimings work
/**
*@brief Selects Byte transfer mode
*/
static inline void norModeByte()
{
    setPinValue(nor_ctrl_port.byte, LOW);
    setNorPortMode(nor_data_port[1], OUT);
}

/**
*@brief Selects Word transfer mode
*/
static inline void norModeWord()
{
    setPinValue(nor_ctrl_port.byte, HIGH);
}

/**
*@brief Resets the NOR flash
*/
static void norResetFlash()
{
    setPinValue(nor_ctrl_port.ce, HIGH);
    setPinValue(nor_ctrl_port.oe, HIGH);
    setPinValue(nor_ctrl_port.reset, LOW);
    _delay_us(5000);
    setPinValue(nor_ctrl_port.reset, HIGH);
    
    setPinValue(nor_ctrl_port.ce, LOW);
    setPinValue(nor_ctrl_port.oe, LOW);
    _delay_us(5000);

    setPinValue(nor_ctrl_port.ce, HIGH);
    setPinValue(nor_ctrl_port.oe, HIGH);

    while (!getPinValue(nor_ctrl_port.ryby));
}

/**
*@brief Inits Pins for a read from NOR flash
*/
static inline void norStartReadData()
{
    _delay_us(2);
    setPinValue(nor_ctrl_port.reset, HIGH);
    _delay_us(1);

    setPinValue(nor_ctrl_port.we, HIGH);
    setPinValue(nor_ctrl_port.ce, LOW);
    setPinValue(nor_ctrl_port.oe, LOW);
}

/**
*@brief Finishes the NOR Data read
*/
static inline void norEndReadData()
{
  _delay_us(5);
  setPinValue(nor_ctrl_port.oe, HIGH);
  setPinValue(nor_ctrl_port.ce, HIGH);
}

/**
*@brief Reads a word from the NOR
*@return    2 Byte of Data
*/
static inline uint16_t norRead16DataBits(uint32_t address)
{
    uint16_t databyte;
    //apply address
    setNorPortValue(nor_addr_port[0],(uint8_t)(address & 0x000000FF));
    setNorPortValue(nor_addr_port[1],(uint8_t)((address & 0x0000FF00) >> 8));
    setNorPortValue(nor_addr_port[2],(uint8_t)((address & 0x00FF0000) >> 16));
    setNorPortValue(nor_addr_port[3],(uint8_t)((address & 0xFF000000) >> 24));
    
    uint8_t lower;
    getNorPortValue(nor_data_port[0], &lower);
    uint8_t upper;
    getNorPortValue(nor_data_port[0], &upper);

    databyte = ((uint16_t)upper)<<8 | lower;
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
    
    setPinValue(nor_ctrl_port.we, HIGH);

    setPinValue(nor_data_port[1][7], (uint8_t)(address & 0x00000001));// A-1/Q15

     //apply address
    setNorPortValue(nor_addr_port[0],(uint8_t)((address & 0x000001FE) >> 1));
    setNorPortValue(nor_addr_port[1],(uint8_t)((address & 0x0001FE00) >> 9));
    setNorPortValue(nor_addr_port[2],(uint8_t)((address & 0x01FE0000) >> 17));
    setNorPortValue(nor_addr_port[3],(uint8_t)((address & 0xFE000000) >> 25));

    setPinValue(nor_ctrl_port.ce, LOW);
    setPinValue(nor_ctrl_port.oe, LOW);

    getNorPortValue(nor_data_port[0], &databyte);

    setPinValue(nor_ctrl_port.oe, HIGH);
    setPinValue(nor_ctrl_port.ce, HIGH);

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
