/**
 * @file main.c
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
#include "secxtractor.h"

#if defined(XTRACTOR_ARCH_TEST)
#include "hal/test/hal-test.c"
#elif defined(XTRACTOR_ARCH_AVR)
#include "hal/avr/hal-avr.c"
#elif defined(XTRACTOR_ARCH_ARM)
#include "hal/arm/hal-arm.c"
#else
#error "XTRACTOR_ARCH not defined"
#endif

#include "secxtractor.c"
#include "shell.c"
