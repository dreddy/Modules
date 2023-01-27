#!/usr/bin/env python

import os
import struct

fd = os.open("/dev/scratch", os.O_RDWR)
d = struct.pack("i", 0x10)
os.write(fd, d)
os.close(fd)
