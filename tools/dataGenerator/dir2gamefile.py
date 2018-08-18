#!/usr/bin/env python3

import sys, os
import glob
import struct

inDirName = None
outFileName = None

try:
    inDirName = sys.argv[1]
    outFileName = sys.argv[2]
except:
    pass

if not inDirName or not outFileName:
    print("Usage: "+sys.argv[0]+"[directory to compress] [output file]")
    sys.exit(-1)

dirs = inDirName.split(' ')

ofile = open(outFileName, 'wb')

# Write header
ofile.write(struct.pack('b', 0x12))
ofile.write(struct.pack('b', 1))

allItems = []
allSize = 4 # 2 header, 1 end of header '0'
start = 0

# Read file list
for d in dirs:
    files = glob.glob(d+'/**/*.*', recursive=True)
    for f in files:
        size = int(os.stat(f).st_size)

        name = f.encode('ascii')
        if f.endswith('/config.lua'): # Promote config to main directory
            name = 'config.lua'.encode('ascii')

        allItems.append((f, name, start, size))
        start = start + size

        allSize = allSize + len(name) + 10

# Write generated list
for i in allItems:
    ofile.write(struct.pack('H', len(i[1])))
    for l in i[1]:
        ofile.write(struct.pack('b', l))
    ofile.write(struct.pack('I', allSize+i[2]))
    ofile.write(struct.pack('I', i[3]))

    print(len(i[1]), i[0], '( from:', i[0], ')', allSize+i[2], i[3])

# End header
ofile.write(struct.pack('H', 0))


for i in allItems:
    ifile = open(i[0], 'rb')
    ofile.write(ifile.read())
    ifile.close()

ofile.close()

