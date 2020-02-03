/**
 * @file adc.c
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

static uint16_t adc_result;
static uint8_t result_flag = 0;

/**
 * @brief Interrupt service routine for analog/digital conversion on channel 0.
 */
ISR(ADCA_CH0_vect)
{

	adc_result = ADCA.CH0.RES;
	result_flag = 1;
}

/**
 * @brief Function for initialization of the ADC.
 */
static inline void initADC(void)
{
	ADCA.REFCTRL |= 0x10;
	/*Single ended  */
	ADCA.CH0.CTRL = ADC_CH_INPUTMODE0_bm;
	/* Set ADC Prescaler (32Mhz/64 = 500khz)*/
	ADCA.PRESCALER = ADC_PRESCALER2_bm;
	/* Enable Conversion Complete Interrupt with high priority*/
	ADCA.CH0.INTCTRL = ADC_CH_INTLVL1_bm | ADC_CH_INTLVL0_bm;
	/* Ensure the conversion complete flag is cleared */
	ADCA.INTFLAGS = ADC_CH0IF_bm;
	/* Enable ADC */
	ADCA.CTRLA = ADC_ENABLE_bm;
	/*ADC Pin as input*/
	DEVICE_FUNCTION_PORT.DIRCLR = 0x01;
	/*Select CH0 Pin0 (no configuration needed here)*/
	ADCA.CH0.MUXCTRL = 0x00;
}

static inline uint16_t readVoltage()
{
	/*Start conversion*/
	ADCA.CH0.CTRL |= 0x80;
	//for 2.7V voltage regulator
	return (adc_result * 14) / 10 + (adc_result / 61) * (adc_result / 123);
}
