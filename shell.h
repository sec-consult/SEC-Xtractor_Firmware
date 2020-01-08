/**
 * @file shell.h
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

#ifndef _SHELL_H
#define _SHELL_H

#define SHELL_LINE_MAX_LENGTH 256

void shellInit(void);
static void shell(char character);

typedef void (shell_command_callback_t)(char *arguments);

typedef struct {
    char *commandPrefix;
    char *usage;
    char *helptext;
    shell_command_callback_t* callback;
} shell_command_t;

#define SHELL_COMMAND(prefix, callback, usage, helptext) {(prefix), (usage), (helptext), (&callback)}
#define END_OF_COMMANDS() {(NULL), (NULL), (NULL), (NULL)}

#define IS_END_OF_COMMANDS(sc) ((sc)->commandPrefix == NULL)

#endif
