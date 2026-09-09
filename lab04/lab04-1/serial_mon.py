#!/usr/bin/env python3
"""Serial monitor for the NUCLEO-F767ZI ST-Link VCP (USART3 @ 115200 8N1).

Usage:
    python3 lab04/serial_mon.py            # read-only monitor
    python3 lab04/serial_mon.py --hex      # show raw hex bytes instead
"""
import os
import sys
import time
import termios

PORT = "/dev/cu.usbmodem1203"
BAUD = 115200

if len(sys.argv) > 1 and sys.argv[1] == "--hex":
    HEX = True
else:
    HEX = False

fd = os.open(PORT, os.O_RDWR | os.O_NOCTTY)
attrs = termios.tcgetattr(fd)
attrs[4] = attrs[5] = termios.B115200  # input/output baud
attrs[2] = attrs[2] | termios.CREAD    # enable receiver
termios.tcsetattr(fd, termios.TCSANOW, attrs)
os.set_blocking(fd, False)

print(f"Monitoring {PORT} @ {BAUD} baud (Ctrl-C to quit)")
try:
    while True:
        try:
            chunk = os.read(fd, 4096)
            if HEX:
                sys.stdout.buffer.write(chunk.hex().encode() + b"\n")
            else:
                sys.stdout.buffer.write(chunk)
            sys.stdout.buffer.flush()
        except BlockingIOError:
            time.sleep(0.05)
except KeyboardInterrupt:
    pass
finally:
    os.close(fd)
