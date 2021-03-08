/**
 * @file nand_pins.h
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

#define NAND_ALE_HIGH()			{ NAND_CTRL_PORT.OUTSET = NAND_ALE; }
#define NAND_ALE_LOW()			{ NAND_CTRL_PORT.OUTCLR = NAND_ALE; }
#define NAND_CLE_HIGH() 		{ NAND_CTRL_PORT.OUTSET = NAND_CLE; }
#define NAND_CLE_LOW()			{ NAND_CTRL_PORT.OUTCLR = NAND_CLE; }
#define NAND_WP_HIGH()			{ NAND_CTRL_PORT.OUTSET = NAND_WP; }
#define NAND_WP_LOW()			{ NAND_CTRL_PORT.OUTCLR = NAND_WP; }
#define NAND_WE_HIGH()			{ NAND_CTRL_PORT.OUTSET = NAND_WE; }
#define NAND_WE_LOW()			{ NAND_CTRL_PORT.OUTCLR = NAND_WE; }
#define NAND_RE_HIGH()			{ NAND_CTRL_PORT.OUTSET = NAND_RE; }
#define NAND_RE_LOW()			{ NAND_CTRL_PORT.OUTCLR = NAND_RE; }
#define NAND_CE_HIGH()			{ NAND_CTRL_PORT.OUTSET = NAND_CE; }
#define NAND_CE_LOW()			{ NAND_CTRL_PORT.OUTCLR = NAND_CE; }
#define NAND_RB_PULLUP_HIGH() 	{ NAND_CTRL_PORT.OUTSET = NAND_RB; }
#define NAND_IO_PORT0_OUT(cmd)	{ NAND_IO_PORT0.OUT = cmd; }
#define NAND_IO_PORT1_OUT(cmd)	{ NAND_IO_PORT1.OUT = cmd; }
#define NAND_IO_PORT0_DIR_IN()	{ NAND_IO_PORT0.DIR = 0x00; }	
#define NAND_IO_PORT1_DIR_IN()	{ NAND_IO_PORT0.DIR = 0x00; }
#define NAND_IO_PORT0_DIR_OUT()	{ NAND_IO_PORT0.DIR = 0xFF; }
#define NAND_IO_PORT1_DIR_OUT()	{ NAND_IO_PORT1.DIR = 0xFF; }	
#define	NAND_IO_PORT0_IN		{ NAND_IO_PORT0.IN }
#define	NAND_IO_PORT1_IN		{ NAND_IO_PORT1.IN }
#define NAND_RB_READ() (((NAND_CTRL_PORT.IN & NAND_RB) == NAND_RB) ? 1 : 0)

#define NAND_DELAY() {for(int i=0; i<nandDelayTime; i++){NOP;}}