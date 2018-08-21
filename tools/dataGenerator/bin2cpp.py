#!/usr/bin/env python3

import binascii
import sys, os
from string import Template

templateStart = Template('''// File auto generated
#pragma once

namespace EmbeddedData
{
const unsigned char $varName[] =
{
''')

templateStop = Template('''
};
const uint32_t ${varName}_size = sizeof($varName)/sizeof(char);
}

''')

inFileName = None
outFileName = None

try:
    inFileName = sys.argv[1]
    outFileName = sys.argv[2]
    varName = sys.argv[3]
except:
    pass

if not inFileName or not outFileName or not varName:
    print("Usage: "+sys.argv[0]+"[binary file to convert] [output file] [variable name]")
    sys.exit(-1)


ifile = open(inFileName, 'rb')
ofile = open(outFileName, 'wb')


ofile.write(templateStart.substitute(varName=varName).encode('ascii'))

idx = 0
for c in ifile.read():
    ofile.write((hex(c)+', ').encode('ascii'))
    idx = idx + 1
    if idx == 20:
        ofile.write(('\n').encode('ascii'))
        idx = 0

ofile.write(templateStop.substitute(varName=varName).encode('ascii'))

ifile.close()
ofile.close()

