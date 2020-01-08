# 
# Copyright (C): SEC Consult Unternehmensberatung GmbH, 2019         \n
# Web:           https://sec-consult.com/                            \n
#                                                                    \n
# This Source Code Form is subject to the terms of the Mozilla Public\n
# License, v. 2.0. If a copy of the MPL was not distributed with this\n
# file, You can obtain one at https://mozilla.org/MPL/2.0/.          \n
# 
# @authors Wolfgang Ettlinger
#

import sys
import serial
import time

ser = serial.Serial(sys.argv[1], int(sys.argv[2]))
# send BREAK,BREAK,BREAK,"reset",CR
for c in (b'\x03\x03\x03reset\r'):
    time.sleep(.05)
    ser.write(c)
    ser.flush()

ser.close()
