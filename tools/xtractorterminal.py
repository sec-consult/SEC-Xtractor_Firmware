#!/usr/bin/env python3
# 
# Copyright (C): SEC Consult Unternehmensberatung GmbH, 2020         \n
# Web:           https://sec-consult.com/                            \n
#                                                                    \n
# This Source Code Form is subject to the terms of the Mozilla Public\n
# License, v. 2.0. If a copy of the MPL was not distributed with this\n
# file, You can obtain one at https://mozilla.org/MPL/2.0/.          \n
# 
# @authors Wolfgang Ettlinger
#
import re
import datetime
import time
import tempfile
from binascii import unhexlify
from serial.tools.miniterm import Miniterm, key_description, Transform
import serial
import sys

BAUD = 8000000
EOL = 'lf'
ENCODING = 'UTF-8'
HEADER = r'''
### ### ###
#   #   #    \  /
 #  ##  #     \/ ___ ___       _ ___  _  ___
  # #   #     /\  |  | |  /\  |   |  | | | |
### ### ###  /  \ |  |   /  \ |_  |  |_| |
'''

NL = '\n'
OUTPUT_BOUNDARY = "-----------------------"
OUTPUT_BOUNDARY_THICK = "======================="
DUMP_BEGIN_MARKER = "---- BEGIN DUMP ----"
DUMP_END_MARKER = "---- END DUMP ----"
KB = 1024


class XtractorTransformation(Transform):
    def __init__(self):
        self.buf = ''
        self.destination_file = None
        self.byte_count = 0
        self.start_timestamp = None

    def rx(self, text):
        res = []

        inputlines = (text).split(NL)

        for i in range(len(inputlines) - 1):
            line = inputlines[i]

            if i == 0:
                line = self.buf + line
                self.buf = ''

            if self.destination_file is None:
                res.append(inputlines[i])

            process_out = self.process(line)

            if process_out is not None:
                res.append(process_out)

        self.buf += inputlines[-1]

        if self.destination_file is None:
            res.append(inputlines[-1])
        elif len(res) != 0:
            res.append('')  # force newline

        return NL.join(res)

    def process(self, line):
        line = line.strip(' \r\n')
        if self.destination_file is not None:
            if line in (OUTPUT_BOUNDARY, OUTPUT_BOUNDARY_THICK, DUMP_END_MARKER):
                fname = self.destination_file.name
                self.destination_file.close()
                self.destination_file = None
                return "Dump saved to '{0}' ({1}KB)".format(fname, self.byte_count / KB)
            else:
                data = unhexlify(line.replace(' ', ''))
                self.destination_file.write(data)
                old_byte_count = self.byte_count
                self.byte_count += len(data)

                if int(old_byte_count / 100 / KB) != int(self.byte_count / 100 / KB):
                    return "Received {0:,.0f} KB at {1:.1f} KB/s".format(
                        self.byte_count / KB,
                        self.byte_count / KB / (time.time() - self.start_timestamp))

        elif line == DUMP_BEGIN_MARKER:
            self.destination_file = tempfile.NamedTemporaryFile(delete=False, dir='.', 
                prefix='dump_'+re.sub('[^0-9-]', '_', datetime.datetime.now().isoformat()), 
                suffix='.xtractor')
            self.byte_count = 0
            self.start_timestamp = time.time()
            return "Receiving dump ('{0}')...".format(self.destination_file.name)


class XtractorMiniterm(Miniterm):
    def update_transformations(self):
        super().update_transformations()

        self.rx_transformations.append(XtractorTransformation())


def main(args):
    if len(args) != 1:
        print('USAGE: xtractorterminal.py [serial device]')
        return -1

    device = args[0]

    serial_instance = serial.serial_for_url(
        device, BAUD, parity='N', rtscts=False, xonxoff=True)

    miniterm = XtractorMiniterm(serial_instance, echo=False,
                                eol=EOL, filters=['default'])
    miniterm.exit_character = chr(0x1d)
    miniterm.menu_character = chr(0x14)
    miniterm.raw = False
    miniterm.set_rx_encoding(ENCODING)
    miniterm.set_tx_encoding(ENCODING)

    sys.stderr.write(HEADER)
    sys.stderr.write('--- Miniterm (XTractor) on {p.name}  {p.baudrate},{p.bytesize},{p.parity},{p.stopbits} ---\n'.format(
        p=miniterm.serial))
    sys.stderr.write('--- Quit: {} | Menu: {} | Help: {} followed by {} ---\n'.format(
        'Ctrl+[Key next to return, en:"]", de:"+"]',
        key_description(miniterm.menu_character),
        key_description(miniterm.menu_character),
        key_description('\x08')))

    miniterm.start()
    miniterm.serial.write(b'\x03')  # send CTRL+C
    try:
        miniterm.join(True)
    except KeyboardInterrupt:
        pass
    sys.stderr.write('\n--- exit ---\n')
    miniterm.join()
    miniterm.close()


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
