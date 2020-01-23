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
#define DUMP_BEGIN_MARKER "---- BEGIN DUMP ----" NL 
#define DUMP_END_MARKER "---- END DUMP ----" NL 

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

#define CHAR_BELL 0x07
#define CHAR_TAB 0x09
#define CHAR_XOFF 0x13
#define CHAR_XON 0x11
#define CHAR_BREAK 0x03
#define CHAR_BACKSPACE 0x08
#define CHAR_DEL 0x7f
#define CHAR_CR 0x0D
#define CHAR_LF 0x0A

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
#define IDCODE_MASK_MICRON_TECHNOLOGY (0x2C)
#define IDCODE_MASK_MARVELL_SEMICONDUCTORS  (0xE9)

#pragma endregion

#pragma region ONFI
#define ONFI_CMD_READ_ID 0x90
#define ONFI_CMD_RESET 0xFF
#define ONFI_CMD_BLOCK_ERASE_CYCLE_1 0x60
#define ONFI_CMD_BLOCK_ERASE_CYCLE_2 0xd0
#define ONFI_CMD_PAGE_CACHE_PROGRAM_CYCLE_1 0x80
#define ONFI_CMD_PAGE_CACHE_PROGRAM_CYCLE_2 0x10
#define ONFI_CMD_READ_STATUS 0x70
#define ONFI_CMD_READ_PARAMETER_PAGE 0xEC
#define ONFI_CMD_READ_CYCLE_1 0x00
#define ONFI_CMD_READ_CYCLE_2 0x30

#define ONFI_FEATURE_16_BIT_BUS (1<<0)
#define ONFI_FEATURE_MULTIPLE_LUNS (1<<1)
#define ONFI_FEATURE_NON_SEQ_PAGE_PROG (1<<2)
#define ONFI_FEATURE_MULTI_PLANE_PROG_ERASE (1<<3)
#define ONFI_FEATURE_ODD_EVEN_PAGE_COPYBACK (1<<4)
#define ONFI_FEATURE_NV_DDR (1<<5)
#define ONFI_FEATURE_MULTI_PLANE_READ (1<<6)
#define ONFI_FEATURE_EXTENDED_PARAMETER_PAGE (1<<7)
#define ONFI_FEATURE_PROGRAM_PAGE_REGISTER_CLEAR_ENHANCEMENT (1<<8)
#define ONFI_FEATURE_EZ_NAND (1<<9)
#define ONFI_FEATURE_NV_DDR2 (1<<10)
#define ONFI_FEATURE_VOLUME_ADDRESSING (1<<11)
#define ONFI_FEATURE_EXT_VPP (1<<12)
#define ONFI_FEATURE_NV_DDR3 (1<<13)
#define ONFI_FEATURE_ZQ_CALIBRATION (1<<14)
#define ONFI_FEATURE_PACKAGE_ELECTRICAL_SPEC (1<<15)

typedef struct {
    uint16_t mask;
    char* description;
} onfi_feature_description_t;

onfi_feature_description_t onfi_feature_descriptions[] = {
    {ONFI_FEATURE_16_BIT_BUS, "16 bit bus"},
    {ONFI_FEATURE_MULTIPLE_LUNS, "Multiple LUNs"},
    {ONFI_FEATURE_NON_SEQ_PAGE_PROG, "Non-sequential page programming"},
    {ONFI_FEATURE_ODD_EVEN_PAGE_COPYBACK, "Odd to even page Copyback"},
    {ONFI_FEATURE_MULTI_PLANE_PROG_ERASE, "Multi-plane program and erase operations"},
    {ONFI_FEATURE_MULTI_PLANE_READ, "Multi-plane read operations"},
    {ONFI_FEATURE_NV_DDR, "NV-DDR"},
    {ONFI_FEATURE_NV_DDR2, "NV-DDR2"},
    {ONFI_FEATURE_NV_DDR3, "NV-DDR3"},
    {ONFI_FEATURE_EXTENDED_PARAMETER_PAGE, "Extended parameter page"},
    {ONFI_FEATURE_PROGRAM_PAGE_REGISTER_CLEAR_ENHANCEMENT, "Program page register clear enhancement"},
    {ONFI_FEATURE_EZ_NAND, "EZ NAND"},
    {ONFI_FEATURE_VOLUME_ADDRESSING, "Volume addressing"},
    {ONFI_FEATURE_EXT_VPP, "External Vpp"},
    {ONFI_FEATURE_ZQ_CALIBRATION, "ZQ Calibration"},
    {ONFI_FEATURE_PACKAGE_ELECTRICAL_SPEC, "Package Electrical Specification"},
    {0, NULL}
};

#pragma endregion

#pragma region dump methods
static inline void dumpStart();
static inline void dumpByte(uint8_t b);
static inline void dumpEnd();
static inline int isOperationCanceled();
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
uint8_t jtagDelay = 0;
long jtagDelayTime = 5000; // 5 Milliseconds
long nandDelayTime = 0;
uint8_t PULLUP = 0;
uint8_t cancelCurrentOperation = 0;
uint8_t dumpFast = 0;

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
