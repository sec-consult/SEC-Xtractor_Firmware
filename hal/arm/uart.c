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
#include <stdio.h>
#include "stm32f446x.h"
#include "arm-helpers.h"

/*! @brief Interrupt variable uart_rx_byte. When a byte is received via the main UART, uart_rx_byte is set to "1".*/
volatile uint8_t uart_rx_byte = 0;
volatile uint8_t uart_has_input = 0;

/*! @brief Interrupt variable terminal_output. When a characters are outputted via the main UART, terminal_output is set to "1".*/
volatile uint8_t terminal_output = 1;

/**
 * @brief Interrupt service routine for receive complete on UART3.
 */
void USART3_IRQHandler(void)
{
	uart_rx_byte = USART3->DR;
	uart_has_input = 1;
}
/**
 * @brief Interrupt service routine for receive complete on UART3.
 */
void USART1_IRQHandler(void)
{
	uart_rx_byte = USART1->DR;
	uart_has_input = 1;
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

	//PA9=TX, PA10=RX
	//Activate AHB1 clock for UART
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

	__NOP();

	//1, 5 == 4000000 baud
	//1,3 OVER8 = 8000000 baud
	USART1->BRR = 1 << USART_BRR_DIV_Mantissa_Pos | 3<< USART_BRR_DIV_Fraction_Pos;
	GPIOA->OSPEEDR |= (0x3<<GPIO_OSPEEDR_OSPEED9_Pos) | (0x3<<GPIO_OSPEEDR_OSPEED10_Pos);
	GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE10) | GPIO_MODER_MODER10_1;
	GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE9) | GPIO_MODER_MODER9_1; 
	//Set Alternate function AF7 on PD8=TX + PD9=RX
	GPIOA->AFR[1] = (7 << GPIO_AFRH_AFSEL9_Pos) | (7 << GPIO_AFRH_AFSEL10_Pos);
	USART1->CR2 &= USART_CR2_STOP_Msk;      //One Stop bit
	USART1->CR1 |= USART_CR1_RXNEIE;
	USART1->CR1 |=  USART_CR1_UE | USART_CR1_OVER8;
	//Set Hardware Flow control
	GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE11) | GPIO_MODER_MODER11_1;
	GPIOA->MODER = (GPIOA->MODER & ~GPIO_MODER_MODE12) | GPIO_MODER_MODER12_1; 
	GPIOA->AFR[1] = (7 << GPIO_AFRH_AFSEL11_Pos) | (7 << GPIO_AFRH_AFSEL12_Pos);
	USART1->CR3 |= USART_CR3_RTSE;
	USART1->CR3 |= USART_CR3_CTSE;
	//USART1->CR1 |=  USART_CR1_UE;
	USART1->CR1 |= USART_CR1_TE;
	USART1->CR1 |= USART_CR1_RE;
	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_EnableIRQ(USART1_IRQn);


	// //Activate AHB1 clock for UART
	// RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	// RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

	// __NOP();

	// //22, 13== 115200 baud
	// //0, 11 == 4000000 baud
	// USART3->BRR = 22 << USART_BRR_DIV_Mantissa_Pos | 13 << USART_BRR_DIV_Fraction_Pos;
	// GPIOD->MODER = (GPIOD->MODER & ~GPIO_MODER_MODE8) | GPIO_MODER_MODER8_1;
	// GPIOD->MODER = (GPIOD->MODER & ~GPIO_MODER_MODE9) | GPIO_MODER_MODER9_1; 
	// //Set Alternate function AF7 on PD8=TX + PD9=RX
	// GPIOD->AFR[1] = (7 << GPIO_AFRH_AFSEL8_Pos) | (7 << GPIO_AFRH_AFSEL9_Pos);
	// USART3->CR2 &= USART_CR2_STOP_Msk;      //One Stop bit
	// USART3->CR1 |= USART_CR1_RXNEIE;
	// USART3->CR1 |=  USART_CR1_UE;
	// USART3->CR1 |= USART_CR1_TE;
	// USART3->CR1 |= USART_CR1_RE;
	// NVIC_SetPriority(USART3_IRQn, 0);
	// NVIC_EnableIRQ(USART3_IRQn);
}

/**
 * @brief This function sends one character to be outputted on the main communication UART port.
 * @param c The variable c contains the character that is send to the output.
 */
void uartWriteChar(char c)
{
		// before writing data, check that receiver wants data
	checkUartFlowControl(); 
	while(!(USART1->SR & USART_SR_TXE));
	USART1->DR = c;
	
	// // before writing data, check that receiver wants data
	// checkUartFlowControl(); 
	// while(!(USART3->SR & USART_SR_TXE));
	// USART3->DR = c;
}


/**
 * @brief This function is a prototype that can be used for man-in-the-middle UART attacks.
 */
static inline void uartMitMInit(void)
{
							  //set receive complete interrupt level to 3 (high)
}

static inline void uartMitMWriteChar(char c)
{

}

static inline void uartMitMWriteString(char *s)
{
	
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
