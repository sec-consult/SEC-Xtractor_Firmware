/**
 * @file spi.c
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

void cmdSpiDump(char *arguments)
{
	uartWriteString(NL "start spi nand dump");
	spiConfigurePins();
	spiGDreadID();
	uartWriteString(NL);
}
