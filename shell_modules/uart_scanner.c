/**
 * @file uart_scanner.c
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
 
#include "../secxtractor.h"
#include "../hal.h"

/**
 * @brief Executes the UART Scanner module and prints the results
 */
void cmdUartScanner(char *arguments)
{
    #ifdef ESCAPE_CODE_SUPPORTED
        CLEAN_SCREEN()
    #endif

    //check for valid pinslen
    if(pinslen == 0){
        uartWriteString("Please provide a pinlen first!"NL);
        return;
    }

    //PORTQ.DIRSET = PIN3_bm;
    initTimer();
    initScan(pinslen);
    while(!isOperationCanceled()){
        volatile scanner_result_t* result = getChanges();

        //uartprintf("Pin:\tCnt:\tHigh(%%)\tBaud"NL);
        uartprintf("Pin:\tCnt:\tHigh(%%)"NL);
        uartWriteString(OUTPUT_BOUNDARY);
        for(uint8_t i=0; i<pinslen; i++){
            uint8_t high = result[i].time_high/((float)result[i].time_high+result[i].time_low)*100;
            ///TODO: Work on Autobaud feature 
            //uint32_t baud = result[i].shortest_lowtime;
            //uartprintf("A%u:\t%"PRIu32 "\t%d%%\t%"PRIu32 NL,i,result[i].pin_changes, high, 32000000/getShortestLowTime());
            uartprintf("A%u:\t%"PRIu32 "\t%d%%"NL,i,result[i].pin_changes, high);
        }
        _delay_ms(1000);
        #ifdef ESCAPE_CODE_SUPPORTED
            CLEAN_SCREEN()
        #endif
    }
}
