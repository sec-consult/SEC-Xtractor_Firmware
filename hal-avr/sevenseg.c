/**
 * @file sevenseg.c
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

/**
 * @brief This function enables a segment and disables the others.
 */

static inline void enableSegment(int n)
{
	PORTE.OUTSET = 0x13;
	switch (n)
	{
	case 0:
		PORTE.OUTCLR = 0x01;
		break;
	case 1:
		PORTE.OUTCLR = 0x02;
		break;
	case 2:
		PORTE.OUTCLR = 0x10;
		break;
	default:
		PORTE.OUTCLR = 0x00;
		break;
	}	
}

/**
 * @brief This function depicts the known numbers on the 7-segment.
 * @param number The input variable number contains the number depicted on the 7-segment.
 */
static inline void sevensegShowDec(uint8_t number)
{
	switch (number)
	{
	case 0:
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0x7E;
		break;
	case 1:
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0x0C;
		break;
	case 2:
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xB7;
		break;
	case 3:
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0x9E;
		break;
	case 4:
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xCC;
		break;
	case 5:
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xDA;
		break;
	case 6:
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xFA;
		break;
	case 7:
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0x0E;
		break;
	case 8:
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xFE;
		break;
	case 9:
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xDE;
		break;
	default:
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xFE;
	}
}


/**
 * @brief This function depicts the known characters on the displays.
 */
static inline void sevensegShowChar(char chr)
{
	switch (chr)
	{
	case '0':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0x7E;
		break;
	case '1':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0x0C;
		break;
	case '2':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xB7;
		break;
	case '3':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0x9E;
		break;
	case '4':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xCC;
		break;
	case '5':
	case 'S':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xDA;
		break;
	case '6':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xFA;
		break;
	case '7':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0x0E;
		break;
	case '8':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xFE;
		break;
	case '9':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xDE;
		break;
	case 'A':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xEE;
		break;
	case 'a':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xEE;
		break;
	case 'B':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xF8;
		break;
	case 'b':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xF8;
		break;
	case 'C':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0x72;
		break;
	case 'c':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0x72;
		break;
	case 'D':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xBC;
		break;
	case 'd':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xBC;
		break;
	case 'E':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xF2;
		break;
	case 'e':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xF2;
		break;
	case 'F':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xE2;
		break;
	case 'f':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xE2;
		break;
	case 'L':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0x71;
		break;
	case 'l':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0x71;
		break;
	case 'O':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xB8;
		break;
	case 'o':
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xB8;
		break;
	default:
		DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
		DEVICE_FUNCTION_PORT.OUTCLR = 0xFE;
	}
}

static inline void sevensegWriteNumber(uint16_t printed_value)
{
	enableSegment(0);
	sevensegShowDec(printed_value / 1000);
	DELAY_MS(4);
	enableSegment(1);
	sevensegShowDec(printed_value / 100 - ((printed_value / 1000) * 10));
	DELAY_MS(4);
	enableSegment(2);
	sevensegShowDec(printed_value / 10 - ((printed_value / 100) * 10));
	DELAY_MS(4);
}

/**
 * @brief This function depicts a progress bar on the displays.
 */
static inline void sevensegShowProgressBar(void)
{
	DEVICE_FUNCTION_PORT.OUTSET = 0xFE;
	DEVICE_FUNCTION_PORT.OUTCLR = 0xB8;

	PORTE.OUTSET = 0x13;
	PORTE.OUTCLR = 0x13;
}

/**
 * @brief This function initializes the 7 segment elements.
 * The direction is set to output for 7 of 8 pins (0xFE).
 * The last pin is used as measurement pin for the current output voltage.
 * @param selftest_flag This variable indicates whether a self test should be performed or not.
 */
static inline void sevensegInit()
{
	DEVICE_FUNCTION_PORT.OUTCLR = 0xFE;
	DEVICE_FUNCTION_PORT.DIRSET = 0xFE;
	PORTE.OUTCLR = 0x13;
	PORTE.DIRSET = 0x13;
	
#if SELF_TEST == 1
	/* self test */

	sevensegShowChar('0'); _delay_ms(200);
	sevensegShowChar('1'); _delay_ms(200);
	sevensegShowChar('2'); _delay_ms(200);
	sevensegShowChar('3'); _delay_ms(200);
	sevensegShowChar('4'); _delay_ms(200);
	sevensegShowChar('5'); _delay_ms(200);
	sevensegShowChar('6'); _delay_ms(200);
	sevensegShowChar('7'); _delay_ms(200);
	sevensegShowChar('8'); _delay_ms(200);
	sevensegShowChar('9'); _delay_ms(200);
	sevensegShowChar('A'); _delay_ms(200);
	sevensegShowChar('B'); _delay_ms(200);
	sevensegShowChar('C'); _delay_ms(200);
	sevensegShowChar('D'); _delay_ms(200);
	sevensegShowChar('E'); _delay_ms(200);
	sevensegShowChar('F'); _delay_ms(200);
#endif
}
