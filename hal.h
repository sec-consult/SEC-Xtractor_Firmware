/**
 * @file hal.h
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
#ifndef _HAL_H
#define _HAL_H

#include <stdint.h>

#if defined(XTRACTOR_ARCH_TEST)
#include "hal-test/definitions.h"
#elif defined(XTRACTOR_ARCH_AVR)
#include "hal-avr/definitions.h"
#else
#error "XTRACTOR_ARCH not defined"
#endif

#pragma region generic device functionality
static inline uint16_t readVoltage(void);
static inline void activateInterrupts();
static inline void disableJTAG(void);
static inline void initADC(void);
static inline void initSystemClock(void);
static inline void resetDevice(void);
#pragma endregion

#pragma region GPIO
static inline void portOut(xtractor_port_t *port, uint8_t b);
static inline void portSetDirection(xtractor_port_t *port, uint8_t mask);
#pragma endregion

#pragma region UART functionality
static inline uint8_t uartHasInput(void);
static inline uint8_t uartGetInput(void);
static inline void uartInit(void);
static inline void uartprintf(char *fmt, ...);
static inline void uartWriteChar(char c);
static inline void uartMitMInit(void);
static inline void uartMitMWriteChar(char c);
static inline void uartMitMWriteString(char *s);
static inline void uartWriteString(char *s);
#pragma endregion

#pragma region seven segment display
static inline void sevensegWriteNumber(uint16_t printed_value);
static inline void sevensegShowProgressBar(void);
static inline void sevensegInit();
#pragma endregion

#pragma region SPI
void spiConfigurePins();
void spiGDreadID();
uint8_t spiRead();
#pragma endregion

#pragma region NAND
typedef struct {
    uint8_t signature[4];
    uint16_t rev_num;
    uint16_t feature_support;
    uint16_t opt_cmd_support;
    uint8_t reserved1[22];
    uint8_t vendor[12];
    uint8_t model[20];
    uint8_t vendor_jedec;
    uint16_t date;
    uint8_t reserved2[13];
    uint32_t bytes_per_page;
    uint16_t spare_bytes_per_page;
    uint32_t data_bytes_per_partial_page;
    uint16_t spare_bytes_per_partial_page;
    uint32_t pages_per_block;
    uint32_t block_per_logic_unit;
    uint8_t logical_units;
    uint8_t address_cycles;
    uint8_t bits_per_cell;
    uint16_t max_bad_block_per_lun;
    uint16_t block_endurance;
    uint8_t guaranteed_valid_blocks_at_beginning;
    uint16_t block_endurance_for_guaranteed_valid_block;
    uint8_t num_of_programs_per_page;
    uint8_t partial_programming_attributes;
    uint8_t num_of_bits_ecc_correctability;
    uint8_t interleaved_addressing;
    uint8_t interleaved_operation_attributes;
    uint8_t reserved3[13];
    uint8_t io_pin_capacitance;
    uint16_t timing_mode_support;
    uint16_t program_cache_timing_mode_support;
    uint16_t max_page_program_time;
    uint16_t max_block_erase_time;
    uint16_t max_page_read_time;
    uint16_t min_change_column_setup_time;
    uint8_t reserved4[23];
    uint16_t vendor_rev_num;
    uint8_t vendor_specific[88];
    uint16_t integrity_crc;
} onfi_param_page_t;

typedef struct{
    onfi_param_page_t param_page[3];
} onfi_param_page_full_t;
static inline void nandReset();
static inline void nandInit();
static inline void nandReadONFI(uint8_t result[]);
static inline void nandReadChipID(uint8_t result[]);
static inline void onfiWriteData(onfi_param_page_t *params, uint16_t pattern,uint8_t write_ecc);
static inline void onfiReadParameters(onfi_param_page_full_t *params);
static inline void onfiReadData(onfi_param_page_t *params, uint8_t read_ecc);
static inline void initOnfiParam();
static inline void onfiEraseAll(onfi_param_page_t *params, uint8_t enable_progress);
#pragma endregion

#pragma region NOR
static void norResetFlash();
static inline void norModeWord();
static inline void norModeByte();
static inline void norStartReadData();
static inline void norEndReadData();
static inline uint8_t norRead8DataBits(uint32_t address);
static inline uint16_t norRead16DataBits(uint32_t address);
//static inline void norWrite8DataBits(uint32_t address, uint8_t data);
//static inline void norEraseChip();
#pragma endregion

#pragma region UART_SCANNER
typedef struct {
    uint32_t pin_changes;
    uint64_t last_change;
    uint64_t time_high;
    uint64_t time_low;
    uint32_t shortest_lowtime;
}scanner_result_t;
static inline void initScan(uint8_t pin_num);
static volatile inline scanner_result_t* getChanges(void);
static volatile inline uint32_t getShortestLowTime(void);
#pragma endregion

#pragma region TIMER
static inline void initTimer(void);
static inline uint64_t getTime(void);
#pragma endregion

#pragma region JTAG
static inline uint8_t jtagReadPin(uint8_t pin);
static inline void jtagWritePin(uint8_t pin, uint8_t level);
void jtagInitPins(int tck, int tms, int tdi, int ntrst, uint8_t pinslen);
void jtagTapState(char jtagTapState[], int tck, int tms);
static void jtagPrintPins(int tck, int tms, int tdo, int tdi, int ntrst);
static void jtagPulseTms(int tck, int tms, int s_tms);
static void jtagPulseTdi(int tck, int tdi, int s_tdi);
uint8_t jtagPulseTdo(int tck, int tdo);
#pragma endregion

#endif
