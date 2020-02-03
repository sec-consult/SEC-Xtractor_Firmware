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

typedef char* xtractor_port_t;

#define NOP {}
#define DELAY_MS(ms) {}
#define DELAY_US(us) {}

#define NAND_IO_PORT0 	"NAND_IO_PORT0"
#define NAND_IO_PORT1 	"NAND_IO_PORT1"
#define NAND_CTRL_PORT 	"NAND_CTRL_PORT"
#define NOR_ADDR_PORT0	"NOR_ADDR_PORT0"
#define NOR_ADDR_PORT1	"NOR_ADDR_PORT1"
#define NOR_ADDR_PORT2	"NOR_ADDR_PORT2"
#define NOR_ADDR_PORT3	"NOR_ADDR_PORT3"
#define NOR_DATA_PORT0	"NOR_DATA_PORT0"
#define NOR_DATA_PORT1	"NOR_DATA_PORT1"
#define NOR_CTRL_PORT	"NOR_CTRL_PORT"

#define DEBUG(x, ...) fprintf(stderr, "DEBUG: " x NL , ##__VA_ARGS__ )

#define NL "\n"

#endif
