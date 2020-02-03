/**
 * @file hal-test.c
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
#include "../secxtractor.h"
#include <unistd.h>
#include <stdarg.h>

#include "definitions.h"

static inline uint8_t uartHasInput(void)
{
    return 1;
}

static inline uint8_t uartGetInput(void)
{
    int c = fgetc(stdin);
    if (c == 0x0A)
    {
        c = 0x0D;
    }
    return c;
}

static inline void uartWriteChar(char c)
{
    putc(c, stdout);
}

static inline void uartMitMWriteChar(char c)
{
    putc(c, stdout);
}

static inline void uartMitMWriteString(char *s)
{
    puts(s);
}

static inline void uartWriteString(char *s)
{
    puts(s);
}

static inline void uartprintf(char *fmt, ...)
{
    va_list arglist;
    va_start(arglist, fmt);
    vprintf(fmt, arglist);
    va_end(arglist);
}

static inline void resetDevice(void) {}
static inline uint16_t readVoltage(void) { return 0; }
static inline void activateInterrupts() {}
static inline void disableJTAG(void) {}
static inline void initADC(void) {}
static inline void initSystemClock(void) {}
static inline uint8_t jtagReadPin(uint8_t pin) { return 0; }
static inline void jtagWritePin(uint8_t pin, uint8_t level) {}
static inline void portOut(xtractor_port_t port, uint8_t b) {}
static inline void portSetDirection(xtractor_port_t port, uint8_t mask)
{
    DEBUG("portSetDirection: %s %x", port, mask);
}
static inline void uartInit(void) {}
static inline void uartMitMInit(void) {}
static inline void sevensegWriteNumber(uint16_t printed_value) {}
static inline void sevensegShowProgressBar(void) {}
static inline void sevensegInit() {}
void spiConfigurePins() {}
uint8_t spiRead() { return 0; }
void spiGDreadID() {}
static inline void nandReset() {}
static inline void nandInit() {}
static inline void nandReadONFI(uint8_t result[]) {}
static inline void nandReadChipID(uint8_t result[]) {}
static inline void nandReadData16bit() {}
static inline void nandWriteData16bitTest() {}
static inline void nandReadData8bit() {}
static inline void nandReadData8bitF59L1G81() {}
void jtagTapState(char jtagTapState[], int tck, int tms) {}
void jtagInitPins(int tck, int tms, int tdi, int ntrst, uint8_t pinslen) {}
static void jtagPrintPins(int tck, int tms, int tdo, int tdi, int ntrst) {}
static void jtagPulseTms(int tck, int tms, int s_tms) {}
static void jtagPulseTdi(int tck, int tdi, int s_tdi) {}
uint8_t jtagPulseTdo(int tck, int tdo) { return 0; }
static void norResetFlash() {}
static inline void norModeWord() {}
static inline void norModeByte() {}
static inline void norStartReadData()
{
    usleep(100000);
    DEBUG("norStartReadData");
}
static inline void norEndReadData()
{
    DEBUG("norEndReadData");
}

static inline uint8_t norRead8DataBits(uint32_t address)
{
    return 0xAB;
}

static inline uint16_t norRead16DataBits(uint32_t address)
{
    return 0xABCD;
}
