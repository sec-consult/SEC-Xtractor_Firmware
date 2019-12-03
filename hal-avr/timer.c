/**
 * @file timer.c
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


 static volatile uint64_t overflow_val = 0;

 static inline void initTimer(void)
 {
    /*timer test block */
	TCC0.CNTL = 0;
	TCC0.CNTH = 0;
    TCC0.INTCTRLA = TC_OVFINTLVL_MED_gc;
    PMIC.CTRL |= PMIC_MEDLVLEN_bm;
    TCC0.CTRLA = TC_CLKSEL_DIV1_gc;
    sei();
 }

static inline uint64_t getTime(void)
{
    uint16_t cnt = (((uint16_t)TCC0.CNTH)<<8) | TCC0.CNTL;
    return overflow_val + cnt;
}

 ISR(TCC0_OVF_vect)
 {
     overflow_val += 0x10000;
 }
