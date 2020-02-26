/**
 * @file jtag.c
 *
 * Copyright (C): SEC Consult Unternehmensberatung GmbH, 2019         \n
 * Web:           https://sec-consult.com/                            \n
 * Notice:        This code is based on the project JTAGenum from Nathan Andrew Fain\n
 *                It is available under https://github.com/cyphunk/JTAGenum\n
 * This Source Code Form is subject to the terms of the Mozilla Public\n
 * License, v. 2.0. If a copy of the MPL was not distributed with this\n
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.          \n
 * 
 * @authors Thomas Weber, Wolfgang Ettlinger
 */

static inline void setPinMode(uint8_t pin, uint8_t inout) //inout = 1 -> output enabled
{

}

static inline void jtagWritePin(uint8_t pin, uint8_t level)
{

}

static inline uint8_t jtagReadPin(uint8_t pin)
{
    return 0;
}

/*****  JTAG Functionality from JTAGenum *****/
/*
 * Set the JTAG TAP state machine
 */
void jtagTapState(char jtagTapState[], int tck, int tms)
{

}

void jtagInitPins(int tck, int tms, int tdi, int ntrst, uint8_t pinslen)
{

}

static void jtagPrintPins(int tck, int tms, int tdo, int tdi, int ntrst)
{

}

static void jtagPulseTms(int tck, int tms, int s_tms)
{

}

static void jtagPulseTdi(int tck, int tdi, int s_tdi)
{

}

uint8_t jtagPulseTdo(int tck, int tdo)
{

}
