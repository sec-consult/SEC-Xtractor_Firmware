/**
 * @file secxtractor.h
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
#ifndef _SECXTRACTOR_H
#define _SECXTRACTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define GLOBAL_VERSION_STRING "SEC Xtractor, version 1.31(0.5)"
#define SHELL_STRING "[hacker@SEC Xtractor]# "

#pragma region utility stuff
#define OUTPUT_BOUNDARY "-----------------------" NL
#define OUTPUT_BOUNDARY_THICK "=======================" NL

#define NOP2  {NOP; NOP;}
#define NOP3  {NOP; NOP; NOP;}
#define NOP4  {NOP; NOP; NOP; NOP;}
#define NOP5  {NOP; NOP; NOP; NOP; NOP;}
#define NOP6  {NOP; NOP; NOP; NOP; NOP; NOP;}
#define NOP7  {NOP; NOP; NOP; NOP; NOP; NOP; NOP;}
#define NOP8  {NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;}
#define NOP9  {NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;}
#define NOP10 {NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;}
#pragma endregion

#pragma region UART_SCANNER
#define SCANNER_MAX_PIN_CNT (16)
#pragma endregion

#pragma region NOR
#define NOR_CE (1 << 0)
#define NOR_OE (1 << 1)
#define NOR_WE (1 << 2)
#define NOR_WP (1 << 3)
#define NOR_RESET (1 << 4)
#define NOR_BYTE (1 << 5)
#define NOR_RYBY (1 << 6)
#define NOR_NC (1 << 7)

#define NOR_READ_LENGTH 0x200000
#pragma endregion

#pragma region ID codes
/*! \brief General chip detection mask table. */
#define IDCODE_MASK_GENERIC 0x7E

#define IDCODE_MASK_AMD (0x01)
#define IDCODE_MASK_AMI (0x02)
#define IDCODE_MASK_FAIRCHILD (0x83)
#define IDCODE_MASK_FUJITSU (0x04)
#define IDCODE_MASK_GTE (0x85)
#define IDCODE_MASK_HARRIS (0x86)
#define IDCODE_MASK_HITACHI (0x07)
#define IDCODE_MASK_INMOS (0x08)
#define IDCODE_MASK_INTEL (0x89)
#define IDCODE_MASK_ITT (0x8A)
#define IDCODE_MASK_INTERSIL (0x0B)
#define IDCODE_MASK_MONOLITHIC_MEMORIES (0x8C)
#define IDCODE_MASK_MOSTEK (0x0D)
#define IDCODE_MASK_FREESCALE (0x0E)
#define IDCODE_MASK_NATIONAL (0x8F)
#define IDCODE_MASK_NEC (0x10)
#define IDCODE_MASK_RCA (0x91)
#define IDCODE_MASK_RAYTHEON (0x92)
#define IDCODE_MASK_CONEXANT (0x13)
#define IDCODE_MASK_SEEQ (0x94)
#define IDCODE_MASK_NXP (0x15)
#define IDCODE_MASK_SYNERTEK (0x16)
#define IDCODE_MASK_TEXAS_INSTRUMENTS (0x97)
#define IDCODE_MASK_TOSHIBA (0x98)
#define IDCODE_MASK_XICOR (0x19)
#define IDCODE_MASK_ZILOG (0x1A)
#define IDCODE_MASK_EUROTECHNIQUE (0x9B)
#define IDCODE_MASK_MITSUBISHI (0x1C)
#define IDCODE_MASK_LUCENT (0x9D)
#define IDCODE_MASK_EXEL (0x9E)
#define IDCODE_MASK_ATMEL (0x1F)
#define IDCODE_MASK_STMICROELECTRONICS (0x20)
#define IDCODE_MASK_LATTICE_SEM (0xA1)
#define IDCODE_MASK_NCR (0xA2)
#define IDCODE_MASK_WAFER_SCALE_INTEGRATION (0x23)
#define IDCODE_MASK_IBM (0xA4)
#define IDCODE_MASK_TRISTAR (0x25)
#define IDCODE_MASK_VISIC (0x26)
#define IDCODE_MASK_INTL_CMOS_TECHNOLOGY (0xA7)
#define IDCODE_MASK_SSSI (0xA8)
#define IDCODE_MASK_MICROCHIPTECHNOLOGY (0x29)
#define IDCODE_MASK_RICOH_LTD (0x2A)
#define IDCODE_MASK_VLSI (0xAB)
#define IDCODE_MASK_MCRON_TECHNOLOGY (0x2C)

#pragma endregion

#pragma region dump methods
static inline void dumpStart();
static inline void dumpByte(uint8_t b);
static inline void dumpEnd();
#pragma endregion

// Once you have found the JTAG pins you can define
// the following to allow for the boundary scan and
// irenum functions to be run. Define the values
// as the index for the pins[] array of the found
// jtag pin:
//#define	TCK                      3
//#define	TMS                      0
//#define	TDO                      2
//#define	TDI                      1
//#define	TRST                     4

// Pattern used for scan() and loopback() tests
#define PATTERN_LEN 64
// Use something random when trying find JTAG lines:
static char pattern[PATTERN_LEN] = "0110011101001101101000010111001001";
// Use something more determinate when trying to find
// length of the DR register:
//static char pattern[PATTERN_LEN] = "1000000000000000000000000000000000";

// Max. number of JTAG enabled chips (MAX_DEV_NR) and length
// of the DR register together define the number of
// iterations to run for scan_idcode():
#define MAX_DEV_NR 8
#define IDCODE_LEN 32

// Target specific, check your documentation or guess
#define SCAN_LEN 1890 // used for IR enum. bigger the better
//#define IR_LEN                   8
// IR registers must be IR_LEN wide:
#define IR_IDCODE "01100" // always 011
#define IR_SAMPLE "10100" // always 101
#define IR_PRELOAD IR_SAMPLE

/*
 * END USER DEFINITIONS
 */

// TAP TMS states we care to use. NOTE: MSB sent first
// Meaning ALL TAP and IR codes have their leftmost
// bit sent first. This might be the reverse of what
// documentation for your target(s) show.
#define TAP_RESET "11111" // looping 1 will return \
                          // IDCODE if reg available
#define TAP_SHIFTDR "111110100"
#define TAP_SHIFTIR "1111101100" // -11111> Reset -0> Idle -1> SelectDR \
                                 // -1> SelectIR -0> CaptureIR -0> ShiftIR

// Ignore TCK, TMS use in loopback check:
#define IGNOREPIN 0xFFFF
// Flags configured by UI:
uint8_t debug = 0;
uint8_t DELAY = 0;
long DELAYUS = 5000; // 5 Milliseconds
uint8_t PULLUP = 0;

#define NAND_ALE (1 << 4)
#define NAND_CLE (1 << 5)
#define NAND_WP (1 << 3)
#define NAND_WE (1 << 2)
#define NAND_RE (1 << 1)
#define NAND_CE (1 << 0)
/*! \brief READY/BUSY input definition. */
#define NAND_RB (1 << 6)



/* pin mapping: A31~A0
 * 1111 1111 1111 1111 1111 1111 1111 1111
 *    PORTF|    PORTD|    PortC|    PortB|
 * 0-7 -> B
 * 8-15 -> C
 * 16-23 -> D
 * 24-31 -> F
 * 
 * pin mapping: DATA15~DATA0
 * 1111 1111 1111 1111
 *    PortJ|    PortH|
 * 0-7 -> H
 * 8-15 -> J
 * 
 * pin mapping: Control 7~0
 * PK0 = CE#
 * PK1 = OE#
 * PK2 = WE#
 * PK3 = WP#/ACC
 * PK4 = RESET#
 * PK5 = BYTE#
 * PK6 = RY#/BY#
 * 
 * 7SEG:
 * PA1 = A
 * PA2 = B
 * PA3 = C
 * PA4 = D
 * PA5 = E
 * PA6 = F
 * PA7 = G
 * 13-15, 16:30- */

#endif
