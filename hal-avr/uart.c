/**
 * @file uart.c
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
#include <stdarg.h>

/*! @brief Interrupt variable uart_rx_byte. When a byte is received via the main UART, uart_rx_byte is set to "1".*/
volatile uint8_t uart_rx_byte = 0;
volatile uint8_t uart_has_input = 0;

/*! @brief Interrupt variable terminal_output. When a characters are outputted via the main UART, terminal_output is set to "1".*/
volatile uint8_t terminal_output = 1;

/**
 * @brief Interrupt service routine for receive complete on UARTE.
 */
ISR(USARTE0_RXC_vect)
{
	uart_rx_byte = USARTE0.DATA;
	uart_has_input = 1;
	sevensegShowChar('A');
}

/**
 * @brief Interrupt service routine for transmit complete on UARTE.
 */
ISR(USARTE0_TXC_vect)
{
	terminal_output = 0;
	sevensegShowChar('B');
}

static inline void checkUartFlowControl()
{
	if(uart_has_input && uart_rx_byte == CHAR_XOFF){
		uart_has_input = 0; // consume XOFF char
		
		// XOFF (or any other char) causes output to continue
		while(!uart_has_input) {}
		uart_has_input = 0;
	}
}

inline uint8_t uartHasInput()
{
	uint8_t result = 0;
	if (uart_has_input && uart_rx_byte != CHAR_XOFF)
	{
		result = 1;
	}
	return result;
}

inline uint8_t uartGetInput(void)
{
	uart_has_input = 0;
	return uart_rx_byte;
}

inline uint8_t uartPeekInput(void)
{
	return uart_rx_byte;
}

/**
 * @brief This function initializes the main communication UART port.
 * This port is used as primary communication channel to the user and has a speed of 4 Megabaud.
 */
static inline void uartInit(void)
{
	/*4M Baudrate */
	USARTE0.BAUDCTRLB = 0x00;
	USARTE0.BAUDCTRLA = 0x00;

	/*activate send and receive and activate double speed*/
	USARTE0.CTRLB = USART_TXEN_bm | USART_RXEN_bm | USART_CLK2X_bm;
	//	USARTE0.BAUDCTRLB = 0xF0;
	//	USARTE0.BAUDCTRLA = 0x21; //-> 9600
	//	USARTE0.CTRLB = USART_TXEN_bm | USART_RXEN_bm;	//activate send and receive
	USARTE0.CTRLC = USART_CHSIZE_8BIT_gc; //change size to 8 bit
	PORTE.DIR = 0xE8;					  //set pc3 to output
	/* Interrupt Section */
	//USARTE0.STATUS = USART_RXCIF_bm | USART_TXCIF_bm; //set receive interrupt enable flag
	USARTE0.CTRLA = 0x30;				  //set receive complete interrupt level to 3 (high)
}

/**
 * @brief This function sends one character to be outputted on the main communication UART port.
 * @param c The variable c contains the character that is send to the output.
 */
static inline void uartWriteChar(char c)
{
	// before writing data, check that receiver wants data
	checkUartFlowControl(); 

	PORTQ.DIRSET = PIN3_bm;
	PORTQ.OUTSET = PIN3_bm;
	//while(!(USARTE0.STATUS & USART_DREIF_bm));
	USARTE0.DATA = c; //send data to uart tx
	//terminal_output = 1;
	while(!(USARTE0.STATUS & USART_TXCIF_bm));
	USARTE0.STATUS|= USART_TXCIF_bm;
	PORTQ.OUTCLR = PIN3_bm;
	//while (terminal_output);
}


/**
 * @brief This function is a prototype that can be used for man-in-the-middle UART attacks.
 */
static inline void uartMitMInit(void)
{
	/* 9600 baud */
	//	USARTC0.BAUDCTRLA = 0xC;
	//	USARTC0.BAUDCTRLB = 0x40;
	/* 115200 baud*/
	USARTC0.BAUDCTRLA = 0x83;
	USARTC0.BAUDCTRLB = 0xD0;					   // -3 twos complement
	USARTC0.CTRLB = USART_TXEN_bm | USART_RXEN_bm; //	//activate send and receive

	USARTC0.CTRLC = USART_CHSIZE_8BIT_gc; //change size to 8 bit
	PORTC.DIR = 0xFB;					  //set pc3 to output
	/* Interrupt Section */
	USARTC0.STATUS = USART_RXCIF_bm | USART_TXCIF_bm; //set receive interrupt enable flag
	USARTC0.CTRLA = 0x3C;							  //set receive complete interrupt level to 3 (high)
}

static inline void uartMitMWriteChar(char c)
{
	USARTC0.DATA = c; //send data to uart tx
}

static inline void uartMitMWriteString(char *s)
{
	uint16_t len = (uint16_t)strlen((const char *)s);

	for (uint16_t i = 0; i < len; i++)
	{
		uartMitMWriteChar(s[i]);
	}
}

static inline void uartWriteString(char *s)
{
	uint16_t len = (uint16_t)strlen((const char *)s);

	for (uint16_t i = 0; i < len; i++)
	{
		uartWriteChar(s[i]);
	}
}

static inline void uartprintf(char *fmt, ...)
{
    va_list arglist;
	va_start(arglist, fmt);
    int size = vsnprintf(NULL, 0, fmt, arglist);
    char str[size + 1];

    vsnprintf(str, size + 1, fmt, arglist);
    uartWriteString(str);

	va_end(arglist);
}
