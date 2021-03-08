/**
 * @file hal-avr.c
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
#include "../../secxtractor.h"
#include "../../hal.h"
#include "stm32f446x.h"
//#include "arm-helpers.h"
#include "core_cm4.h"
/* 4 Megabit UART */
#include <stddef.h>

// #include "adc.c"
#include "gpio.c"
#include "jtag.c"
#include "nand.c"
#include "nor.c"
// #include "spi.c"
#include "uart.c"
// #include "uart_scanner.c"
// #include "timer.c"

// static inline void portOut(xtractor_port_t *port, uint8_t b)
// {
// 	port->OUT = b;
// }

// static inline void portSetDirection(xtractor_port_t *port, uint8_t mask)
// {
// 	port->DIR = mask;
// }

// static inline void activateInterrupts()
// {

// }

/**
 * @brief Function to disable the JTAG port. This prevents the JTAG brute forcer from detecting itself.
 */
static inline void disableJTAG()
{

}

/**
 * @brief Clock initialization for 32MHz internal oscillator.
 */
static inline void initSystemClock(void)
{
    //Configure main internal regulator output voltage
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    __NOP();
    //uint32_t tmp = RCC->AHB1ENR & RCC_AHB1ENR_GPIOAEN; //used as delay, according to stm32cubemx
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    __NOP();
    PWR->CR |= (0x3 << PWR_CR_VOS_Pos);
    while(!(PWR->CR & PWR_CSR_VOSRDY));
    //Config CPU, AHB, APB Bus clock
    RCC->CR |= RCC_CR_HSION;
    while(!(RCC->CR & RCC_CR_HSIRDY));

    RCC->CR &= RCC_CR_PLLON;
    // PLLM = 8, PLLN = 180, PLLP=2, PLLQ = 2, PLL Source HSI
    RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLM) | (8<<RCC_PLLCFGR_PLLM_Pos);
    RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLR) | (0x2<<RCC_PLLCFGR_PLLR_Pos);
    RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLQ) | (0x2<<RCC_PLLCFGR_PLLQ_Pos);
    RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLN) | (180<<RCC_PLLCFGR_PLLN_Pos);
    RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLQ) | (0x2<<RCC_PLLCFGR_PLLQ_Pos);
   // RCC->PLLCFGR = (8<<RCC_PLLCFGR_PLLM_Pos) | (0x2<<RCC_PLLCFGR_PLLR_Pos) | \
    //(0x2<<RCC_PLLCFGR_PLLQ_Pos) | (180 << RCC_PLLCFGR_PLLN_Pos) | (RCC_PLLCFGR_PLLQ_1);
    RCC->CR |= RCC_CR_PLLON;
    while(!(RCC->CR & RCC_CR_PLLRDY));

    // PWR->CR |= PWR_CR_ODEN;
    // while(!(PWR->CSR & PWR_CSR_ODRDY));
    // PWR->CR |= PWR_CR_ODSWEN;
    // while(!(PWR->CSR & PWR_CSR_ODSWRDY));
    // __NOP();
    // __NOP();
    // __NOP();

    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | FLASH_ACR_LATENCY_8WS; //168MHz with 8 WS
    //first set APB Divider to max to not cross illegal states
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV16 | RCC_CFGR_PPRE2_DIV16;
    RCC->CFGR &= ~RCC_CFGR_HPRE_Msk;
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
    
    //switch to clock
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_SW_Msk) | RCC_CFGR_SW_PLL;
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL));

    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE1_Msk) | RCC_CFGR_PPRE1_DIV4;
    RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_PPRE2_Msk) | RCC_CFGR_PPRE2_DIV2;

    //enable GPIO Clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN  | \
                RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN | \
                RCC_AHB1ENR_GPIOEEN | RCC_AHB1ENR_GPIOFEN |  \
                RCC_AHB1ENR_GPIOGEN | RCC_AHB1ENR_GPIOHEN;
    
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
}

static inline void resetDevice(void)
{
    NVIC_SystemReset();
}
