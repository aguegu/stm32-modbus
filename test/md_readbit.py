#!/usr/bin/env python
import minimalmodbus

x = minimalmodbus.Instrument('/dev/ttyUSB0', 2) 
x.serial.parity = 'E'

print x.read_bit(0, 2)

print x.read_register(0, 0, 3)
print x.read_register(1, 0, 3)
print x.read_register(2, 0, 3)
print x.read_register(3, 0, 3)
