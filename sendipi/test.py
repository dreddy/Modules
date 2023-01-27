#!/usr/bin/env python3

import os
import struct

fd = os.open("/sys/kernel/debug/dr_send_spurious", os.O_RDWR)
d = struct.pack("i", 0x10)
os.write(fd, d)
os.close(fd)
