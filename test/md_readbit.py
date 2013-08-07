#!/usr/bin/env python
import minimalmodbus
import time

x = minimalmodbus.Instrument('/dev/ttyUSB0', 2) 
x.serial.parity = 'E'

for i in range(4):
	x.write_register(i, 1450, 0, 6)

for i in range(4):
	print x.read_register(i, 0, 3)

time.sleep(5)


for i in range(4):
	x.write_register(i, 1000, 0, 6)

for i in range(4):
	print x.read_register(i, 0, 3)

